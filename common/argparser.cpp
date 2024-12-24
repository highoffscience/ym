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
#include <iterator>
#include <string_view>
#include <utility>

/** ArgParser
 * 
 * @brief Initializes class variables.
 * 
 * @note ArgParser assumes ownership argHandlers, excluding it's destruction.
 *       Param argHandlers must outlive the ArgParser instance.
 * 
 * @note ArgParser assumes ownership Argv_Ptr, excluding it's destruction.
 *       Argv_Ptr must outlive the ArgParser instance.
 * 
 * @param Argc        -- Argument count  (as supplied from main()).
 * @param Argv_Ptr    -- Argument vector (as supplied from main()).
 * @param argHandlers -- Array of argument handlers.
 */
ym::ArgParser::ArgParser(
   int            const Argc,
   strlit const * const Argv_Ptr,
   ArgHandlers_T     argHandlers
#if (!YM_ARGPARSER_USE_STD_SPAN)
   , uint32       const NHandlers
#endif
)
   : _abbrs       {/*default*/},
     _argHandlers {argHandlers},
     _Argc        {Argc       },
     _Argv        {Argv_Ptr   }
#if (!YM_ARGPARSER_USE_STD_SPAN)
   , _NHandlers   {NHandlers  }
#endif
{
   init();
}

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
ym::ArgParser::ArgParser(
   str     const Argv,
   ArgHandlers_T argHandlers
#if (!YM_ARGPARSER_USE_STD_SPAN)
   ,
   uint32  const NHandlers
#endif
)
   : _abbrs       {/*default*/},
     _argHandlers {argHandlers},
     _Argc        {-1_i32     },
     _Argv        {Argv       }
   #if (!YM_ARGPARSER_USE_STD_SPAN)
   , _NHandlers   {NHandlers  }
   #endif
{
   init();
}

/** init
 * 
 * @brief Initializes all abbreviation pointers.
 */
void ym::ArgParser::init(void)
{
   for (auto i = 0_u64; i < _abbrs.size(); ++i)
   { // initialize ptrs
      _abbrs[i] = nullptr;
   }
}

/** parse
 *
 * @brief Parses through the command line arguments and populates registered args.
 * 
 * @throws ParseError -- If a parsing error occurs.
 * 
 * @returns bool -- True if the parse succeeds, false otherwise.
 */
bool ym::ArgParser::parse(void)
{
   organizeAndValidateArgHandlerVector();

   auto succeed = true; // until told otherwise

   for (rawstr token = nullptr; token = getNextToken(token); /*nothing*/)
   { // go through all command line arguments

      if (token[0_u32] == '-')
      { // arg found

         if (token[1_u32] == '-')
         { // longhand arg found

            token = parseLonghand(token);
            if (!token)
            { // 
               succeed = false;
               break;
            }
         }
         else
         { // shorthand arg found
            name += 1_u32;

            auto const NAbbrs = std::strlen(name);
            ParseError::check(NAbbrs > 0_u32, "Expected abbr at count %d but none found", i);

            for (auto j = 0_u32; j < NAbbrs; ++j)
            { // go through all abbrs in pack

               auto const Abbr = name[j];

               if (Abbr == 'h')
               { // help menu requested
                  displayHelpMenu();
               }
               else
               { // standard arg
                  auto argPtr = getArgPtrFromAbbr(Abbr);

                  if (NAbbrs == 1_u32)
                  { // only 1 abbr - might have value
                     i = parseArgSet(argPtr, i);
                     break;
                  }
                  else
                  { // abbr found
                     ParseError::check(argPtr->isFlag(), "Arg '%s' not a flag", argPtr->getName().get());
                     argPtr->enbl(true);
                  }
               }
            }
         }
      }
      else
      { // unexpected command line argument
         ParseError::check(false, "Argument '%s' was unexpected", name);
      }
   }
}

/**
 * TODO
 * 
 */
auto ym::ArgParser::parseLonghand(rawstr token) -> rawstr
{
   token += 2_u32; // move past "--"

   if (std::strcmp(token, "help") == 0) // must be full name - no prefix allowed
   { // help menu requested
      displayHelpMenu();
      token = nullptr;
   }
   else
   { // standard arg
      
      auto isNegation = false; // until told otherwise

      if (auto newToken = getArgNegation(token); newToken != token)
      { // arg prefixed by --no- - negation on flag
         isNegation = true;
         token = newToken;
      }

      auto const ArgPtr = getArgPtrFromPrefix(token);
      token = parseArg(ArgPtr, token, isNegation);
   }

   return token;
}

