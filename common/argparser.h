/**
 * @file    argparser.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "ymception.h"

#include <vector>

namespace ym
{

/**
 * TODO
 */
class ArgParser
{
public:
   /**
    * TODO
    */
   class Arg
   {
   public:
      explicit Arg(str const Name);

      inline auto getName(void) const { return _Name; }
      inline auto getDesc(void) const { return _desc; }
      inline auto getVal (void) const { return _val;  }

      Arg & desc      (str const Desc      );
      Arg & defaultVal(str const DefaultVal);

   private:
      str const _Name; // arg name (used as the key)
      str       _desc; // description
      str       _val;  // value
   };

   explicit ArgParser(std::vector<Arg> && args_uref);

   YM_DECL_YMCEPT(ArgParserError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NameEmpty)
   // YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescEmpty)
   // YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescInUse)
   // YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValEmpty )
   // YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValInUse )

   void parse(int const         Argc,
              str const * const Argv_Ptr);

   Arg * getArgPtr(str const Key);

private:
   std::vector<Arg> _args;
};

} // ym
