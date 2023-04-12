/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "argparser.h"

#include "ops.h"
#include "textlogger.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <utility>

/**
 * TODO
 */
ym::ArgParser::ArgParser(void)
   : _args  {/*default*/},
     _abbrs {{nullptr  }}
{
}

auto ym::ArgParser::getInstancePtr(void) -> ArgParser *
{
   static ArgParser instance;

   return &instance;
}

/**
 * TODO this should return an exception with all the errors
 */
void ym::ArgParser::parse(std::vector<Arg> && args_uref,
                          int const           Argc,
                          str const * const   Argv_Ptr)
{
   _args = std::move(args_uref);

   // searching algorithms require keys be sorted
   std::sort(_args.begin(), _args.end(),
      [](Arg const & Lhs, Arg const & Rhs) -> bool {
         return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
      }
   );

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
         ArgParserError_ParseError::check(false, "Argument '%s' was unexpected", (Argv_Ptr[i]));
      }
   }
}

/**
 * TODO
 */
auto ym::ArgParser::parse_Helper(Arg       * const arg_Ptr,
                                 int32             idx,
                                 int32       const Argc,
                                 str const * const Argv_Ptr) -> int32
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

/**
 * TODO
 * 
 * @note std::binary_search doesn't return a pointer to the found object, which is
 *       why we use std::bsearch.
 */
auto ym::ArgParser::getArgPtr(str const Key) const -> Arg *
{
   return
      static_cast<Arg *>(
         std::bsearch(Key, _args.data(), _args.size(), sizeof(Arg),
            [](void const * Lhs_ptr, void const * Rhs_ptr) -> int {
               return std::strcmp(static_cast<str>(Lhs_ptr),
                                  static_cast<Arg const *>(Rhs_ptr)->getName());
            }
         )
      );
}

/**
 * TODO
 */
auto ym::ArgParser::operator[](str const Key) const -> Arg const *
{
   auto const * const Arg_Ptr = getArgPtr(Key);
   ArgParserError_AccessError::check(Arg_Ptr, "Key '%s' not found", Key);
   return Arg_Ptr;
}

/**
 * TODO
 */
bool ym::ArgParser::isSet(str const Key) const
{
   auto const * const Arg_Ptr = (*this)[Key];

   return Arg_Ptr->isFlag() ? Arg_Ptr->isEnbl() : ymIsStrNonEmpty(Arg_Ptr->getVal());
}

/** getArgPtrFromPrefix
 * 
 * @brief TODO
 */
auto ym::ArgParser::getArgPtrFromPrefix(str const Key) -> Arg *
{
   Arg *      arg_ptr = nullptr;
   auto const KeyLen  = std::strlen(Key);

   for (auto & arg_ref : _args)
   { // search through all stored args
      auto const Cmp = std::strncmp(Key, arg_ref.getName(), KeyLen);

      if (Cmp == 0_i32)
      { // prefix found (whole key matched)
         ArgParserError_ParseError::check(!arg_ptr, "Prefix '%s' is ambiguous", Key);
         arg_ptr = &arg_ref;
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
 * @brief TODO
 */
auto ym::ArgParser::getArgPtrFromAbbr(char const Abbr) -> Arg *
{
   auto const AbbrIdx = getAbbrIdx(Abbr);

   return (AbbrIdx < _abbrs.size()) ? getArgPtr(_abbrs[AbbrIdx]) : nullptr;
}

/** getArgPtrPtrFromAbbr
 * 
 * @brief TODO
 */
auto ym::ArgParser::getAbbrIdx(char const Abbr) const -> uint32
{
   return static_cast<uint32>(
      (Abbr >= 'A' && Abbr <= 'Z') ? (Abbr - 'A'         ) :
      (Abbr >= 'a' && Abbr <= 'z') ? (Abbr - 'a' + 26_i32) :
      (Abbr >= '0' && Abbr <= '9') ? (Abbr - '0' + 52_i32) :
      _abbrs.size());
}

/**
 * TODO
 */
ym::ArgParser::Arg::Arg(str const Name)
   : _name   {Name},
     _desc   {""  },
     _val    {""  }
{
   ArgParserError_ArgError::check(ymIsStrNonEmpty(getName()), "Name must be non-empty");
   ArgParserError_ArgError::check(std::isalnum(getName()[0_u32]), "Name '%s' is invalid", getName());
}

/**
 * TODO
 */
auto ym::ArgParser::Arg::desc(str const Desc) -> Arg &
{
   ArgParserError_ArgError::check(ymIsStrEmpty(getDesc()), "Description must not have already been set");
   ArgParserError_ArgError::check(ymIsStrNonEmpty(Desc), "Description must be non-empty");

   _desc = Desc;

   return *this;
}

/**
 * TODO count values
 */
auto ym::ArgParser::Arg::val(str const DefaultVal) -> Arg &
{
   ArgParserError_ArgError::check(ymIsStrEmpty(getVal()),
      "Arg '%s' already set with val '%s' (requested '%s')", getName(), getVal(), DefaultVal);

   ArgParserError_ArgError::check(ymIsStrNonEmpty(DefaultVal),
      "Val must be non-empty for arg '%s'", getName());

   setVal(DefaultVal);

   return *this;
}

/**
 *
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

/**
 * TODO state reason we don't have default flag values (kinda pointless)
 */
auto ym::ArgParser::Arg::flag(void) -> Arg &
{
   ArgParserError_ArgError::check(!isFlag(),
      "Arg '%s' already marked as a flag", getName());

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