/** parseArg
 * 
 * @brief Sets the value depending on the type of argument.
 * 
 * @throws ParseError -- If a parsing occurs.
 * 
 * @param argPtr    -- Pointer to argument.
 * @param currToken -- Current token of command line arguments.
 * 
 * @returns rawstr -- Next token to parse of command line arguments.
 */
auto ym::ArgParser::parseArg(ArgPtr_T argPtr,
                             rawstr   currToken) const -> rawstr
{
   if (argPtr->isFlag())
   { // enable argument - no explicit value
      argPtr->enbl(true);
   }
   else
   { // value is next command line argument

      idx += 1_i32;
      ParseError::check(idx < _Argc, "No value for arg '%s'", argPtr->getName().get());

      // TODO this will incorrectly trigger for negative numbers
      ParseError::check(_Argv_Ptr[idx][0_u32] != '-', "Arg '%s' cannot have command line value ('%s')",
         argPtr->getName().get(), _Argv_Ptr[idx].get());

      argPtr->defval(_Argv_Ptr[idx]);

      if (argPtr->isList())
      { // argument is list

         argPtr->_nvals = 1_i32; // one element so far

         for (idx + 1_i32; (idx < _Argc) && (_Argv_Ptr[idx][0] != '-'); ++idx)
         { // go through all args that are not commands
            argPtr->_nvals++;
         }
      }
   }

   return idx;
}

/** get
 * 
 * @brief Returns the registered argument info associated with the given key.
 * 
 * @note std::binary_search doesn't return a pointer to the found object, which is
 *       why we use a custom binary search function.
 * 
 * @throws AccessError -- If no argument with the given name found.
 * 
 * @param Key -- Name of argument.
 * 
 * @returns ArgCIt_T -- Found argument.
 */
auto ym::ArgParser::get(str const Key) const -> ArgCPtr_T
{
   auto const BeginPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cbegin()
#else
      _argHandlers
#endif
   ;

   auto const EndPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cend()
#else
      _argHandlers + _NHandlers;
#endif
   ;

   auto const ArgCPtr = ymBinarySearch(BeginPtr, EndPtr, Key,
      [](str const Key, ArgCPtr_T const & ArgCPtr) -> int32 {
         return std::strcmp(Key, ArgCPtr->getName());
      }
   );

   AccessError::check(ArgCPtr != EndPtr, "Key '%s' not found", Key.get());

   return ArgCPtr;
}

/** getNextToken
 * 
 * TODO
 */
auto ym::ArgParser::getNextToken(rawstr currToken) -> rawstr
{
   rawstr nextCmdLineArg = nullptr;

   if (_Argc < 0_i32)
   { // cmd line args are in one string
      
      if (!currCmdLineArg)
      { // no selected cmd line arg - set to argument string
         currCmdLineArg = _Argv._Argv;
      }

      // TODO this logic is wrong

      while (*currCmdLineArg && !std::isspace(static_cast<unsigned char>(*currCmdLineArg)))
      { // advance stream to next token separator
         currCmdLineArg++;
      }

      while (*currCmdLineArg && std::isspace(static_cast<unsigned char>(*currCmdLineArg)))
      { // advance stream to next token
         currCmdLineArg++;
      }

      if (currCmdLineArg)
      { // captured a cmd line arg before

         while (*currCmdLineArg && !std::isspace(static_cast<unsigned char>(*currCmdLineArg)))
         { // advance stream to next token separator
            currCmdLineArg++;
         }

         while (*currCmdLineArg && std::isspace(static_cast<unsigned char>(*currCmdLineArg)))
         { // advance stream to next token
            currCmdLineArg++;
         }

         if (*currCmdLineArg)
         { // found next token
            nextCmdLineArg = currCmdLineArg;
         }
      }
   }
   else
   { // cmd line args as passed into main()

      if (currCmdLineArg)
      { // captured a cmd line arg before

         auto const CurrIdx = static_cast<int32>(std::distance(_Argv._Argv_Ptr[0].get(), currCmdLineArg));
         auto const NextIdx = CurrIdx + 1_i32;

         if (NextIdx < _Argc)
         { // there is a next element
            nextCmdLineArg = _Argv._Argv_Ptr[NextIdx].get();
         }
      }
      else if (_Argc > 0_i32)
      { // there is a next element
         nextCmdLineArg = _Argv._Argv_Ptr[0].get();
      }
   }

   return nextCmdLineArg;
}

