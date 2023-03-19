/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "argparser.h"

#include "textlogger.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string_view>
#include <utility>

/**
 * TODO
 */
ym::ArgParser::ArgParser(void)
   : _args  {/*default*/},
     _abbrs {nullptr    }
{
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
            auto * const agr_Ptr = getArgPtrFromPrefix(name);
            ArgParserError_NoArgFound::assert(agr_Ptr, "Arg '%s' not registered", name);
            ArgParserError_NoValFound::assert(++i < Argc, "No value for arg '%s'", name);
            agr_Ptr->setVal(Argv_Ptr[i]);
         }
         else
         { // shorthand arg found
            name += 1_u32;
            ArgParserError_NoAbbrFound::assert(name[0_u32] != '\0',
               "Expected abbr at count %d but none found", i);
            for (auto i = 0_u32; name[i] != '\0'; ++i)
            {
               auto   const Abbr    = name[i];
               auto * const arg_Ptr = getArgPtrFromAbbr(Abbr);
               ArgParserError_NoAbbrFound::assert(arg_Ptr, "Abbr '%c' not registered", Abbr);
               ArgParserError_AbbrNoFlag::assert(arg_Ptr->getFlag(), "Abbr '%c' not a flag");
               arg_Ptr->enable(true);
            }
         }
      }
   }
}

/**
 * TODO
 * 
 * @note std::binary_search doesn't return a pointer to the found object, which is
 *       why we use std::bsearch.
 */
auto ym::ArgParser::getArgPtr(str const Key) -> Arg *
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
         ArgParserError_AmbigPrefix::assert(!arg_ptr, "Prefix '%s' is ambiguous", Key);
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
   auto * const * const agr_Ptr_Ptr = getArgPtrPtrFromAbbr(Abbr);
   return (agr_Ptr_Ptr) ? *agr_Ptr_Ptr : nullptr;
}

/** getArgPtrPtrFromAbbr
 * 
 * @brief TODO
 */
auto ym::ArgParser::getArgPtrPtrFromAbbr(char const Abbr) -> Arg * *
{
   return (Abbr >= 'A' && Abbr <= 'Z') ? &_abbrs[Abbr - 'A'         ] :
          (Abbr >= 'a' && Abbr <= 'z') ? &_abbrs[Abbr - 'a' + 26_i32] :
          (Abbr >= '0' && Abbr <= '9') ? &_abbrs[Abbr - '0' + 52_i32] :
          nullptr;
}

/**
 * TODO
 */
ym::ArgParser::Arg::Arg(str         const Name,
                        ArgParser * const ap_Ptr)
   : _ap_Ptr {ap_Ptr },
     _name   {Name   },
     _desc   {nullptr},
     _val    {nullptr},
     _abbr   {'\0'   },
     _flag   {false  },
     _enbl   {false  }
{
   ArgParserError_NameEmpty::assert(ymIsStrNonEmpty(getName()), "Name must be non-empty");
   ArgParserError_NameInvalid::assert(std::isalnum(getName()[0_u32]), "Name '%s' is invalid", getName());
}

/**
 * TODO
 */
auto ym::ArgParser::Arg::desc(str const Desc) -> Arg &
{
   ArgParserError_DescInUse::assert(!getDesc(), "Description must not have already been set");

   _desc = Desc;

   ArgParserError_DescEmpty::assert(ymIsStrNonEmpty(getDesc()), "Description must be non-empty");

   return *this;
}

/**
 * TODO count values
 */
auto ym::ArgParser::Arg::defaultVal(str const DefaultVal) -> Arg &
{
   ArgParserError_ValWithFlag::assert(!getFlag(),
      "Arg '%s' is flag but requests default val '%s'", getName(), DefaultVal);

   ArgParserError_ValInUse::assert(!getVal(),
      "Arg '%s' already set with val '%s' (requested '%s')", getName(), getVal(), DefaultVal);

   _val = DefaultVal;

   ArgParserError_ValEmpty::assert(ymIsStrNonEmpty(getVal()),
      "Val must be non-empty for arg '%s'", getName());

   return *this;
}

/**
 *
 */
auto ym::ArgParser::Arg::abbr(char const Abbr) -> Arg &
{
   auto * * const arg_ptr_Ptr = _ap_Ptr->getArgPtrPtrFromAbbr(Abbr);

   ArgParserError_AbbrInvalid::assert(arg_ptr_Ptr,
      "Illegal abbr '%c' found for arg '%s'", Abbr, getName());

   ArgParserError_AbbrInUse::assert((*arg_ptr_Ptr)->getAbbr() == '\0',
      "Arg '%s' wants abbr '%c' but it's already used by arg '%s'",
      getName(), Abbr, (*arg_ptr_Ptr)->getName());

   _abbr = Abbr;
   *arg_ptr_Ptr = this;
   enable(true);

   return *this;
}

/**
 * TODO count values
 */
auto ym::ArgParser::Arg::flag(void) -> Arg &
{
   ArgParserError_ValWithFlag::assert(!getVal(),
      "Arg '%s' has val '%s' but requested to be a flag", getName(), getVal());

   _flag = true;

   return *this;
}

/**
 * TODO count values
 */
auto ym::ArgParser::Arg::enable(bool const Enable) -> Arg &
{
   flag();
   _enbl = Enable;

   return *this;
}

/**
 * TODO count values
 */
void ym::ArgParser::Arg::setVal(str const Val)
{
   _val = Val;
}
