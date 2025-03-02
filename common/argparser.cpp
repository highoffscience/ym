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
   ArgHandlers_T        argHandlers
#if (!YM_ARGPARSER_USE_STD_SPAN)
   , uint32       const NHandlers
#endif
) :
   _abbrs       {/*default*/},
   _argHandlers {argHandlers},
   _Argc        {Argc       },
   _Argv        {Argv_Ptr   }
#if (!YM_ARGPARSER_USE_STD_SPAN)
   , _NHandlers {NHandlers  }
#endif
{ }

/** ArgParser
 * 
 * @brief Initializes class variables.
 * 
 * @note ArgParser assumes ownership of argHandlers.
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
) : _abbrs      {/*default*/},
   _argHandlers {argHandlers},
   _Argc        {-1_i32     },
   _Argv        {Argv       }
#if (!YM_ARGPARSER_USE_STD_SPAN)
   , _NHandlers {NHandlers  }
#endif
{ }

/** parse
 *
 * @brief Parses through the command line arguments and populates registered args.
 * 
 * @throws ParseError -- If a parsing error occurs.
 * @throws Whatever the other functions throw.
 * 
 * @returns bool -- True if the parse finishes normally, false otherwise (fails or aborts).
 */
auto ym::ArgParser::parse(void) -> ParseResult_T
{
   organizeAndValidateArgHandlerVector();

   auto result = ParseResult_T::Success; // until told otherwise

   for (rawstr token = nullptr; token = getNextToken(token); /*nothing*/)
   { // go through all command line arguments

      if (token[0_u32] == '-')
      { // arg found

         if (token[1_u32] == '-')
         { // longhand arg found ("--")
            token = parseLonghand(token, &result);
         }
         else
         { // shorthand arg found ("-")
            token = parseShorthand(token, &result);
         }

         if (result == ParseResult_T::HelpMenuCalled)
         { // help menu was called - exit parse
            break;
         }
      }
      else
      { // unexpected command line argument
         result = ParseResult_T::Failure;
         YMASSERT(false, ParseError, YM_DAH, "Argument '{}' was unexpected", token);
      }
   }

   return result;
}

/** parseLonghand
 * 
 * @brief Parses next long command.
 * 
 * @param token -- Current token in stream.
 * 
 * @returns rawstr -- Next token to parse.
 */
auto ym::ArgParser::parseLonghand(
   rawstr                token,
   ParseResult_T * const result_out_Ptr) -> rawstr
{
   token += 2_u32; // move past "--"

   if (*token && std::strcmp(token, "help") == 0) // must be full name - no prefix allowed
   { // help menu requested
      displayHelpMenu();
      *result_out_Ptr = ParseResult_T::HelpMenuCalled;
   }
   else
   { // standard arg
      
      auto isNegation = false; // until told otherwise

      if (auto newToken = getArgNegation(token); newToken != token)
      { // arg prefixed by --no- - negation on flag
         isNegation = true;
         token = newToken;
      }

      auto * const arg_Ptr = getArgPtrFromPrefix(token);
      token = parseArg(arg_Ptr, token, isNegation);
   }

   return token;
}

/** parseShorthand
 * 
 * @brief Parses next short command.
 * 
 * @param token -- Current token in stream.
 * 
 * @returns rawstr -- Next token to parse.
 */
auto ym::ArgParser::parseShorthand(rawstr token) -> rawstr
{
   token += 2_u32; // move past "-"

   auto const NAbbrs = std::strlen(token);
   YMASSERT(NAbbrs > std::size_t(0u), ParseError, YM_DAH, "Expected abbreviation after '-' symbol");

   for (auto i = 0_u32; i < NAbbrs; ++i)
   { // go through all abbrs in pack

      auto const Abbr = token[i];

      if (Abbr == 'h')
      { // help menu requested
         displayHelpMenu();
         token = nullptr;
         break;
      }
      else
      { // standard arg

         auto * const arg_Ptr = getArgPtrFromAbbr(Abbr);

         if (NAbbrs == 1_u32)
         { // only 1 abbr - might have value
            token = parseArg(arg_Ptr, token);
            break;
         }
         else
         { // abbr found
            ParseError::check(arg_Ptr->isFlag(), "Arg '%s' not a flag", arg_Ptr->getName().get());
            arg_Ptr->enbl(true);
         }
      }
   }

   return token;
}

/**
 * TODO
 */
auto ym::ArgParser::getArgNegation(rawstr currToken) -> rawstr
{
   constexpr std::string_view NegPrefix("-no");

   if (*currToken && std::strncmp(currToken, NegPrefix.data(), NegPrefix.size()) == 0)
   { // has negation
      currToken += NegPrefix.size();
   }

   return currToken;
}

/** parseArg
 * 
 * @brief Sets the value depending on the type of argument.
 * 
 * @throws ParseError -- If a parsing occurs.
 * 
 * @param arg_Ptr    -- Pointer to argument.
 * @param token      -- Current token of command line arguments.
 * @param IsNegation -- Flag requested to be off.
 * 
 * @returns rawstr -- Next token to parse of command line arguments.
 */
