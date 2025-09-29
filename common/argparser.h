/**
 * @file    argparser.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <array>
#include <span>
#include <type_traits>

namespace ym
{

/** ArgParser
 *
 * @brief A parsing utility for use for command line arguments.
 * 
 * TODO add commands, like
 *      ./a.out status -a (note that status doesn't require a preceeding --).
 * TODO add completion hints ("--statis" leads to "did you mean --status?")
 *      of course this functionality will be guarded in a NOT_LITE build.
 *
 * @note Example usages follow:
 *       --input <value> // long hand arg with desired value
 *       --assert        // long hand flag set to enable
 *       -o <value>      // (output) short hand arg with desired value
 *       -c              // (clean) short hand flag set to enable
 *       -cb             // (clean; build) short hand (abbr pack) flags set to enable
 */
class ArgParser
{
public:
   /** Arg
    * 
    * @brief Provides type info for each parsable argument.
    * 
    * @note Flag based arguments are only able to be turned on, not off.
    *       For the opposite one can make "--no-..." argument.
    * 
    * @note Uses cascading.
    * 
    * @note The flags cannot be made constexpr - we need a const storage location for these
    *       variables since we do direct pointer comparisons. The compiler may optimize
    *       away storage locations for constexpr, breaking our use cases.
    *       static volatile constexpr is also allowed but appears to messy.
    *       c-style strings can be optimized such that any "0" or "1" can have the same
    *       pointer values, making "0" and "1" as integers interpreted as flags! My
    *       unittest doesn't catch this latter error because argparser is first compiled
    *       into a library, where optimization of combining string literals cannot
    *       occur - this is not true though when argparser is compiled into another
    *       source file. Also, volatile doesn't help.
    */
   class Arg
   {
      friend ArgParser;
      
   private:
      enum Flags_T : uint8 {
         FFlag = 0u,
         FEnbl,
         FList,
         FReqd
      };

   public:
      explicit Arg(str const Name);

      inline auto   getName (void) const { return _name;  }
      inline auto   getDesc (void) const { return _desc;  }
             rawstr getVal  (uint32 const Idx = 0_u32) const;
      inline auto   getAbbr (void) const { return _abbr;  }
      inline auto   getNVals(void) const { return _nvals; }

      inline auto isFlag (void) const { return _flags.test(FFlag); }
      inline auto isEnbl (void) const { return _flags.test(FEnbl); }
      inline auto isList (void) const { return _flags.test(FList); }
      inline auto isReqd (void) const { return _flags.test(FReqd); }

      inline Arg & desc  (rawstr const Desc       ) { _desc = Desc;            return *this; }
      inline Arg & defval(rawstr const DefaultVal ) { _val  = DefaultVal;      return *this; }
      inline Arg & abbr  (char   const Abbr       ) { _abbr = Abbr;            return *this; }
      inline Arg & enbl  (bool   const Enbl = true) { _flags.set(FFlag);
                                                      _flags.set(FEnbl, Enbl);
                                                      _val  = Enbl ? "1":"0";  return *this; }
      inline Arg & list  (bool   const List = true) { _flags.set(FList, List); return *this; }

      YM_DECL_YMASSERT(Error)

   // private:
      // no consts - see static assert below
      str           _name {""}; // arg name (used as the key)
      str           _desc {""}; // description
      rawstr        _val  {  }; // value
      uint32        _nvals{  }; // number of values, if list
      char          _abbr {  }; // abbreviation
      Bitset<uint8> _flags{  }; // flags
   };

   // copyable to load Arg params into vector
   // assignable for std::sort
   static_assert(std::is_copy_assignable_v<Arg>, "Arg needs to be copyable/assignable");

   /// @brief Result status of the parse
   enum class ParseResult_T : uint32
   {
      Success,
      Failure,
      HelpMenuCalled
   };

   explicit ArgParser(
      int            const Argc,       // command line arg count
      strlit const * const Argv_Ptr,   // command line args
      std::span<Arg>       argHandlers // user-defined arg handlers
   );

   explicit ArgParser(
      str      const Argv,       // command line args
      std::span<Arg> argHandlers // user-defined arg handlers
   );

   YM_NO_COPY  (ArgParser)
   YM_NO_ASSIGN(ArgParser)

   ParseResult_T parse(void);

          Arg const * get       (str const Key) const;
   inline Arg const * operator[](str const Key) const { return get(Key); }

   YM_DECL_YMASSERT(Error)
   YM_DECL_YMASSERT(Error, ParseError )
   YM_DECL_YMASSERT(Error, ArgError   )
   YM_DECL_YMASSERT(Error, AccessError)

private:
   static constexpr auto s_NValidChars = static_cast<uint32>('~' - '!' + 1); // 126 - 33 + 1
   static constexpr auto getNValidChars(void) { return s_NValidChars; }

   static constexpr auto isValidChar(char const Char) { return Char >= '!' && Char <= '~' && Char != '-'; }
   static constexpr auto getAbbrIdx (char const Abbr) { return Abbr - '!'; }

   using AbbrSet_T = std::array<Arg *, s_NValidChars>;

   void organizeAndValidateArgHandlerVector(void);

   void displayHelpMenu(void) const;

   rawstr getNextToken(void);

   Arg * getArgPtrFromPrefix(rawstr const Prefix);
   Arg * getArgPtrFromAbbr  (char   const Abbr  );

   ParseResult_T parseLonghand (rawstr token);
   ParseResult_T parseShorthand(rawstr token);

   ParseResult_T parseLonghand(
      Arg * const arg_Ptr,
      bool  const IsNeg = false);

   ParseResult_T parseShorthand(
      Arg * const arg_Ptr,
      bool  const MightHaveValue);

   /// @brief Helper type.
   union Argv_T
   {
      constexpr Argv_T(strlit const * const Vec_) : Vec{Vec_} {}
      constexpr Argv_T(str            const Str_) : Str{Str_} {}

      strlit const * const Vec; // array of args (as passed to main)
      str            const Str; // one string
   };

   /// @brief Helper type.
   union Idx_T
   {
      constexpr Idx_T(int    const Vec_idx_) : vec_idx{Vec_idx_} {}
      constexpr Idx_T(rawstr const Str_idx_) : str_idx{Str_idx_} {}

      int    vec_idx; // used for array of args (as passed to main)
      rawstr str_idx; // used for one string
   };
           
   AbbrSet_T      _abbrs      {       };
   std::span<Arg> _argHandlers{       };
   int    const   _Argc       {       };
   Argv_T const   _Argv       {nullptr};
   Idx_T          _tidx       {nullptr};
};

} // ym
