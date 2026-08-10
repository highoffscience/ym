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
 *      Commands should stack, like
 *      ./a.out report status console
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
      /// @brief Options for arguments. Flags are stored in bitset so enum class would be cumbersome.
      enum Flags_T : std::size_t {
         FFlag,
         FEnbl,
         FList,
         FReqd,
         FCmd // TODO not yet implemented
      };

   public:
      explicit Arg(strlit const Name);

      inline auto   getName (void) const { return _name;  }
      inline auto   getDesc (void) const { return _desc;  }
             optstr getVal  (std::size_t const Idx = 0uz) const;
      inline auto   getAbbr (void) const { return _abbr;  }
      inline auto   getNVals(void) const { return _nvals; }

      inline auto isFlag (void) const { return _flags.test(FFlag); }
      inline auto isEnbl (void) const { return _flags.test(FEnbl); }
      inline auto isList (void) const { return _flags.test(FList); }
      inline auto isReqd (void) const { return _flags.test(FReqd); }
      inline auto isCmd  (void) const { return _flags.test(FCmd ); }

      inline Arg & desc  (strlit const Desc       ) { _desc = Desc;            return *this; }
      inline Arg & defval(str    const DefaultVal ) { _val  = DefaultVal;      return *this; }
      inline Arg & abbr  (char   const Abbr       ) { _abbr = Abbr;            return *this; }
      inline Arg & enbl  (bool   const Enbl = true) { _flags.set(FFlag);
                                                      _flags.set(FEnbl, Enbl);
                                                      _val  = Enbl ? "1":"0";  return *this; }
      inline Arg & list  (bool   const List = true) { _flags.set(FList, List); return *this; }
      inline Arg & reqd  (bool   const Reqd = true) { _flags.set(FReqd, Reqd); return *this; }
      inline Arg & cmd   (bool   const Cmd  = true) { _flags.set(FCmd,  Cmd ); return *this; }

      // TODO with lite build can just have error and alias ArgError, AccesError, and ParseError
      YM_DECL_YMASSERT(Error)

   private:
      // no consts - see static assert below
      strlit     _name {""}; // arg name (used as the key)
      strlit     _desc {""}; // description
      optstr     _val  {  }; // value
      uint32     _nvals{  }; // number of values, if list
      char       _abbr {  }; // abbreviation
      ByteBitset _flags{  }; // flags
   };

   // copyable to load Arg params into vector
   // assignable for std::sort
   static_assert(std::is_copy_assignable_v<Arg>, "Arg needs to be copyable/assignable");

   /// @brief Result status of the parse
   enum class ParseResult_T
   {
      Success,
      Failure,
      HelpMenuCalled
   };

   explicit ArgParser(
      int                 const Argc,       // command line arg count
      bound<rawstr const> const Argv_Ptr,   // command line args
      std::span<Arg>            argHandlers // user-defined arg handlers
   );

   explicit ArgParser(
      str      const Argv,       // command line args
      std::span<Arg> argHandlers // user-defined arg handlers
   );

   YM_NO_COPY  (ArgParser)
   YM_NO_ASSIGN(ArgParser)

   ParseResult_T parse(void);

          bound<Arg const> get       (str const Key) const;
   inline bound<Arg const> operator[](str const Key) const { return get(Key); }

   YM_DECL_YMASSERT(Error)

private:
   static constexpr auto isValidChar(char const Char) { return Char >= '!' && Char <= '~' && Char != '-'; }
   static constexpr auto getAbbrIdx (char const Abbr) { return static_cast<unsigned>(Abbr - '!'); }

   static constexpr auto s_NValidChars = static_cast<unsigned>('~' - '!' + 1); // 126 - 33 + 1
   using AbbrSet_T = std::array<loose<Arg>, s_NValidChars>;

   void organizeAndValidateArgHandlerVector(void);

   void displayHelpMenu(void) const;

   optstr getNextToken(void);

   bound<Arg> getArgPtrFromPrefix(str  const Prefix);
   bound<Arg> getArgPtrFromAbbr  (char const Abbr  );

   ParseResult_T parseLonghand (str token);
   ParseResult_T parseShorthand(str token);

   ParseResult_T parseLonghand(
      bound<Arg> const arg_Ptr,
      bool       const IsNeg = false);

   ParseResult_T parseShorthand(
      bound<Arg> const arg_Ptr,
      bool       const MightHaveValue);

   /// @brief Helper type.
   union Argv_T
   {
      constexpr Argv_T(bound<rawstr const> const Vec_) noexcept : Vec{Vec_} {}
      constexpr Argv_T(str /* - - - - - */ const Str_) noexcept : Str{Str_} {}

      bound<rawstr const> const Vec; // array of args (as passed to main)
      str /* - - - - - */ const Str; // one string
   };

   /// @brief Helper type.
   union Idx_T
   {
      constexpr Idx_T(void) noexcept = default;
      constexpr Idx_T(int const Vec_idx_) noexcept : vec_idx{Vec_idx_} {}
      constexpr Idx_T(str const Str_idx_) noexcept : str_idx{Str_idx_} {}

      int    vec_idx;   // used for array of args (as passed to main)
      optstr str_idx{}; // used for one string
   };

   AbbrSet_T      _abbrs      {};
   std::span<Arg> _argHandlers{};
   int    const   _Argc       {};
   Argv_T const   _Argv; // no default ctor
   Idx_T          _tidx       {};
};

} // ym
