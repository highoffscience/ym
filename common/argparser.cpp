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
#include <string_view>
#include <utility>

/** ArgParser
 * 
 * @brief Initializes class variables.
 * 
 * @note ArgParser assumes ownership argHandlers.
 * 
 * @param Argc        -- Argument count  (as supplied from main()).
 * @param Argv        -- Argument vector (as supplied from main()).
 * @param argHandlers -- Array of argument handlers.
 */
ym::ArgParser::ArgParser(int const         Argc,
                         str const * const Argv_Ptr,
                         std::span<Arg>    argHandlers)
   : _Argc        {Argc         },
     _Argv_Ptr    {Argv_Ptr     },
     _argHandlers {argHandlers  },
     _abbrs       {/* default */}
{
   for (auto & argIt_ref : _abbrs)
   { // initialize iterators
      argIt_ref = _argHandlers.end();
   }
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

      auto name = _Argv_Ptr[i].get(); // ok manipulating raw pointer here - I think I know what I'm doing

      if (name[0_u32] == '-')
      { // arg found
         if (name[1_u32] == '-')
         { // longhand arg found
            name += 2_u32;

            if (std::strcmp(name, "help") == 0) // must be full name - no prefix allowed
            { // help menu requested
               displayHelpMenu();
            }
            else
            { // standard arg
               auto const ArgIT = getArgPtrFromPrefix(name);
               ArgParserError::check(ArgParserError::Tag_T::ParseError,
                  ArgIT != _argHandlers.end(), "Arg '%s' not registered", name);

               i = parseArgSet(ArgIT, i);
            }
         }
         else
         { // shorthand arg found
            name += 1_u32;

            auto const NAbbrs = std::strlen(name);
            ArgParserError::check(ArgParserError::Tag_T::ParseError,
               NAbbrs > 0_u32, "Expected abbr at count %d but none found", i);

            for (auto j = 0_u32; j < NAbbrs; ++j)
            { // go through all abbrs in pack

               auto const Abbr = name[j];

               if (Abbr == 'h')
               { // help menu requested
                  displayHelpMenu();
               }
               else
               { // standard arg
                  auto argIT = getArgPtrFromAbbr(Abbr);
                  ArgParserError::check(ArgParserError::Tag_T::ParseError,
                     argIT != _argHandlers.end(), "Abbr '%c' not registered", Abbr);

                  if (NAbbrs == 1_u32)
                  { // only 1 abbr - might have value
                     i = parseArgSet(argIT, i);
                     break;
                  }
                  else
                  { // abbr found
                     ArgParserError::check(ArgParserError::Tag_T::ParseError,
                        argIT->isFlag(), "Arg '%s' not a flag", argIT->getName().get());
                     argIT->enbl(true);
                  }
               }
            }
         }
      }
      else
      { // unexpected command line argument
         ArgParserError::check(ArgParserError::Tag_T::ParseError,
            false, "Argument '%s' was unexpected", name);
      }
   }
}

/** parse_Helper
 * 
 * @brief Sets the value depending on the type of argument.
 * 
 * @throws ArgParserError_ParseError -- If a parsing occurs.
 * 
 * @param argIt -- Pointer to argument.
 * @param idx   -- Current index of command line argument.
 * 
 * @returns int32 -- Updated index of command line argument.
 */
auto ym::ArgParser::parseArgSet(ArgIt_T argIt,
                                int32   idx) const -> int32
{
   if (argIt->isFlag())
   { // enable argument - no explicit value
      argIt->enbl(true);
   }
   else
   { // value is next command line argument
      idx += 1_i32;
      ArgParserError::check(ArgParserError::Tag_T::ParseError,
         idx < _Argc, "No value for arg '%s'", argIt->getName().get());
      argIt->defval(_Argv_Ptr[idx]);
   }

   return idx;
}

/** getArgPtr
 * 
 * @brief Returns the registered argument info associated with the given key.
 * 
 * @note std::binary_search doesn't return a pointer to the found object, which is
 *       why we use a custom binary search function.
 * 
 * @throws ArgParserError_AccessError -- If no argument with the given name found.
 * 
 * @param Key -- Name of argument.
 * 
 * @returns ArgCIt_T -- Found argument.
 */
