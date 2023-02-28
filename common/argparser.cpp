/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "argparser.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <tuple>
#include <utility>

/**
 * TODO
 */
ym::ArgParser::ArgParser(std::vector<Arg> && args_uref)
   : _args {std::move(args_uref)}
{
   // std::sort(_args.begin(), _args.end(),
   //    [](Arg const & Lhs, Arg const & Rhs) -> bool {
   //       return std::strcmp(Lhs.getName(), Rhs.getName()) < 0_i32;
   //    }
   // );
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

namespace ym
{
   struct Base
   {
      void store(str const Format)
      {
         (void)Format;
      }
   };

   struct TestError : public Ymception
   {
      inline TestError() : Ymception("") {}

      static void assertHandler_test(str name, str const Format)
      {
         (void)Format;
         TestError e;
         (void)e;
      }

      template <Loggable... Args_T>
      struct assert
      {
         explicit inline assert(bool const Condition,
                     str const Format,
                     [[maybe_unused]] Args_T const... Args,
                     std::source_location const SrcLoc = std::source_location::current())
         {
            if (Condition)
            {
               assertHandler_test("" /*getClassName()*/, Format);
               (void)SrcLoc;
            }
         }
      };

      template <typename... Args_T>
      assert(bool const Condition,
            str const Format,
            Args_T const... Args) -> assert<Args_T...>;
   };

   // template <Ymceptable DerivedYmception_T>
   // struct ymAssert
   // {
   //    template <Loggable... Args_T>
   //    explicit inline ymAssert(bool const Condition,
   //                             str const Format,
   //                             [[maybe_unused]] Args_T const... Args,
   //                             std::source_location const SrcLoc = std::source_location::current())
   //    {
   //       if (Condition)
   //       {
   //          (void)Format;
   //          (void)SrcLoc;
   //       }
   //    }
   // };

   // template <typename... Args_T>
   // struct Assert_ArgParserError_NameEmpty : public Base
   // {
   //    Assert_ArgParserError_NameEmpty(bool const Condition,
   //          str const Format,
   //          [[maybe_unused]] Args_T const... Args,
   //          std::source_location const SrcLoc = std::source_location::current()) 
   //    {
   //       if (Condition)
   //       {
   //          store(Format);
   //          (void)SrcLoc;
   //       }
   //    }
   // };

   // template <Ymceptable DerivedYmception_T>
   // template <typename... Args_T>
   // ymAssert<DerivedYmception_T>(bool const Condition,
   //       str const Format,
   //       Args_T const... Args) -> ymAssert<Args_T...>;

   // inline void ymAssert([[maybe_unused]] bool const Condition,
   //                      [[maybe_unused]] str  const Format) { if (Condition) }
   
   // template <Loggable Arg_T>
   // inline void ymAssert([[maybe_unused]] bool  const Condition,
   //                      [[maybe_unused]] str   const Format,
   //                      [[maybe_unused]] Arg_T const Arg) {}
}

/**
 * TODO
 */
ym::ArgParser::Arg::Arg(str const Name)
   : _Name {Name   },
     _desc {nullptr},
     _val  {nullptr}
{
   // ymAssert<ArgParserError_NameEmpty>(ymIsStrNonEmpty(getName()), "Name must be non-empty");

   TestError::assert(ymIsStrNonEmpty(getName()), "Name must be non-empty", 7, 9.0);
}

/**
 * TODO
 */
auto ym::ArgParser::Arg::desc(str const Desc) -> Arg &
{
   // ymAssert<ArgParserError_DescInUse>(!getDesc(), YM_SRCLOC(), "Description must not have already been set");

   //Ymception::assert(!getDesc(), "Description must not have already been set");

   _desc = Desc;

   // ymAssert<ArgParserError_DescEmpty>(ymIsStrNonEmpty(getDesc()), "Description must be non-empty");

   return *this;
}

/**
 * TODO count values
 */
auto ym::ArgParser::Arg::defaultVal(str const DefaultVal) -> Arg &
{
   // ymAssert<ArgParserError_ValEmpty>(!getVal(), "Value must not have already been set");

   _val = DefaultVal;

   // ymAssert<ArgParserError_ValInUse>(ymIsStrNonEmpty(getVal()), "Val must be non-empty");

   return *this;
}

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
