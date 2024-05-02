/**
 * @file    argparser.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "ymception.h"

#include <array>
#include <span>
#include <type_traits>

namespace ym
{

/** ArgParser
 *
 * @brief A parsing utility for use for command line arguments.
 * 
 * @note Singleton. Reasons being:
 *       1) Argparser has a static member so the Arg classes don't need
 *          to store an explicit pointer to the outer class, making them 24 bytes
 *          instead of 32 bytes.
 *       2) It makes sense to only parse the command line arguments once,
 *          since there is only one copy of them. This is a specialized
 *          parser, others like json and xml require much different implementations.
 *       3) Everyone has easy access to the instance, instead of relying on
 *          passing the instance explicitly, which can get annoying.
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
      friend class ArgParser; // for setVal() and enable()

   public:
      explicit Arg(str const Name);

      inline auto getName(void) const { return _name; }
      inline auto getDesc(void) const { return _desc; }
      inline auto getVal (void) const { return _val;  }
      inline auto getAbbr(void) const { return _abbr; }

      inline auto isFlag (void) const { return _flag; }
      inline auto isEnbl (void) const { return _enbl; }

      inline Arg & desc  (str  const Desc       ) { _desc = Desc;       return *this; }
      inline Arg & defval(str  const DefaultVal ) { _val  = DefaultVal; return *this; }
      inline Arg & abbr  (char const Abbr       ) { _abbr = Abbr;       return *this; }
      inline Arg & enbl  (bool const Enbl = true) { _flag = true;
                                                    _enbl = Enbl;
                                                    _val  = _enbl ? "1" : "0";
                                                                        return *this; }

   private:
      // no consts - see static assert below
      str  _name; // arg name (used as the key)
      str  _desc; // description
      str  _val;  // value
      char _abbr; // abbreviation
      bool _flag; // flag
      bool _enbl; // enabled
   };

   // copyable to load Arg params into vector
   // assignable for std::sort
   static_assert(std::is_copy_assignable_v<Arg>, "Arg needs to be copyable/assignable");

   explicit ArgParser(int         const Argc,         // command line arg count
                      str const * const Argv_Ptr,     // command line args
                      std::span<Arg>    argHandlers); // user-defined arg handlers

   YM_NO_COPY  (ArgParser)
   YM_NO_ASSIGN(ArgParser)

   void parse(void);

          Arg const * get       (str const Key) const;
   inline Arg const * operator[](str const Key) const { return get(Key); }

   YM_DECL_YMCEPT(ArgParserError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ParseError )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ArgError   )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AccessError)

private:
   static constexpr auto s_NValidChars = static_cast<uint32>('~' - '!' + 1u);
   static constexpr auto getNValidChars(void) { return s_NValidChars; }

   static constexpr auto isValidChar(char const Char) { return Char >= '!' && Char <= '~'; }
   static constexpr auto getAbbrIdx (char const Abbr) { return Abbr - '!'; }

   void organizeAndValidateArgHandlerVector(void);

   void displayHelpMenu(void) const;

   using ArgIt_T   = std::span<Arg>::iterator;
   using AbbrSet_T = std::array<ArgIt_T, s_NValidChars>;

   ArgIt_T getArgPtrFromKey   (str  const Key   ) const;
   ArgIt_T getArgPtrFromPrefix(str  const Prefix);
   ArgIt_T getArgPtrFromAbbr  (char const Abbr  );

   int32 parseArgSet(ArgIt_T argIt,
                     int32   idx) const;
                     
   AbbrSet_T           _abbrs;
   int32 const         _Argc;
   str   const * const _Argv_Ptr;
   std::span<Arg>      _argHandlers;
};

} // ym
