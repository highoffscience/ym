/**
 * @file    argparser.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "ymception.h"

#include <array>
#include <type_traits>
#include <vector>

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
private:
   /** Arg
    * 
    * @brief Provides type info for each parsable argument.
    * 
    * @note Flag based arguments are only able to be turned on, not off.
    *       For the opposite one can make "--no-..." argument.
    * 
    * @note Uses cascading.
    */
   class Arg
   {
   public:
      friend class ArgParser; // for setVal() and enable()

      explicit Arg(str const Name);

      inline auto getName(void) const { return _name; }
      inline auto getDesc(void) const { return _desc; }
      inline auto getVal (void) const { return _val;  }

      inline auto isFlag (void) const { return _flag; }
      inline auto isEnbl (void) const { return _enbl; }

      Arg & desc(str  const Desc      );
      Arg & val (str  const DefaultVal);
      Arg & abbr(char const Abbr      );
      Arg & flag(void                 ); // defaults to false

   private:
      void setVal(str  const Val );
      void enable(bool const Enbl);

      // no consts - see static assert below
      str  _name; // arg name (used as the key)
      str  _desc; // description
      str  _val;  // value
      bool _flag; // flag
      bool _enbl; // enabled
   };

   // copyable to load Arg params into vector
   // assignable for std::sort
   static_assert(std::is_copy_assignable_v<Arg>, "Arg needs to be copyable/assignable");

   explicit ArgParser(void);

public:
   YM_NO_COPY  (ArgParser)
   YM_NO_ASSIGN(ArgParser)

   static ArgParser * getInstancePtr(void);

   inline Arg arg(str const Name) const;

   void parse(std::vector<Arg> && args_uref,
              int const           Argc,
              str const * const   Argv_Ptr);

          Arg const * get       (str const Key);
   inline Arg const * operator[](str const Key);

   bool isSet(str const Key);

   YM_DECL_YMCEPT(ArgParserError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_CreationError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ParseError   )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ArgError     )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AccessError  )

private:
   void organizeAndValidateArgVector(void);

   void displayHelpMenu(void) const;

   char  getAbbrFromKey     (str  const Key   ) const;

   Arg * getArgPtrFromKey   (str  const Key   );
   Arg * getArgPtrFromPrefix(str  const Prefix);
   Arg * getArgPtrFromAbbr  (char const Abbr  );

   uint32 getAbbrIdx(char const Abbr) const;

   int32 parse_Helper(Arg       * const arg_Ptr,
                      int32             idx,
                      int32       const Argc,
                      str const * const Argv_Ptr) const;

   std::vector<Arg>        _args;
   std::array<str, 62_u32> _abbrs; // stores the keys
};

/** arg
 * 
 * @brief Creates a parsabble argument.
 * 
 * @note Class Arg is private so we need this method to create the argument.
 * 
 * @param Name -- Name (key) of argument.
 * 
 * @returns Arg -- Empty named argument.
 */
inline auto ym::ArgParser::arg(str const Name) const -> Arg
{
   return Arg(Name);
}

/** operator[]
 * 
 * @brief Returns the registered argument info associated with the given key.
 * 
 * @throws ArgParserError_AccessError -- If no argument with the given name found.
 * 
 * @param Key -- Name of argument.
 * 
 * @returns Arg * -- Found argument, or null if no argument found.
 */
inline auto ym::ArgParser::operator[](str const Key) -> Arg const *
{
   return get(Key);
}

} // ym
