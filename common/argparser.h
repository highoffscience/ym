/**
 * @file    argparser.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "ymception.h"

#include <array>
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
      explicit Arg(str         const Name,
                   ArgParser * const ap_Ptr);

      inline auto getName(void) const { return _Name; }
      inline auto getDesc(void) const { return _desc; }
      inline auto getVal (void) const { return _val;  }
      inline auto getAbbr(void) const { return _abbr; }

      Arg & desc      (str  const Desc      );
      Arg & defaultVal(str  const DefaultVal);
      Arg & abbr      (char const Abbr      );

   private:
      ArgParser * const _ap_Ptr;
      str         const _Name; // arg name (used as the key)
      str               _desc; // description
      str               _val;  // value
      char              _abbr; // abbreviation
   };

   explicit ArgParser(void);

   void init(std::vector<Arg> && args_uref);

   Arg arg(str const Name);

   void parse(int const         Argc,
              str const * const Argv_Ptr);

   Arg * getArgPtr(str const Key);

   YM_DECL_YMCEPT(ArgParserError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NameEmpty)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescEmpty)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescInUse)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValEmpty )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValInUse )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_InvalidAbbr )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AbbrInUse )

private:
   std::vector<Arg>          _args;
   std::array<Arg *, 52_u64> _abbrs;
};

} // ym
