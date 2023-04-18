/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "argparser.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <utility>

/** ArgParser
 * 
 * @brief Constructor.
 */
ym::ArgParser::ArgParser(void)
   : _args  {/*default*/},
     _abbrs {{nullptr  }}
{
}

/** getInstancePtr
 * 
 * @brief Returns the single instance pointer.
 * 
 * @return ArgParser * -- Single instance pointer.
 */
auto ym::ArgParser::getInstancePtr(void) -> ArgParser *
{
   static ArgParser instance;

   return &instance;
}

/** parse
 *
 * @brief Parses through the command line arguments and populates registered args.
 * 
 * @throws ArgParserError_ParseError -- If a parsing error occurs.
 * 
 * @param args_uref -- List of registerd arguments.
 * @param Argc      -- Argument count  (as supplied from main()).
 * @param Argv_Ptr  -- Argument vector (as supplied from main()).
 */
void ym::ArgParser::parse(std::vector<Arg> && args_uref,
                          int const           Argc,
                          str const * const   Argv_Ptr)
{
   _args = std::move(args_uref);

   organizeAndValidateArgVector();

   for (auto i = 1_i32; i < Argc; ++i)
   { // go through all command line arguments
      auto name = Argv_Ptr[i];

      if (name[0_u32] == '-')
      { // arg found
         if (name[1_u32] == '-')
         { // longhand arg found
            name += 2_u32;

            auto * const arg_Ptr = getArgPtrFromPrefix(name);
            ArgParserError_ParseError::check(arg_Ptr, "Arg '%s' not registered", name);

            i = parse_Helper(arg_Ptr, i, Argc, Argv_Ptr);
         }
         else
         { // shorthand arg found
            name += 1_u32;

            auto const NAbbrs = std::strlen(name);
            ArgParserError_ParseError::check(NAbbrs > 0_u32, "Expected abbr at count %d but none found", i);

            for (auto j = 0_u32; j < NAbbrs; ++j)
            { // go through all abbrs in pack

               auto   const Abbr    = name[j];
               auto * const arg_Ptr = getArgPtrFromAbbr(Abbr);
               ArgParserError_ParseError::check(arg_Ptr, "Abbr '%c' not registered", Abbr);

               if (NAbbrs == 1_u32)
               { // only 1 abbr - might have value
                  i = parse_Helper(arg_Ptr, i, Argc, Argv_Ptr);
                  break;
               }
               else
               { // abbr found
                  ArgParserError_ParseError::check(arg_Ptr->isFlag(), "Arg '%s' not a flag", arg_Ptr->getName());
                  arg_Ptr->enable(true);
               }
            }
         }
      }
      else
      { // unexpected command line argument
         ArgParserError_ParseError::check(false, "Argument '%s' was unexpected", Argv_Ptr[i]);
      }
   }
}

/** parse_Helper
 * 
 * @brief Sets the value depending on the type of argument.
 * 
 * @throws ArgParserError_ParseError -- If a parsing occurs.
 * 
 * @param arg_Ptr  -- Pointer to registered argument.
 * @param idx      -- Current index of command line argument.
 * @param Argc     -- Argument count  (as supplied from main()).
 * @param Argv_Ptr -- Argument vector (as supplied from main()).
 * 
 * @return int32 -- Updated index of command line argument.
 */