/** organizeAndValidateArgHandlerVector
 * 
 * @brief Sorts and checks for duplicate args.
 * 
 * @throws Error    -- If std::sort fails.
 * @throws ArgError -- If there is a duplicate argument (argument with the same name).
 * @throws ArgError -- If name is null or empty or invalid.
 * @throws ArgError -- If description is null or empty.
 * @throws ArgError -- If the abbreviation is invalid or
 *                     has been previously set.
 * @throws ArgError -- If flag has been set and value is invalid.
 * @throws ArgError -- If Enabled but not marked as a flag.
 */
void ym::ArgParser::organizeAndValidateArgHandlerVector(void)
{
   auto const BeginPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cbegin()
#else
      _argHandlers
#endif
   ;

   auto const EndPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cend()
#else
      _argHandlers + _NHandlers;
#endif
   ;

   // --- --- --- --- organize --- --- --- ---

   try
   { // std::sort may fail
      // searching algorithms require keys be sorted
      std::sort(BeginPtr, EndPtr,
         [](Arg const & Lhs, Arg const & Rhs) -> bool {
            return std::strcmp(Lhs.getName(), Rhs.getName()) < 0;
         }
      );
   }
   catch(std::exception const & E)
   { // throw custom error
      ParseError::check(false, "std::sort failed with msg '%s'", E.what());
   }

   // --- --- --- --- validate --- --- --- ---

   for (auto it = BeginPtr; it != EndPtr; it++)
   { // go through all args

      auto const Key  = it->getName();
      auto const Desc = it->getDesc();
      auto const Val  = it->getVal ();
      auto const Abbr = it->getAbbr();

      // --- --- detect duplicate keys --- ---

      if (it != BeginPtr)
      { // compare current key to previous key
         ParseError::check(std::strcmp(Key, (it - 1_u32)->getName()) != 0, "Duplicate key '%s'", Key.get());
      }

      // --- --- validate key name --- ---

      ParseError::check(*Key, "Name must be non-empty");
      ParseError::check(std::strcmp(Key, "help") != 0, "Arg cannot be named the reserved word 'help'");

      // dereference here is safe, we just checked that above
      for (auto currChar = Key.get(); *currChar != '\0'; ++currChar)
      { // go through all chars in proposed name
         ParseError::check(isValidChar(*currChar), "Name '%s' cannot contain '%c'", Key.get(), *currChar);
      }

      // --- --- validate description --- ---

      ParseError::check(*Desc, "Description must be non-empty");

      // --- --- validate value --- ---

      // nothing to check here - value/flag consistency tested below

      // --- --- validate abbreviation --- ---

      if (Abbr != '\0')
      { // assigned abbr
         ParseError::check(isValidChar(Abbr), "Abbr 0x%x not valid for arg '%s'", Abbr, Key.get());
         ParseError::check(_abbrs[getAbbrIdx(Abbr)] == EndPtr, "Abbr '%c' already occupied", Abbr);

         _abbrs[getAbbrIdx(Abbr)] = it;
      }

      // --- --- validate flag --- ---

      if (it->isFlag())
      { // marked as flag
         ParseError::check(std::strcmp(Val, "0") == 0 || std::strcmp(Val, "1") == 0,
            "Arg '%s' is a flag - cannot have arbitrary value", Key.get());
      }
      else
      { // not a flag
         ParseError::check(!it->isEnbl(), "Arg '%s' cannot be enabled and not marked as a flag", Key.get());
      }

      // --- --- validate enable --- ---

      if (it->isEnbl())
      { // enabled - better be a flag
         ParseError::check( it->isFlag(), "Arg '%s' is marked as enabled but is not a flag", Key.get());
         ParseError::check(!it->isList(), "Arg '%s' is marked enabled but also a list", Key.get());
      }

      // --- --- validate list --- ---

      if (it->isList())
      { // list
         ParseError::check(!it->isFlag(), "Arg '%s' is marked as a list and a flag", Key.get());

         // list/enbl exclusion already tested
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

   auto const BeginPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cbegin()
#else
      _argHandlers
#endif
   ;

   auto const EndPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cend()
#else
      _argHandlers + _NHandlers;
#endif
   ;

   auto maxKeyLen = 0_u32;

   for (auto it = BeginPtr; it != EndPtr; it++)
   { // go through all registered arguments
      if (auto const KeyLen = std::strlen(it->getName()); KeyLen > maxKeyLen)
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

   for (auto it = BeginPtr; it != EndPtr; it++)
   { // go through all registered arguments
      auto const KeyLen = std::strlen(it->getName());
      ymLog(VG::ArgParser, " --%s%s : %s", it->getName().get(), spaces_bptr + KeyLen, it->getDesc().get());

      if (auto const Abbr = it->getAbbr(); isValidChar(Abbr))
      { // this arg has an abbreviation
         ymLog(VG::ArgParser, "   (-%c)", Abbr);
      }
   }
}

/** getArgPtrFromPrefix
 * 
 * @brief Returns the registered argument info associated with the given prefix.

 * 
 * @throws ParseError -- If prefix is ambiguous.
 * @throws ParseError -- If prefix is not found.
 * 
 * @param Prefix -- Prefix of argument.
 * 
 * @returns ArgPtr_T -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromPrefix(str const Prefix) -> ArgPtr_T
{
   auto const BeginPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cbegin()
#else
      _argHandlers
#endif
   ;

   auto const EndPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cend()
#else
      _argHandlers + _NHandlers;
#endif
   ;

   std::string_view const PrefixSV(Prefix);

   // returns first iterator where comp() evals to false
   auto const LowerBoundArgIt = std::lower_bound(BeginPtr, EndPtr, PrefixSV,
      // should return true if Arg is ordered before Prefix
      [](Arg const & Arg, std::string_view const & PrefixSV) -> bool {
         return std::strncmp(Arg.getName(), PrefixSV.data(), PrefixSV.size()) < 0;
      }
   );

   ArgPtr_T searchedForArgIt = nullptr;

   for (auto argIt = LowerBoundArgIt; argIt != EndPtr; argIt++)
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
            ParseError::check(argIt == EndPtr, "Prefix '%s' is ambiguous", PrefixSV.data());
            searchedForArgIt = argIt;
         }
      }
      else if (Cmp < 0)
      { // not a match - end search
         break;
      }
   }

   ParseError::check(searchedForArgIt, "Prefix '%s' doesn't match any handlers", PrefixSV.data());

   return searchedForArgIt;
}

/** getArgPtrFromAbbr
 * 
 * @brief Returns the registered argument info associated with the given abbreviation.
 * 
 * @param Abbr -- Abbreviation of argument.
 * 
 * @returns ArgPtr_T -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromAbbr(char const Abbr) -> ArgPtr_T
{
   ParseError::check(isValidChar(Abbr), "Abbr 0x%x not valid", Abbr);

   auto const SearchedForArgIt = _abbrs[getAbbrIdx(Abbr)];

   ParseError::check(SearchedForArgIt, "Abbr '%c' not registered", Abbr);

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
   : _name  {Name },
     _desc  {""   },
     _val   {""   },
     _nvals {0_u32},
     _abbr  {'\0' },
     _flag  {false},
     _enbl  {false},
     _list  {false}
{
}

/** getVal
 * 
 * @brief Gets the value associated with the given arg (and index if applicable).
 * 
 * @param Idx -- Index of requested list element, if applicable.
 * 
 * @return str -- Requested value.
 */
auto ym::ArgParser::Arg::getVal(uint32 const Idx) const -> str
{
   auto retVal = _val;

   if (isList())
   { // access the requested element
      Error::check(Idx < _nvals, "Invalid index %u for arg %s in [0..%u)", Idx, getName().get(), _nvals);
      retVal = _val + Idx; // _val is a pointer to an element in _Argv_Ptr
   }

   return retVal;
}
