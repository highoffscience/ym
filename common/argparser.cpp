/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "argparser.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

/**
 * TODO
 */
ym::ArgParser::ArgParser(void)
   : _args {/*default*/}
{
   // std::sort(_args.begin(), _args.end(),
   //    [](Arg const & Lhs, Arg const & Rhs) -> bool {
   //       return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
   //    }
   // );
}

/**
 * TODO
 */
void ym::ArgParser::init(std::vector<Arg> && args_uref)
{
   _args = std::move(args_uref);

   // std::sort(_args.begin(), _args.end(),
   //    [](Arg const & Lhs, Arg const & Rhs) -> bool {
   //       return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
   //    }
   // );
}

/**
 * TODO
 */
auto ym::ArgParser::arg(str const Name) -> Arg
{
   return Arg(Name, this);
}

/**
 * TODO this should return an exception with all the errors
 */
void ym::ArgParser::parse([[maybe_unused]] int const         Argc,
                          [[maybe_unused]] str const * const Argv_Ptr)
{
   // auto isAlpha = [](char const C) -> bool {
   //    return (C >= 'A' && C <= 'Z') ||
   //           (C >= 'a' && C <= 'z');
   // }

   // for (int i = 1; i < Argc; ++i)
   // {
   //    auto name = Argv_Ptr[i];

   //    if (name[0] == '-') // accessing 0th element is safe because it's null terminated
   //    { // potential arg
   //       if (name[1] == '-')
   //       { // potential longhand arg
   //          if (isAlpha(name[2]))
   //          { // found longhand arg
   //             name += 2;
   //             if (auto * const arg_Ptr = get(name); arg_Ptr)
   //             { // found match
   //                // TODO
   //                if (arg_Ptr->isBool())
   //                {
   //                   arg_Ptr->setVal(...)
   //                }
   //                if (++i < Argc)
   //                { // found value
   //                   arg_Ptr->_val = Argv_Ptr[i];
   //                }
   //                else
   //                { // no value - EOF
   //                   std::printf("No value for arg %s!\n", arg);
   //                   everythingGood = false;
   //                }
   //             }
   //             else
   //             { // found unregistered arg
   //                std::printf("Found unregistered arg %s!\n", arg);
   //                everythingGood = false;
   //             }
   //             continue;
   //          }
   //       }
   //       else if (isAlpha(arg[1]))
   //       { // found shorthand arg(s)
   //          arg += 1;
   //          // TODO
   //          continue;
   //       }
   //    }

   //    std::printf("Found rogue value %s!\n", arg);
   //    everythingGood = false;
   // }

   // return everythingGood;
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

/**
 * TODO
 */
ym::ArgParser::Arg::Arg(str         const Name,
                        ArgParser * const ap_Ptr)
   : _ap_Ptr {ap_Ptr },
     _Name   {Name   },
     _desc   {nullptr},
     _val    {nullptr}
{
   ArgParserError_NameEmpty::assert(ymIsStrNonEmpty(getName()), "Name must be non-empty");
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

   ArgParserError_InvalidAbbr::assert(Idx < _ap_Ptr->_args.size(),
      "Illegal abbr '%c' found for arg '%s'", Abbr, getName());

   // ArgParserError_AbbrInUse::assert(!_abbrs[Idx],
   //    "Arg '%s' wants abbr '%c' but it's already used by arg '%s'",
   //    getName(), Abbr, _abbrs[Idx]->getName());

   // // TODO need to preserve MSB
   // _abbr = Abbr & 0x7F;
   // _abbrs[Idx] = this;

   return *this;
}

// /**
//  *
//  */
// auto ArgParser::Arg::binary(void) -> Arg *
// {
//    _abbr; // TODO

//    return this;
// }
