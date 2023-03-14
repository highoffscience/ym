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
void ym::ArgParser::parse(std::vector<Arg> &&                args_uref,
                          [[maybe_unused]] int const         Argc,
                          [[maybe_unused]] str const * const Argv_Ptr)
{
   _args = std::move(args_uref);

   // we use binary search to find keys so sort first
   std::sort(_args.begin(), _args.end(),
      [](Arg const & Lhs, Arg const & Rhs) -> bool {
         return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
      }
   );

   Arg * arg_ptr = nullptr;

   for (auto i = 1_i32; i < Argc; ++i)
   {
      std::string_view name = Argv_Ptr[i];

      if (name.starts_with("--"))
      { // found longhand arg
         name.remove_prefix(2_u32);

         auto const Pos = name.find('=');
         name = name.substr(Pos); // allowed to be npos

         // TODO set arg_ptr
      }
      else if (name.starts_with('-'))
      { // potential shorthand args or negative number

      }

      if (name[0_u32] == '-')
      { // potential arg
         if (name[1_u32] == '-')
         { // potential longhand arg
            if (std::isalpha(name[2_u32]))
            { // found longhand arg
               name.remove_prefix(2_u32);



               if (auto * const arg_Ptr = getArgPtr(name); arg_Ptr)
               { // found match
                  i += 1_i32; // move to get value
                  ArgParserError_LongHandNoVal
                  if (i < Argc)
                  {
                     arg_Ptr->setVal(name);
                  }
                  

                  if (++i < Argc)
                  { // found value
                     arg_Ptr->_val = Argv_Ptr[i];
                  }
                  else
                  { // no value - EOF
                     std::printf("No value for arg %s!\n", arg);
                     everythingGood = false;
                  }
               }
               else
               { // found unregistered arg
                  std::printf("Found unregistered arg %s!\n", arg);
                  everythingGood = false;
               }
               continue;
            }
         }
         else if (isAlpha(arg[1]))
         { // found shorthand arg(s)
            arg += 1;
            // TODO
            continue;
         }
      }

      std::printf("Found rogue value %s!\n", arg);
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
                                  static_cast<str>(Rhs_ptr));
            }
         )
      );
}

/** getArgPtr_soft
 * 
 * @brief TODO
 */
auto ym::ArgParser::getArgPtr_soft(str const Key) -> Arg *
{
   for (auto const Name : _args)
   {
      
   }
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
     _abbr   {'\0'   }
{
   ArgParserError_NameEmpty::assert(ymIsStrNonEmpty(getName()), "Name must be non-empty");

   ArgParserError_NameInvalid::assert(std::isalpha(getName()[0_u32]), "Name '%s' is invalid", getName());
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
   ArgParserError_ValInUse::assert(!getVal(), "Value must not have already been set");

   _val = DefaultVal;

   ArgParserError_ValEmpty::assert(ymIsStrNonEmpty(getVal()), "Val must be non-empty");

   return *this;
}

/**
 *
 */
auto ym::ArgParser::Arg::abbr(char const Abbr) -> Arg &
{
   auto const Idx = (Abbr >= 'A' && Abbr <= 'Z') ? (Abbr - 'A'         ) :
                    (Abbr >= 'a' && Abbr <= 'z') ? (Abbr - 'a' + 26_i32) :
                    _ap_Ptr->_args.size();

   ArgParserError_AbbrInvalid::assert(Idx < _ap_Ptr->_args.size(),
      "Illegal abbr '%c' found for arg '%s'", Abbr, getName());

   ArgParserError_AbbrInUse::assert(!_ap_Ptr->_abbrs[Idx],
      "Arg '%s' wants abbr '%c' but it's already used by arg '%s'",
      getName(), Abbr, _ap_Ptr->_abbrs[Idx]->getName());

   // TODO only can have abbr for flag arguments

   _abbr = Abbr;
   _ap_Ptr->_abbrs[Idx] = this;

   return *this;
}

/**
 * TODO count values
 */
void ym::ArgParser::Arg::setVal(str const Val)
{
   _val = Val;
}

// /**
//  *
//  */
// auto ArgParser::Arg::binary(void) -> Arg *
// {
//    _abbr; // TODO

//    return this;
// }