auto ym::ArgParser::parse_Helper(Arg       * const arg_Ptr,
                                 int32             idx,
                                 int32       const Argc,
                                 str const * const Argv_Ptr) const -> int32
{
   if (arg_Ptr->isFlag())
   { // enable argument - no explicit value
      arg_Ptr->enable(true);
   }
   else
   { // value is next command line argument
      idx += 1_i32;
      ArgParserError_ParseError::check(idx < Argc, "No value for arg '%s'", arg_Ptr->getName());
      arg_Ptr->setVal(Argv_Ptr[idx]);
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
 * @return Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::get(str const Key) -> Arg const *
{
   auto const * const Arg_Ptr = getArgPtrFromKey(Key);
   ArgParserError_AccessError::check(Arg_Ptr, "Key '%s' not found", Key);
   return Arg_Ptr;
}

/** isSet
 * 
 * @brief Returns if the key has been set.
 * 
 * @throws ArgParserError_AccessError -- If no argument with the given name found.
 * 
 * @param Key -- Name of argument.
 * 
 * @return True if the argument has a valid value, false otherwise.
 */
bool ym::ArgParser::isSet(str const Key)
{
   auto const * const Arg_Ptr = getArgPtrFromKey(Key);
   return Arg_Ptr->isFlag() ? Arg_Ptr->isEnbl() : ymIsStrNonEmpty(Arg_Ptr->getVal());
}

/** organizeAndValidateArgVector
 * 
 * @brief Sorts and checks for duplicate args.
 * 
 * @throws ArgParserError_ParseError -- If there is a duplicate argument
 *         (argument with the same name).
 */
void ym::ArgParser::organizeAndValidateArgVector(void)
{
   // searching algorithms require keys be sorted
   std::sort(_args.begin(), _args.end(),
      [](Arg const & Lhs, Arg const & Rhs) -> bool {
         return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
      }
   );

   // detect duplicates
   for (auto i = 1_u32; i < _args.size(); ++i)
   { // go through all args
      ArgParserError_ParseError::check(
         std::strcmp(_args[i-1_u32].getName(), _args[i].getName()) != 0_i32,
         "Duplicate arg '%s'", _args[i].getName());
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
 * @return Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::getArgPtrFromKey(str const Key) -> Arg *
{
   return
      static_cast<Arg *>(
         std::bsearch(Key, _args.data(), _args.size(), sizeof(Arg),
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
 * @return Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::getArgPtrFromPrefix(str const Prefix) -> Arg *
{
   Arg *      arg_ptr   = nullptr;
   auto const PrefixLen = std::strlen(Prefix);

   for (auto & arg_ref : _args)
   { // search through all stored args
      auto const Cmp = std::strncmp(Prefix, arg_ref.getName(), PrefixLen);

      if (Cmp == 0_i32)
      { // prefix found (whole prefix matched)
         if (std::strlen(arg_ref.getName()) == PrefixLen)
         { // entire key matched
            arg_ptr = &arg_ref;
            break;
         }
         else
         { // prefix matched but maybe others will match so continue search
           // if another match is found it will trigger the assert below
            ArgParserError_ParseError::check(!arg_ptr, "Prefix '%s' is ambiguous", Prefix);
            arg_ptr = &arg_ref;
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
 * @return Arg * -- Found argument, or null if no argument found.
 */
auto ym::ArgParser::getArgPtrFromAbbr(char const Abbr) -> Arg *
{
   auto const AbbrIdx = getAbbrIdx(Abbr);

   return (AbbrIdx < _abbrs.size()) ? getArgPtrFromKey(_abbrs[AbbrIdx]) : nullptr;
}

/** getArgPtrPtrFromAbbr
 * 
 * @brief Founds index of argument associated with the given abbreviation.
 * 
 * @param Abbr -- Abbreviation of argument.
 * 
 * @return uint32 -- Index of argument, or invalid if no argument found.
 */
auto ym::ArgParser::getAbbrIdx(char const Abbr) const -> uint32
{
   return static_cast<uint32>(
      (Abbr >= 'A' && Abbr <= 'Z') ? (Abbr - 'A'         ) :
      (Abbr >= 'a' && Abbr <= 'z') ? (Abbr - 'a' + 26_i32) :
      (Abbr >= '0' && Abbr <= '9') ? (Abbr - '0' + 52_i32) :
      _abbrs.size());
}

/** Arg
 * 
 * @brief Constructor.
 * 
 * @note Cannot check duplicate names here since arg vector has yet to be
 *       populated.
 * 
 * @note Names must start with an alphanumeric character.
 * 
 * @throws ArgParserError_ArgError -- If name is null or empty or invalid.
 * 
 * @param Name -- Name of argument.
 */
ym::ArgParser::Arg::Arg(str const Name)
   : _name {Name},
     _desc {""  },
     _val  {""  }
{
   ArgParserError_ArgError::check(ymIsStrNonEmpty(getName()), "Name must be non-empty");
   ArgParserError_ArgError::check(std::isalnum(getName()[0_u32]), "Name '%s' is invalid", getName());
}

/** desc
 * 
 * @brief Sets the description of the argument.
 * 
 * @throws ArgParserError_ArgError -- If description is null or empty or
 *         has been set previously.
 * 
 * @param Desc -- Description of argument.
 * 
 * @return Arg & -- *this.
 */
auto ym::ArgParser::Arg::desc(str const Desc) -> Arg &
{
   ArgParserError_ArgError::check(ymIsStrEmpty(getDesc()), "Description must not have already been set");
   ArgParserError_ArgError::check(ymIsStrNonEmpty(Desc), "Description must be non-empty");

   _desc = Desc;

   return *this;
}

/** val
 * 
 * @brief Sets the default value for the argument. If no value is suppplied
 *        on the command line this value is the one assumed.
 * 
 * TODO break into multiple @throws declarations for each check statement
 * @throws ArgParserError_ArgError -- If the argument is a flag or
 *         is null or empty or has been previously set.
 * 
 * @param DefaultVal -- Default value of argument.
 * 
 * @return Arg & -- *this.
 */
auto ym::ArgParser::Arg::val(str const DefaultVal) -> Arg &
{
   ArgParserError_ArgError::check(!isFlag(),
      "Arg '%s' is a flag - cannot have explicit value", getName());

   ArgParserError_ArgError::check(ymIsStrEmpty(getVal()),
      "Arg '%s' already set with val '%s' (requested '%s')", getName(), getVal(), DefaultVal);

   ArgParserError_ArgError::check(ymIsStrNonEmpty(DefaultVal),
      "Val must be non-empty for arg '%s'", getName());

   setVal(DefaultVal);

   return *this;
}

/** abbr
 * 
 * @brief Sets the optional abbreviation for the argument.
 * 
 * @note Arguments are first created on the stack and then placed in
 *       permanent memory so the vector storing the abbreviations
 *       cannot simply store *this here.
 * 
 * @throws ArgParserError_ArgError -- If the abbreviation is invalid or
 *         has been previously set.
 * 
 * @param Abbr -- Abbreviation of argument.
 * 
 * @return Arg & -- *this.
 */
auto ym::ArgParser::Arg::abbr(char const Abbr) -> Arg &
{
   auto * const ap_Ptr  = ArgParser::getInstancePtr();
   auto   const AbbrIdx = ap_Ptr->getAbbrIdx(Abbr);

   ArgParserError_ArgError::check(AbbrIdx < ap_Ptr->_abbrs.size(),
      "Illegal abbr '%c' found for arg '%s'", Abbr, getName());

   auto * & abbr_ptr_ref = ap_Ptr->_abbrs[AbbrIdx];

   ArgParserError_ArgError::check(!abbr_ptr_ref,
      "Arg '%s' wants abbr '%c' but it's already used by arg '%s'",
      getName(), Abbr, abbr_ptr_ref);

   abbr_ptr_ref = _name;

   return *this;
}

/** flag
 * 
 * @brief Optionally sets the argument as a binary switch, true or false.
 * 
 * @note Flags also default to false and must be explicitly turned on. If
 *       the opposite behaviour is desired prefix the argument name with
 *       "--no-" to make it clear.
 * 
 * @throws ArgParserError_ArgError -- If already marked as a flag or has
 *         a value set.
 */
auto ym::ArgParser::Arg::flag(void) -> Arg &
{
   ArgParserError_ArgError::check(!isFlag(),
      "Arg '%s' already marked as a flag", getName());

   // Flags will set the _val member but since we just checked if this
   //  argument was a flag only non flag values remain an option.
   ArgParserError_ArgError::check(ymIsStrEmpty(getVal()),
      "Arg '%s' has val '%s' but requested to be a flag", getName(), getVal());

   enable(false);

   return *this;
}

/**
 * 
 */
void ym::ArgParser::Arg::setVal(str const Val)
{
   ArgParserError_ArgError::check(Val, "Arg '%s' cannot have value of null", getName());

   _val = Val;
}

/**
 * TODO we can't check for isFlag here because that function relies on
 *      the val being set to true or false.
 */
void ym::ArgParser::Arg::enable(bool const Enbl)
{
   setVal(Enbl ? _s_TrueFlag : _s_FalseFlag);
}