auto ym::ArgParser::get(str const Key) const -> ArgCIt_T
{
   auto const ArgCIt = ymBinarySearch(_argHandlers.cbegin(), _argHandlers.cend(), Key,
      [](str const Key, ArgCIt_T const & ArgCIt) -> int32 {
         return std::strcmp(Key, ArgCIt->getName());
      }
   );

   ArgParserError::check(ArgParserError::Tag_T::AccessError,
      ArgCIt != _argHandlers.cend(), "Key '%s' not found", Key.get());

   return ArgCIt;
}

/** organizeAndValidateArgHandlerVector
 * 
 * @brief Sorts and checks for duplicate args.
 * 
 * @throws ArgParserError          -- If std::sort fails.
 * @throws ArgParserError_ArgError -- If there is a duplicate argument (argument with the same name).
 * @throws ArgParserError_ArgError -- If name is null or empty or invalid.
 * @throws ArgParserError_ArgError -- If description is null or empty.
 * @throws ArgParserError_ArgError -- If the abbreviation is invalid or
 *         has been previously set.
 * @throws ArgParserError_ArgError -- If flag has been set and value is invalid.
 * @throws ArgParserError_ArgError -- If Enabled but not marked as a flag.
 */
void ym::ArgParser::organizeAndValidateArgHandlerVector(void)
{
   // --- --- --- --- organize --- --- --- ---

   try
   { // std::sort may fail
      // searching algorithms require keys be sorted
      std::sort(_argHandlers.begin(), _argHandlers.end(),
         [](Arg const & Lhs, Arg const & Rhs) -> bool {
            return std::strcmp(Lhs.getName(), Rhs.getName()) < 0;
         }
      );
   }
   catch(std::exception const & E)
   { // throw custom error
      ArgParserError::check(ArgParserError::Tag_T::ParseError,
         false, "std::sort failed with msg '%s'", E.what());
   }

   // --- --- --- --- validate --- --- --- ---

   for (auto it = _argHandlers.begin(); it != _argHandlers.end(); it++)
   { // go through all args

      auto const Key  = it->getName();
      auto const Desc = it->getDesc();
      auto const Val  = it->getVal ();
      auto const Abbr = it->getAbbr();

      // --- --- detect duplicate keys --- ---

      if (it != _argHandlers.begin())
      { // compare current key to previous key
         ArgParserError::check(ArgParserError::Tag_T::ArgError,
            std::strcmp(Key, (it - 1_u32)->getName()) != 0, "Duplicate key '%s'", Key.get());
      }

      // --- --- validate key name --- ---

      ArgParserError::check(ArgParserError::Tag_T::ArgError,
         ymIsStrNonEmpty(Key), "Name must be non-empty");
      ArgParserError::check(ArgParserError::Tag_T::ArgError,
         std::strcmp(Key, "help") != 0, "Arg cannot be named the reserved word 'help'");

      // dereference here is safe, we just checked that above
      for (auto currChar = Key.get(); *currChar != '\0'; ++currChar)
      { // go through all chars in proposed name
         ArgParserError::check(ArgParserError::Tag_T::ArgError,
            isValidChar(*currChar), "Name '%s' cannot contain '%c'", Key.get(), *currChar);
      }

      // --- --- validate description --- ---

      ArgParserError::check(ArgParserError::Tag_T::ArgError,
         ymIsStrNonEmpty(Desc), "Description must be non-empty");

      // --- --- validate value --- ---

      // nothing to check here - value/flag consistency tested below

      // --- --- validate abbreviation --- ---

      if (Abbr != '\0')
      { // assigned abbr
         ArgParserError::check(ArgParserError::Tag_T::ParseError,
            isValidChar(Abbr), "Abbr 0x%x not valid for arg '%s'", Abbr, Key.get());
         ArgParserError::check(ArgParserError::Tag_T::ParseError,
            _abbrs[getAbbrIdx(Abbr)] == _argHandlers.end(), "Abbr '%c' already occupied", Abbr);

         _abbrs[getAbbrIdx(Abbr)] = it;
      }

      // --- --- validate flag --- ---

      if (it->isFlag())
      { // marked as flag
         ArgParserError::check(ArgParserError::Tag_T::ArgError,
            std::strcmp(Val, "0") == 0 || std::strcmp(Val, "1") == 0,
            "Arg '%s' is a flag - cannot have arbitrary value", Key.get());
      }
      else
      { // not a flag
         ArgParserError::check(ArgParserError::Tag_T::ArgError,
            !it->isEnbl(), "Arg '%s' cannot be enabled and not marked as a flag", Key.get());
      }

      // --- --- validate enable --- ---

      if (it->isEnbl())
      { // enabled - better be a flag
         ArgParserError::check(ArgParserError::Tag_T::ArgError,
            it->isFlag(), "Arg '%s' is marked as enabled but is not a flag", Key.get());
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

   for (auto const & Arg : _argHandlers)
   { // go through all registered arguments
      if (auto const KeyLen = std::strlen(Arg.getName()); KeyLen > maxKeyLen)
      { // update max key length
         maxKeyLen = KeyLen;
      }
   }

   auto spaces_bptr = bptr<char>(YM_STACK_ALLOC(char, maxKeyLen + 1_u32));
   for (auto i = 0_u32; i < maxKeyLen; ++i)
   { // init all elements to spaces
      spaces_bptr[i] = ' ';
   }
   spaces_bptr[maxKeyLen] = '\0';

   ymLog(VG::ArgParser, "ArgParser help menu:");

   for (auto const & Arg : _argHandlers)
   { // go through all registered arguments
      auto const KeyLen = std::strlen(Arg.getName());
      ymLog(VG::ArgParser, " --%s%s : %s", Arg.getName().get(), spaces_bptr + KeyLen, Arg.getDesc().get());

      if (auto const Abbr = Arg.getAbbr(); isValidChar(Abbr))
      { // this arg has an abbreviation
         ymLog(VG::ArgParser, "   (-%c)", Abbr);
      }
   }
}

/** getArgPtrFromPrefix
 * 
 * @brief Returns the registered argument info associated with the given prefix.
 * 
 * @throws ArgParserError_ParseError -- If prefix is ambiguous.
 * @throws ArgParserError_ParseError -- If prefix is not found.
 * 
 * @param Prefix -- Prefix of argument.
 * 
 * @returns ArgIt_T -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromPrefix(str const Prefix) -> ArgIt_T
{
   std::string_view const PrefixSV(Prefix);

   // returns first iterator where comp() evals to false
   auto const LowerBoundArgIt = std::lower_bound(_argHandlers.begin(), _argHandlers.end(), PrefixSV,
      // should return true if Arg is ordered before Prefix
      [](Arg const & Arg, std::string_view const & PrefixSV) -> bool {
         return std::strncmp(Arg.getName(), PrefixSV.data(), PrefixSV.size()) < 0;
      }
   );

   auto searchedForArgIt = _argHandlers.end();

   for (auto argIt = LowerBoundArgIt; argIt != _argHandlers.end(); argIt++)
   { // search through all stored args

      auto const Cmp = std::strncmp(PrefixSV.data(), argIt->getName(), PrefixSV.size());

      if (Cmp == 0)
      { // prefix found (whole prefix matched)
         if (std::strlen(argIt->getName()) == PrefixSV.size())
         { // entire key matched
            searchedForArgIt = argIt;
            break;
         }
         else
         { // prefix matched but maybe others will match so continue search
           // if another match is found it will trigger the assert below
            ArgParserError::check(ArgParserError::Tag_T::ParseError,
               argIt == _argHandlers.end(), "Prefix '%s' is ambiguous", PrefixSV.data());
            searchedForArgIt = argIt;
         }
      }
      else if (Cmp < 0)
      { // not a match - end search
         break;
      }
   }

   ArgParserError::check(ArgParserError::Tag_T::ParseError,
      searchedForArgIt != _argHandlers.end(), "Prefix '%s' doesn't match any handlers", PrefixSV.data());

   return searchedForArgIt;
}

/** getArgPtrFromAbbr
 * 
 * @brief Returns the registered argument info associated with the given abbreviation.
 * 
 * @param Abbr -- Abbreviation of argument.
 * 
 * @returns ArgIt_T -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromAbbr(char const Abbr) -> ArgIt_T
{
   ArgParserError::check(ArgParserError::Tag_T::ParseError,
      isValidChar(Abbr), "Abbr 0x%x not valid", Abbr);

   auto const SearchedForArgIt = _abbrs[getAbbrIdx(Abbr)];

   ArgParserError::check(ArgParserError::Tag_T::ParseError,
      SearchedForArgIt != _argHandlers.end(), "Abbr '%c' not registered", Abbr);

   return SearchedForArgIt;
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
