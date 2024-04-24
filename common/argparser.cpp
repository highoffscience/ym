/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "argparser.h"

#include "memio.h"
#include "textlogger.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <utility>

/** ArgParser
 * 
 * @brief Initializes class variables.
 * 
 * @note ArgParser assumes ownership argHandlers.
 * 
 * @param Argc            -- Argument count  (as supplied from main()).
 * @param Argv_Ptr        -- Argument vector (as supplied from main()).
 * @param argHandlers_Ptr -- Array of argument handlers.
 * @param NArgHandlers    -- Number of argument handlers.
 */
ym::ArgParser::ArgParser(int            const Argc,
                         str    const * const Argv_Ptr,
                         Arg          * const argHandlers_Ptr,
                         uint32         const NArgHandlers)
   : _abbrs           { /* default */ },
     _Argc            {Argc           },
     _Argv_Ptr        {Argv_Ptr       },
     _argHandlers_Ptr {argHandlers_Ptr},
     _NArgHandlers    {NArgHandlers   }
{
   // TODO assert
}

/** parse
 *
 * @brief Parses through the command line arguments and populates registered args.
 * 
 * @throws ArgParserError_ParseError -- If a parsing error occurs.
 */
void ym::ArgParser::parse(void)
{
   organizeAndValidateArgHandlerVector();

   for (auto i = 1_i32; i < _Argc; ++i)
   { // go through all command line arguments
      auto name = _Argv_Ptr[i];

      if (name[0_u32] == '-')
      { // arg found
         if (name[1_u32] == '-')
         { // longhand arg found
            name += 2_u32;

            if (std::strcmp(name, "help") == 0_i32) // must be full name - no prefix allowed
            { // help menu requested
               displayHelpMenu();
            }
            else
            { // standard arg
               auto * const arg_Ptr = getArgPtrFromPrefix(name);
               ArgParserError_ParseError::check(arg_Ptr, "Arg '%s' not registered", name);

               i = parseArgSet(arg_Ptr, i);
            }
         }
         else
         { // shorthand arg found
            name += 1_u32;

            auto const NAbbrs = std::strlen(name);
            ArgParserError_ParseError::check(NAbbrs > 0_u32, "Expected abbr at count %d but none found", i);

            for (auto j = 0_u32; j < NAbbrs; ++j)
            { // go through all abbrs in pack

               auto const Abbr = name[j];

               if (Abbr == 'h')
               { // help menu requested
                  displayHelpMenu();
               }
               else
               { // standard arg
                  auto * const arg_Ptr = getArgPtrFromAbbr(Abbr);
                  ArgParserError_ParseError::check(arg_Ptr, "Abbr '%c' not registered", Abbr);

                  if (NAbbrs == 1_u32)
                  { // only 1 abbr - might have value
                     i = parseArgSet(arg_Ptr, i);
                     break;
                  }
                  else
                  { // abbr found
                     ArgParserError_ParseError::check(arg_Ptr->isFlag(), "Arg '%s' not a flag", arg_Ptr->getName());
                     arg_Ptr->enbl(true);
                  }
               }
            }
         }
      }
      else
      { // unexpected command line argument
         ArgParserError_ParseError::check(false, "Argument '%s' was unexpected", name);
      }
   }
}

/** parse_Helper
 * 
 * @brief Sets the value depending on the type of argument.
 * 
 * @note Since this is a helper function, null checking of the param
 *       arg_Ptr has already been done.
 * 
 * @throws ArgParserError_ParseError -- If a parsing occurs.
 * 
 * @param arg_Ptr -- Pointer to argument.
 * @param idx     -- Current index of command line argument.
 * 
 * @returns int32 -- Updated index of command line argument.
 */
auto ym::ArgParser::parseArgSet(Arg   * const arg_Ptr,
                                int32         idx) const -> int32
{
   if (arg_Ptr->isFlag())
   { // enable argument - no explicit value
      arg_Ptr->enbl(true);
   }
   else
   { // value is next command line argument
      idx += 1_i32;
      ArgParserError_ParseError::check(idx < _Argc, "No value for arg '%s'", arg_Ptr->getName());
      arg_Ptr->defval(_Argv_Ptr[idx]);
   }

   return idx;
}

