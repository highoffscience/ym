/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "argparser.h"

#include "memio.h"
#include "globallogger.h"

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
   int                    const Argc,
   BoundPtr<rawstr const> const Argv_BPtr,
   std::span<Arg>               argHandlers) :
      _argHandlers {argHandlers},
      _Argc        {Argc       },
      _Argv        {Argv_BPtr  }
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
   str      const  Argv,
   std::span<Arg>  argHandlers) :
      _argHandlers {argHandlers},
      _Argc        {     -1    }, // -1 indicates argument vector is single string
      _Argv        {    Argv   }
{ }

/** parse
 *
 * @brief Parses through the command line arguments and populates registered args.
 *
 * @throws Error -- If tokens are malformed.
 *
 * @returns ParseResult_T -- Result of the parse.
 */
auto ym::ArgParser::parse(void) -> ParseResult_T
{
   organizeAndValidateArgHandlerVector();

   auto result = ParseResult_T::Success; // until told otherwise

   for (str token = ""; !ym_empty(token = getNextToken().unwrap_or("")); /*nothing*/)
   { // go through all command line arguments

      if (token[0] == '-')
      { // arg found

         if (token[1] == '-')
         { // longhand arg found ("--")
            result = parseLonghand(token + 2);
         }
         else
         { // shorthand arg found ("-")
            result = parseShorthand(token + 1);
         }

         if (result == ParseResult_T::HelpMenuCalled)
         { // help menu was called - exit parse
            break;
         }
      }
      else
      { // unexpected command line argument
         result = ParseResult_T::Failure;
         ymLog(VF::UserError, "Argument '{}' was unexpected", token);
      }
   }

   return result;
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
   auto BeginIt = _argHandlers.begin();
   auto EndIt   = _argHandlers.end();

   // --- --- --- --- organize --- --- --- ---

   try
   { // std::sort may fail (typical standard libraries don't use dynamic
     // allocation, so this catch should never be executed)

      // searching algorithms require keys be sorted
      std::sort(BeginIt, EndIt,
         [](Arg const & Lhs, Arg const & Rhs) -> bool {
            return std::strcmp(Lhs.getName(), Rhs.getName()) < 0;
         }
      );
   }
   catch (std::exception const & E)
   { // throw custom error
      YMASSERT(false, Error, YM_DAH, "std::sort failed with msg '{}'", E.what());
   }

   // --- --- --- --- validate --- --- --- ---

   for (auto it = BeginIt; it != EndIt; it++)
   { // go through all args

      auto const Key  = it->getName();
      auto const Desc = it->getDesc();
      auto const Val  = it->getVal ().unwrap();
      auto const Abbr = it->getAbbr();

      // --- --- detect duplicate keys --- ---

      if (it != BeginIt)
      { // compare current key to previous key
         YMASSERT(std::strcmp(Key, (it - 1)->getName()) != 0, ArgError, YM_DAH,
            "Duplicate key '{}'", Key);
      }

      // --- --- validate key name --- ---

      YMASSERT(*Key, ArgError, YM_DAH, "Name must be non-empty");
      // TODO
      // YMASSERT(Key[0] != '-', ArgError, YM_DAH, "Name '{}' cannot begin with '-'", Key);
      YMASSERT(std::strcmp(Key, "help") != 0, ArgError, YM_DAH,
         "Arg cannot be named the reserved word 'help'");

      // --- --- validate description --- ---

      YMASSERT(*Desc, ArgError, YM_DAH, "Description must be non-empty");

      // --- --- validate value --- ---

      // nothing to check here - value/flag consistency tested below

      // --- --- validate abbreviation --- ---

      if (Abbr != '\0')
      { // assigned abbr
         YMASSERT(isValidChar(Abbr), ArgError, YM_DAH, "Abbr 0x{:x} not valid for arg '{}'", Abbr, Key);
         YMASSERT(!_abbrs[getAbbrIdx(Abbr)], ArgError, YM_DAH,
            "Key '{}' tried to take already occupied Abbr '{}'", Key, Abbr);
         _abbrs[getAbbrIdx(Abbr)] = &*it;
      }

      // --- --- validate flag --- ---

      if (it->isFlag())
      { // marked as flag
         YMASSERT(std::strcmp(Val, "0") == 0 || std::strcmp(Val, "1") == 0, ArgError, YM_DAH,
            "Arg '{}' is a flag - cannot have arbitrary value", Key);
      }
      else
      { // not a flag
         YMASSERT(!it->isEnbl(), ArgError, YM_DAH,
            "Arg '{}' cannot be enabled and not marked as a flag", Key);
      }

      // --- --- validate enable --- ---

      if (it->isEnbl())
      { // enabled - better be a flag
         YMASSERT( it->isFlag(), ArgError, YM_DAH, "Arg '{}' is marked as enabled but is not a flag", Key);
         YMASSERT(!it->isList(), ArgError, YM_DAH, "Arg '{}' is marked enabled but also a list", Key);
      }

      // --- --- validate list --- ---

      if (it->isList())
      { // list
         YMASSERT(!it->isFlag(), ArgError, YM_DAH, "Arg '{}' is marked as a list and a flag", Key);

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
   auto const SE = ymLogPushEnable(VF::ArgParser);

   auto const BeginIt = _argHandlers.cbegin();
   auto const EndIt   = _argHandlers.cend();

   auto maxKeyLen = 0uz;

   for (auto it = BeginIt; it != EndIt; it++)
   { // go through all registered arguments
      if (auto const KeyLen = std::strlen(it->getName()); KeyLen > maxKeyLen)
      { // update max key length
         maxKeyLen = KeyLen;
      }
   }

   auto const spaces_Bptr = BoundPtr(YM_STACK_ALLOC(char, maxKeyLen + 1uz));

   for (auto i = 0uz; i < maxKeyLen; ++i)
   { // init all elements to spaces
      spaces_Bptr[i] = ' ';
   }
   spaces_Bptr[maxKeyLen] = '\0';

   ymLog(VF::ArgParser, "ArgParser help menu:");

   for (auto it = BeginIt; it != EndIt; it++)
   { // go through all registered arguments
      auto const KeyLen = std::strlen(it->getName());
      ymLog(VF::ArgParser, " --{}{} : {}", it->getName(), spaces_Bptr + KeyLen, it->getDesc());

      if (auto const Abbr = it->getAbbr(); isValidChar(Abbr))
      { // this arg has an abbreviation
         ymLog(VF::ArgParser, "   (-{})", Abbr);
      }
   }
}

/** getNextToken
 *
 * @brief Grabs the next token in the given command arguments.
 *
 * @returns optstr -- Next token in the list, or nullptr if no next.
 */
auto ym::ArgParser::getNextToken(void) -> optstr
{
   optstr token{};

   if (_Argc < 0)
   { // cmd line args are in one string

      if (_tidx.str_idx)
      { // no selected cmd line arg yet - set to argument string
         _tidx.str_idx = _Argv.Str;

         while (
            std::isspace(
               static_cast<unsigned char>(
                  *_tidx.str_idx.unwrap_or("~"))) != 0)
         { // advance stream to next token
            _tidx.str_idx++;
         }
      }
      else
      { // advance to next token in stream
         while (
            std::isspace(
               static_cast<unsigned char>(
                  *_tidx.str_idx.unwrap_or(" "))) == 0)
         { // advance stream to next token separator
            _tidx.str_idx++;
         }

         while (
            std::isspace(
               static_cast<unsigned char>(
                  *_tidx.str_idx.unwrap_or("~"))) != 0)
         { // advance stream to next token
            _tidx.str_idx++;
         }
      }

      token = _tidx.str_idx;
   }
   else
   { // cmd line args as passed into main()

      if (_tidx.vec_idx == 0)
      { // ignore the first argument
         _tidx.vec_idx++;
      }

      if (_tidx.vec_idx < _Argc)
      { // there is a next element
         token = _Argv.Vec[_tidx.vec_idx];
         _tidx.vec_idx++;
      }
   }

   return token;
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
 * @returns BoundPtr<Arg> -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromPrefix(str const Prefix) -> BoundPtr<Arg>
{
   FreePtr<Arg> arg_fptr = nullptr;

   auto BeginIt = _argHandlers.begin();
   auto EndIt   = _argHandlers.end();

   std::string_view const PrefixSV(Prefix);

   // returns first iterator where comp() evals to false
   auto const LowerBoundIt = std::lower_bound(BeginIt, EndIt, PrefixSV,
      // should return true if Arg is ordered before Prefix
      [](Arg const & Arg, std::string_view const & PrefixSV) -> bool {
         return std::strncmp(Arg.getName(), PrefixSV.data(), PrefixSV.size()) < 0;
      }
   );

   for (auto it = LowerBoundIt; it != EndIt; it++)
   { // search through all stored args

      auto const Cmp = std::strncmp(PrefixSV.data(), it->getName(), PrefixSV.size());

      if (Cmp == 0)
      { // prefix found (whole prefix matched)
         if (std::strlen(it->getName()) == PrefixSV.size())
         { // entire key matched
            arg_fptr = &*it;
            break;
         }
         else
         { // prefix matched but maybe others will match so continue search
            // if another match is found it will trigger the assert below
            YMASSERT(it == EndIt, ParseError, YM_DAH, "Prefix '{}' is ambiguous", PrefixSV);
            arg_fptr = &*it;
         }
      }
      else if (Cmp < 0)
      { // not a match - end search
         break;
      }
   }

   YMASSERT(arg_fptr, ParseError, YM_DAH, "Prefix '{}' doesn't match any handlers", PrefixSV);

   return arg_fptr.unwrap();
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
 * @returns BoundPtr<Arg> -- Found argument.
 */
auto ym::ArgParser::getArgPtrFromAbbr(char const Abbr) -> BoundPtr<Arg>
{
   YMASSERT(isValidChar(Abbr), ParseError, YM_DAH, "Abbr 0x{:x} not valid", Abbr);

   auto arg_fptr = _abbrs[getAbbrIdx(Abbr)];

   YMASSERT(arg_fptr, ParseError, YM_DAH, "Abbr '{}' not registered", Abbr);

   return arg_fptr.unwrap();
}

/** parseLonghand
 *
 * @brief Parses next long command.
 *
 * @throws Whatever the other functions throw.
 *
 * @param token -- Current token in stream.
 *
 * @returns ParseResult_T -- Result of the parse.
 */
auto ym::ArgParser::parseLonghand(str token) -> ParseResult_T
{
   YMASSERT(!ym_empty(token), ParseError, YM_DAH, "token missing after '--'")

   auto result = ParseResult_T::Success; // until told otherwise

   if (std::strcmp(token, "help") == 0) // must be full name - no prefix allowed
   { // help menu requested
      displayHelpMenu();
      result = ParseResult_T::HelpMenuCalled;
   }
   else
   { // standard arg
      auto const IsNeg = (std::strncmp(token, "no-", 3uz) == 0);
      token += (IsNeg) ? 3 : 0;
      result = parseLonghand(getArgPtrFromPrefix(token), IsNeg);
   }

   return result;
}

/** parseShorthand
 *
 * @brief Parses next short command.
 *
 * @throws ParseError -- If a parsing error occurs.
 * @throws Whatever the other functions throw.
 *
 * @param token -- Current token in stream.
 *
 * @returns ParseResult_T -- Result of the parse.
 */
auto ym::ArgParser::parseShorthand(str token) -> ParseResult_T
{
   YMASSERT(!ym_empty(token), ParseError, YM_DAH, "token missing after '-'")

   auto result = ParseResult_T::Success; // until told otherwise

   auto const NAbbrs = std::strlen(token); // already tested this has non-zero size

   for (auto i = 0uz; i < NAbbrs; ++i)
   { // go through all abbrs in pack

      auto const Abbr = token[i];

      if (Abbr == 'h')
      { // help menu requested
         displayHelpMenu();
         result = ParseResult_T::HelpMenuCalled;
         break;
      }
      else
      { // standard arg
         auto const MightHaveValue = (NAbbrs == 1uz);
         result = parseShorthand(getArgPtrFromAbbr(Abbr), MightHaveValue);
      }
   }

   return result;
}

/** parseLonghand
 *
 * @brief Sets the value depending on the type of argument.
 *
 * @throws ParseError -- If a parsing error occurs.
 * @throws Whatever the other functions throw.
 *
 * @param arg_Ptr -- Arg handler refering to the current token.
 * @param IsNeg   -- Is the flag negated.
 *
 * @returns ParseResult_T -- Result of the parse.
 */
auto ym::ArgParser::parseLonghand(
   BoundPtr<Arg> const arg_BPtr,
   bool          const IsNeg) -> ParseResult_T
{
   auto result = ParseResult_T::Failure; // until told otherwise

   if (arg_BPtr->isFlag())
   { // enable argument - no explicit value
      arg_BPtr->enbl(!IsNeg);
   }
   else
   { // value is next command line argument

      auto token = getNextToken();
      YMASSERT(token, ParseError, YM_DAH, "No value for arg '{}'", arg_BPtr->getName());
      arg_BPtr->defval(token.unwrap());

      if (arg_BPtr->isList())
      { // argument is list

         auto const NExpectedVals = std::strtoul(token.unwrap(), nullptr, 10);

         YMASSERT(NExpectedVals != 0ul && NExpectedVals != ULONG_MAX, ParseError, YM_DAH,
            "List '{}' has invalid hint for number of arguments", arg_BPtr->getName());

         arg_BPtr->_nvals = static_cast<uint32>(NExpectedVals);

         for (auto i = 0u; (i < arg_BPtr->_nvals) && (token = getNextToken()); i++)
         { // fast forward the number of items in list
            if (i == 0u)
            { // record starting point of list
               arg_BPtr->defval(token.unwrap());
            }
         }

         YMASSERT(token, ParseError, YM_DAH, "List '{}' doesn't have {} elements as promised",
            arg_BPtr->getName(), arg_BPtr->_nvals)
      }
   }

   return result;
}

/** parse
 *
 * @brief Sets the value depending on the type of argument.
 *
 * @note This overload is for shorthand args.
 *
 * @throws ParseError -- If a parsing error occurs.
 * @throws Whatever the other functions throw.
 *
 * @param arg_Ptr -- Arg handler refering to the current token.
 * @param MightHaveValue -- True if abbr might have an associated value, false otherwise.
 *
 * @returns ParseResult_T -- Result of the parse.
 */
auto ym::ArgParser::parseShorthand(
   BoundPtr<Arg> const arg_BPtr,
   bool          const MightHaveValue) -> ParseResult_T
{
   auto result = ParseResult_T::Failure; // until told otherwise

   if (MightHaveValue)
   { // only 1 abbr - might have value
      result = parseLonghand(arg_BPtr);
   }
   else
   { // abbr found
      YMASSERT(arg_BPtr->isFlag(), ParseError, YM_DAH,
         "Arg '{}' not a flag", arg_BPtr->getName());
      arg_BPtr->enbl(true);
      result = ParseResult_T::Success;
   }

   return result;
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
 * @returns BoundPtr<Arg const> -- Found argument, or nullptr if none found.
 */
auto ym::ArgParser::get(str const Key) const -> BoundPtr<Arg const>
{
   auto const BeginIt = _argHandlers.cbegin();
   auto const EndIt   = _argHandlers.cend();

   auto const It = ym_binarySearch(BeginIt, EndIt, Key.get(),
      [](auto const Key, auto const & Arg) noexcept -> std::weak_ordering {
         return std::strcmp(Key, Arg.getName()) <=> 0;
      }
   );

   YMASSERT(&*It, AccessError, YM_DAH, "Key '{}' not found", Key);

   return &*It;
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
ym::ArgParser::Arg::Arg(strlit const Name) :
   _name {Name}
{ }

/** getVal
 *
 * @brief Gets the value associated with the given arg (and index if applicable).
 *
 * @param Idx -- Index of requested list element, if applicable.
 *
 * @return str -- Requested value.
 */
auto ym::ArgParser::Arg::getVal(std::size_t const Idx) const -> optstr
{
   auto retVal = _val;

   if (isList())
   { // access the requested element
      YMASSERT(Idx < _nvals, Error, YM_DAH,
         "Invalid index {} for arg {} in [0..{})", Idx, getName(), _nvals);
      retVal += Idx; // retVal is a pointer to an element in _Argv_Ptr
   }

   return retVal;
}
