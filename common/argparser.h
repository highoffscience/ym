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
private:
   /**
    * TODO
    */
   class Arg
   {
   public:
      friend class ArgParser;

      explicit Arg(str         const Name,
                   ArgParser * const ap_Ptr);

      inline auto getName(void) const { return _name; }
      inline auto getDesc(void) const { return _desc; }
      inline auto getVal (void) const { return _val;  }
      inline auto getAbbr(void) const { return _abbr; }

      Arg & desc      (str  const Desc      );
      Arg & defaultVal(str  const DefaultVal);
      Arg & abbr      (char const Abbr      );

   private:
      void setVal(str const Val);

      // no consts! std::sort needs to have assignables
      ArgParser * _ap_Ptr;
      str         _name; // arg name (used as the key)
      str         _desc; // description
      str         _val;  // value
      char        _abbr; // abbreviation
   };

public:
   explicit ArgParser(void);

   inline Arg arg(str const Name);

   void parse(std::vector<Arg> && args_uref,
              int const           Argc,
              str const * const   Argv_Ptr);

   Arg * getArgPtr(str const Key);

   YM_DECL_YMCEPT(ArgParserError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NameEmpty)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NameInvalid)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescEmpty)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescInUse)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValEmpty )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValInUse )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AbbrInvalid )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AbbrInUse )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_LongHandNoVal )

private:
   Arg * getArgPtr_soft(str const Key);

   std::vector<Arg>          _args;
   std::array<Arg *, 52_u64> _abbrs;
};

/**
 * TODO
 */
inline auto ArgParser::arg(str const Name) -> Arg
{
   return Arg(Name, this);
}

} // ym