/** getArgPtr
 * 
 * @brief Returns the registered argument info associated with the given key.
 * 
 * @note std::binary_search doesn't return a pointer to the found object, which is
 *       why we use std::bsearch.
 * 
 * @throws ArgParserError_AccessError -- If no argument with the given name found.
 * 
 * @param Key -- Name of argument.
 * 
 * @returns Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::get(str const Key) const -> Arg const *
{
   auto const * const Arg_Ptr = getArgPtrFromKey(Key);
   ArgParserError_AccessError::check(Arg_Ptr, "Key '%s' not found", Key);
   return Arg_Ptr;
}

/** organizeAndValidateArgHandlerVector
 * 
 * @brief Sorts and checks for duplicate args.
 * 
 * @throws ArgParserError_ParseError -- If there is a duplicate argument
 *         (argument with the same name).
 * @throws ArgParserError_ArgError -- If name is null or empty or invalid.
 * @throws ArgParserError_ArgError -- If description is null or empty.
 * @throws ArgParserError_ArgError -- If the abbreviation is invalid or
 *         has been previously set.
 * @throws ArgParserError_ArgError -- If flag has been set and value is invalid.
 */
void ym::ArgParser::organizeAndValidateArgHandlerVector(void)
{
   // --- --- --- --- organize --- --- --- ---

   // searching algorithms require keys be sorted
   std::sort(_argHandlers_Ptr, _argHandlers_Ptr + _NArgHandlers,
      [](Arg const & Lhs, Arg const & Rhs) -> bool {
         return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
      }
   );

   // --- --- --- --- validate --- --- --- ---

   for (auto i = 0_u32; i < _NArgHandlers; ++i)
   { // go through all args

      auto * const arg_Ptr = _argHandlers_Ptr + i;
      auto   const Key     = arg_Ptr->getName();
      auto   const Desc    = arg_Ptr->getDesc();
      auto   const Val     = arg_Ptr->getVal ();
      auto   const Abbr    = arg_Ptr->getAbbr();

      // --- --- detect duplicate keys --- ---

      if (i > 0_u32)
      { // compare current key to previous key
         ArgParserError_ParseError::check(
            std::strcmp(_argHandlers_Ptr[i-1_u32].getName(), Key) != 0_i32,
            "Duplicate key '%s'", Key);
      }

      // --- --- validate key name --- ---

      ArgParserError_ArgError::check(ymIsStrNonEmpty(Key), "Name must be non-empty");
      ArgParserError_ArgError::check(std::strcmp(Key, "help") != 0_i32,
         "Arg cannot be named the reserved word 'help'");

      // dereference here is safe, we just checked that above
      for (auto currChar = Key; *currChar != '\0'; ++currChar)
      { // go through all chars in proposed name
         ArgParserError_ArgError::check(isValidChar(*currChar),
            "Name '%s' cannot contain '%c'", Key, *currChar);
      }

      // --- --- validate description --- ---

      ArgParserError_ArgError::check(ymIsStrNonEmpty(Desc), "Description must be non-empty");

      // --- --- validate value --- ---

      // nothing to check here - value/flag consistency tested below

      // --- --- validate abbreviation --- ---

      ArgParserError_ParseError::check(isValidChar(Abbr),
         "Abbr 0x%x not valid for arg '%s'", Abbr, Key);
      ArgParserError_ParseError::check(_abbrs[getAbbrIdx(Abbr)], "Abbr '%c' already occupied", Abbr);

      _abbrs[getAbbrIdx(Abbr)] = arg_Ptr;

      // --- --- validate flag --- ---

      if (arg_Ptr->isFlag())
      { // marked as flag
         ArgParserError_ArgError::check(std::strcmp(Val, "0") == 0 || std::strcmp(Val, "1") == 0,
            "Arg '%s' is a flag - cannot have arbitrary value", Key);
      }
      else
      { // not a flag
         ArgParserError_ArgError::check(!arg_Ptr->isEnbl(),
            "Arg '%s' cannot be enabled and not marked as a flag", Key);
      }

      // --- --- validate enable --- ---

      if (arg_Ptr->isEnbl())
      { // enabled - better be a flag
         ArgParserError_ArgError::check(arg_Ptr->isFlag(),
            "Arg '%s' is marked as enabled but is not a flag", Key);
      }
   }
}

/** displayHelpMenu
 * 
 * @brief Prints the help menu.
 */
