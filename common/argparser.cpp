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
ym::ArgParser::ArgParser(std::vector<Arg> && args_uref)
   : _args {std::move(args_uref)}
{
   std::sort(_args.begin(), _args.end(),
      [](Arg const & Lhs, Arg const & Rhs) -> bool {
         return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
      }
   );
}

/**
 * TODO
 */
auto ArgParser::add(Str_T const Name) -> Arg *
{
   // bsearch for the iterator in _args
   //    if Name == iterator -> error - already added element
   //    else emplace(iterator)
   // Note! returned iterator should always point to element that is *after* Name

   //
   // if (Name > _args.back())
   //

   // because of the magic std::vector uses to manages it's elements we need to
   //  guarantee there will be space for elements - no on-the-fly allocation
   if (_args.size() == _args.capacity())
   {
      _args.reserve((_args.capacity() == 0) ? 4 : _args.capacity() * 2);
   }

   _args.emplace_back(Name);

   return &_args.back();
}

/**
 * TODO this should return an exception with all the errors
 */
void ArgParser::parse(int   const         Argc,
                      Str_T const * const Argv_Ptr)
{
   auto isAlpha = [](char const C) -> bool {
      return (C >= 'A' && C <= 'Z') ||
             (C >= 'a' && C <= 'z');
   }

   for (int i = 1; i < Argc; ++i)
   {
      auto name = Argv_Ptr[i];

      if (name[0] == '-') // accessing 0th element is safe because it's null terminated
      { // potential arg
         if (name[1] == '-')
         { // potential longhand arg
            if (isAlpha(name[2]))
            { // found longhand arg
               name += 2;
               if (auto * const arg_Ptr = get(name); arg_Ptr)
               { // found match
                  // TODO
                  if (arg_Ptr->isBool())
                  {
                     arg_Ptr->setVal(...)
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
      everythingGood = false;
   }

   return everythingGood;
}

/**
 *
 */
auto ArgParser::get(Str_T const Key) -> Arg *
{
   return static_cast<Arg *>(std::bsearch(Key, _args.data(), _args.size(), sizeof(Arg),
                                          [](void const * Lhs_ptr, void const * Rhs_ptr) -> int {
      return std::strcmp(static_cast<Str_T>(Lhs_ptr), static_cast<Str_T>(Rhs_ptr));
   }));
}

/**
 * TODO
 */
ym::ArgParser::Arg::Arg(str const Name)
   : _Name   {Name   },
     _desc   {nullptr},
     _val    {nullptr}
{
   // TODO it would be nice to have a family of argparse errors so we could broadly catch them all.
   // ymception class is already setup to handle polymorphism
   ymAssert<ArgParser_NameEmptyError>(!ymIsStrNonEmpty(getName()),
      "Name is empty");
}

/**
 * TODO
 */
auto ArgParser::Arg::desc(str const Desc) -> Arg &
{
   // assert getDesc() already populated

   _desc = Desc;

   // assert !ymIsStrNonEmpty(getDesc())

   return *this;
}

// /**
//  * TODO count values
//  */
// auto ArgParser::Arg::defaultVal(Str_T const DefaultVal) -> Arg *
// {
//    _val = DefaultVal;
//    return this;
// }

// /**
//  *
//  */
// auto ArgParser::Arg::abbr(char const Abbr) -> Arg *
// {
//    uint32 const Idx = (Abbr >= 'A' && Abbr <= 'Z') ? (Abbr - 'A'     ) :
//                       (Abbr >= 'a' && Abbr <= 'z') ? (Abbr - 'a' + 26) : _args.size();

//    if (Idx >= 52)
//    {
//       throw ParseError("Illegal abbr '%c' found for arg '%s'!", Abbr, getName());
//    }

//    if (_abbrs[Idx])
//    {
//       throw ParseError("Arg '%s' wants abbr '%c' but it's already used by arg '%s'!",
//                        getName(), Abbr, _abbrs[Idx]->getName());
//    }

//    // TODO need to preserve MSB
//    _abbr = Abbr & 0x7F;
//    _abbrs[Idx] = this;

//    return this;
// }

// /**
//  *
//  */
// auto ArgParser::Arg::binary(void) -> Arg *
// {
//    _abbr; // TODO

//    return this;
// }