auto ym::ArgParser::parseArg(Arg * const arg_Ptr,
                             rawstr      token,
                             bool  const IsNegation) const -> rawstr
{
   if (arg_Ptr->isFlag())
   { // enable argument - no explicit value
      arg_Ptr->enbl(!IsNegation);
   }
   else
   { // value is next command line argument

      token = getNextToken(token);
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
 * TODO span's can have non-contiguous memory, whereas non-spans must be.
 *
 * @param Key -- Name of argument.
 * 
 * @returns ArgCIt_T -- Found argument.
 */
auto ym::ArgParser::get(str const Key) const -> Arg const *
{
   auto const BeginPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cbegin();
#else
      _argHandlers;
#endif

   auto const EndPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cend();
#else
      _argHandlers + _NHandlers;
#endif

   auto const ArgPtr = ymBinarySearch(BeginPtr, EndPtr, Key,
      [](str const Key, auto const & Arg) -> int32 {
         return std::strcmp(Key, Arg->getName());
      }
   );

   YMASSERT(ArgPtr != EndPtr, AccessError, YM_DAH, "Key '{}' not found", Key);

   return &*ArgPtr;
}

/** getNextToken
 * 
 * TODO
 */
auto ym::ArgParser::getNextToken(rawstr currToken) -> rawstr
{
   rawstr nextCmdLineArg = nullptr;

   if (_Argc < 0)
   { // cmd line args are in one string
      
      if (!currCmdLineArg)
      { // no selected cmd line arg - set to argument string
         currCmdLineArg = _Argv._Argv;
      }

      while (*currCmdLineArg && !std::isspace(static_cast<unsigned char>(*currCmdLineArg)))
      { // advance stream to next token separator
         currCmdLineArg++;
      }

      while (*currCmdLineArg && std::isspace(static_cast<unsigned char>(*currCmdLineArg)))
      { // advance stream to next token
         currCmdLineArg++;
      }

      if (currCmdLineArg)
      { // found next cmd line arg
         nextCmdLineArg = currCmdLineArg;
      }
   }
   else
   { // cmd line args as passed into main()

      if (currCmdLineArg)
      { // captured a cmd line arg before

         auto const CurrIdx = static_cast<int>(std::distance(_Argv._Argv_Ptr[0].get(), currCmdLineArg));
         auto const NextIdx = CurrIdx + 1;

         if (NextIdx < _Argc)
         { // there is a next element
            nextCmdLineArg = _Argv._Argv_Ptr[NextIdx].get();
         }
      }
      else if (_Argc > 0)
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
 * @throws ArgError -- If the abbreviation is invalid or has been previously set.
 * @throws ArgError -- If flag has been set and value is invalid.
 * @throws ArgError -- If Enabled but not marked as a flag.
 */
void ym::ArgParser::organizeAndValidateArgHandlerVector(void)
{
   auto const BeginPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cbegin();
#else
      _argHandlers;
#endif

   auto const EndPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cend();
#else
      _argHandlers + _NHandlers;
#endif

   // --- --- --- --- organize --- --- --- ---

   try
   { // std::sort may fail (typical standard libraries don't use dynamic
     // allocation, so this catch should never be executed)

      // searching algorithms require keys be sorted
      std::sort(BeginPtr, EndPtr,
         [](Arg const & Lhs, Arg const & Rhs) -> bool {
            return std::strcmp(Lhs.getName(), Rhs.getName()) < 0;
         }
      );
   }
   catch (std::exception const & E)
   { // throw custom error
      YMASSERT(false, ParseError, YM_DAH, "std::sort failed with msg '{}'", E.what());
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
         YMASSERT(std::strcmp(Key, (it - 1)->getName()) != 0, ParseError, YM_DAH,
            "Duplicate key '{}'", Key);
      }

      // --- --- validate key name --- ---

      YMASSERT(*Key, ParseError, YM_DAH, "Name must be non-empty");
      YMASSERT(std::strcmp(Key, "help") != 0, ParseError, YM_DAH,
         "Arg cannot be named the reserved word 'help'");

      // dereference here is safe, we just checked that above
      for (auto currChar = Key.get(); *currChar != '\0'; currChar++)
      { // go through all chars in proposed name
         YMASSERT(isValidChar(*currChar), ParseError, YM_DAH,
            "Name '{}' cannot contain '{}'", Key, *currChar);
      }

      // --- --- validate description --- ---

      YMASSERT(*Desc, ParseError, YM_DAH, "Description must be non-empty");

      // --- --- validate value --- ---

      // nothing to check here - value/flag consistency tested below

      // --- --- validate abbreviation --- ---

      if (Abbr != '\0')
      { // assigned abbr
         YMASSERT(isValidChar(Abbr), ParseError, YM_DAH, "Abbr 0x%x not valid for arg '{}'", Abbr, Key);
         YMASSERT(_abbrs[getAbbrIdx(Abbr)] == EndPtr, ParseError, YM_DAH, "Abbr '{}' already occupied", Abbr);

         _abbrs[getAbbrIdx(Abbr)] = &*it;
      }

      // --- --- validate flag --- ---

      if (it->isFlag())
      { // marked as flag
         YMASSERT(std::strcmp(Val, "0") == 0 || std::strcmp(Val, "1") == 0, ParseError, YM_DAH,
            "Arg '{}' is a flag - cannot have arbitrary value", Key);
      }
      else
      { // not a flag
         YMASSERT(!it->isEnbl(), ParseError, YM_DAH,
            "Arg '{}' cannot be enabled and not marked as a flag", Key);
      }

      // --- --- validate enable --- ---

      if (it->isEnbl())
      { // enabled - better be a flag
         YMASSERT( it->isFlag(), ParseError, YM_DAH, "Arg '{}' is marked as enabled but is not a flag", Key);
         YMASSERT(!it->isList(), ParseError, YM_DAH, "Arg '{}' is marked enabled but also a list", Key);
      }

      // --- --- validate list --- ---

      if (it->isList())
      { // list
         YMASSERT(!it->isFlag(), ParseError, YM_DAH, "Arg '{}' is marked as a list and a flag", Key);

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
      _argHandlers.cbegin();
#else
      _argHandlers;
#endif

   auto const EndPtr =
#if (YM_ARGPARSER_USE_STD_SPAN)
      _argHandlers.cend();
#else
      _argHandlers + _NHandlers;
#endif

   auto maxKeyLen = std::size_t(0);

   for (auto it = BeginPtr; it != EndPtr; it++)
   { // go through all registered arguments
      if (auto const KeyLen = std::strlen(it->getName()); KeyLen > maxKeyLen)
      { // update max key length
         maxKeyLen = KeyLen;
      }
   }

   auto spaces_bptr = bptr<char>(YM_STACK_ALLOC(char, maxKeyLen + std::size_t(1)));
   for (auto i = std::size_t(0); i < maxKeyLen; ++i)
   { // init all elements to spaces
      spaces_bptr[i] = ' ';
   }
   spaces_bptr[maxKeyLen] = '\0';

   ymLog(VG::ArgParser, "ArgParser help menu:");

   for (auto it = BeginPtr; it != EndPtr; it++)
   { // go through all registered arguments
      auto const KeyLen = std::strlen(it->getName());
      ymLog(VG::ArgParser, " --{}{} : {}", it->getName(), spaces_bptr + KeyLen, it->getDesc());

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
 * @returns Arg * -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromPrefix(rawstr const Prefix) -> Arg *
{
   Arg * arg_ptr = nullptr;

   if (Prefix)
   { // valid prefix

      auto const BeginPtr =
   #if (YM_ARGPARSER_USE_STD_SPAN)
         _argHandlers.cbegin();
   #else
         _argHandlers;
   #endif

      auto const EndPtr =
   #if (YM_ARGPARSER_USE_STD_SPAN)
         _argHandlers.cend();
   #else
         _argHandlers + _NHandlers;
   #endif

      std::string_view const PrefixSV(Prefix);

      // returns first iterator where comp() evals to false
      auto const LowerBoundArgIt = std::lower_bound(BeginPtr, EndPtr, PrefixSV,
         // should return true if Arg is ordered before Prefix
         [](Arg const & Arg, std::string_view const & PrefixSV) -> bool {
            return std::strncmp(Arg.getName(), PrefixSV.data(), PrefixSV.size()) < 0;
         }
      );

      for (auto argIt = LowerBoundArgIt; argIt != EndPtr; argIt++)
      { // search through all stored args

         auto const Cmp = std::strncmp(PrefixSV.data(), argIt->getName(), PrefixSV.size());

         if (Cmp == 0)
         { // prefix found (whole prefix matched)
            if (std::strlen(argIt->getName()) == PrefixSV.size())
            { // entire key matched
               arg_ptr = &*argIt;
               break;
            }
            else
            { // prefix matched but maybe others will match so continue search
              // if another match is found it will trigger the assert below
               YMASSERT(argIt == EndPtr, ParseError, YM_DAH, "Prefix '{}' is ambiguous", PrefixSV);
               arg_ptr = &*argIt;
            }
         }
         else if (Cmp < 0)
         { // not a match - end search
            break;
         }
      }

      YMASSERT(arg_ptr, ParseError, YM_DAH, "Prefix '{}' doesn't match any handlers", PrefixSV);
   }

   return arg_ptr;
}

/** getArgPtrFromAbbr
 * 
 * @brief Returns the registered argument info associated with the given abbreviation.
 * 
 * @throws ParseError -- If the requested Abbr is invalid.
 * @throws ParseError -- If the requested Abbr is not registered.
 *
 * @param Abbr -- Abbreviation of argument.
 * 
 * @returns Arg * -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromAbbr(char const Abbr) -> Arg *
{
   YMASSERT(isValidChar(Abbr), ParseError, YM_DAH, "Abbr 0x%x not valid", Abbr);

   auto * const arg_Ptr = _abbrs[getAbbrIdx(Abbr)];

   YMASSERT(arg_Ptr, ParseError, YM_DAH, "Abbr '{}' not registered", Abbr);

   return arg_Ptr;
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
   : _name {Name}
{ }

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