void ym::ArgParser::displayHelpMenu(void) const
{
   auto const SE = ymLogPushEnable(VG::ArgParser);

   auto maxKeyLen = 0_u32;

   for (auto i = 0_u32; i < _NArgHandlers; ++i)
   { // go through all registered arguments
      if (auto const KeyLen = std::strlen(_argHandlers_Ptr[i].getName()); KeyLen > maxKeyLen)
      { // update max key length
         maxKeyLen = KeyLen;
      }
   }

   auto * const spaces_Ptr = YM_STACK_ALLOC(char, maxKeyLen + 1_u32);
   for (auto i = 0_u32; i < maxKeyLen; ++i)
   { // init all elements to spaces
      spaces_Ptr[i] = ' ';
   }
   spaces_Ptr[maxKeyLen] = '\0';

   ymLog(VG::ArgParser, "ArgParser help menu:");

   for (auto i = 0_u32; i < _NArgHandlers; ++i)
   { // go through all registered arguments
      auto const KeyLen = std::strlen(_argHandlers_Ptr[i].getName());
      ymLog(VG::ArgParser, " --%s%s : %s", _argHandlers_Ptr[i].getName(), spaces_Ptr + KeyLen, _argHandlers_Ptr[i].getDesc());

      if (auto const Abbr = _argHandlers_Ptr[i].getAbbr(); isValidChar(Abbr))
      { // this arg has an abbreviation
         ymLog(VG::ArgParser, "   (-%c)", Abbr);
      }
   }
}

/** getArgPtrFromKey
 * 
 * @brief Returns the registered argument info associated with the given key.
 * 
 * @note std::binary_search doesn't return a pointer to the found object, which is
 *       why we use std::bsearch.
 * 
 * @note This function calls non-const functions that don't modify but return
 *       non-const references so this function cannot be declared const.
 *       Technically we can because the compiler is fooled by the algorithm
 *       we use. The void * returned by std::bsearch can incorrectly cast
 *       to a non-const reference.
 * 
 * @param Key -- Name of argument.
 * 
 * @returns Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::getArgPtrFromKey(str const Key) const -> Arg *
{
   return (!Key) ? nullptr :
      static_cast<Arg *>(
         std::bsearch(Key, _argHandlers_Ptr, _NArgHandlers, sizeof(Arg),
            // use of int (not int32) for legacy portability
            [](void const * Lhs_ptr, void const * Rhs_ptr) -> int {
               return std::strcmp(static_cast<str>(Lhs_ptr),
                                  static_cast<Arg const *>(Rhs_ptr)->getName());
            }
         )
      );
}

/** getArgPtrFromPrefix
 * 
 * @brief Returns the registered argument info associated with the given prefix.
 * 
 * @throws ArgParserError_ParseError -- If prefix is ambiguous.
 * 
 * @param Prefix -- Prefix of argument.
 * 
 * @returns Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::getArgPtrFromPrefix(str const Prefix) -> Arg *
{
   Arg *      arg_ptr   = nullptr;
   auto const PrefixLen = std::strlen(Prefix);

   for (auto i = 0_u32; i < _NArgHandlers; ++i)
   { // search through all stored args
      auto const Cmp = std::strncmp(Prefix, _argHandlers_Ptr[i].getName(), PrefixLen);

      if (Cmp == 0_i32)
      { // prefix found (whole prefix matched)
         if (std::strlen(_argHandlers_Ptr[i].getName()) == PrefixLen)
         { // entire key matched
            arg_ptr = _argHandlers_Ptr + i;
            break;
         }
         else
         { // prefix matched but maybe others will match so continue search
           // if another match is found it will trigger the assert below
            ArgParserError_ParseError::check(!arg_ptr, "Prefix '%s' is ambiguous", Prefix);
            arg_ptr = _argHandlers_Ptr + i;
         }
      }
      else if (Cmp < 0_i32)
      { // not a match - end search
         break;
      }
   }

   return arg_ptr;
}

/** getArgPtrFromAbbr
 * 
 * @brief Returns the registered argument info associated with the given abbreviation.
 * 
 * @param Abbr -- Abbreviation of argument.
 * 
 * @returns Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::getArgPtrFromAbbr(char const Abbr) -> Arg *
{
   ArgParserError_ParseError::check(isValidChar(Abbr), "Abbr 0x%x not valid", Abbr);

   return _abbrs[getAbbrIdx(Abbr)];
}

// ---------------------------------- Arg ----------------------------------

/** Arg
 * 
 * @brief Constructor.
 * 
 * @note Names must start with an alphanumeric character.
 * 
 * @param Name -- Name of argument.
 */
ym::ArgParser::Arg::Arg(str const Name)
   : _name {Name },
     _desc {""   },
     _val  {""   },
     _abbr {'\0' },
     _flag {false},
     _enbl {false}
{
}
