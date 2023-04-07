/**
 * @file    argparser.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

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
      friend class ArgParser; // for setVal() and enable()

      explicit Arg(str const Name);

      inline auto getName(void) const { return _name; }
      inline auto getDesc(void) const { return _desc; }
      inline auto getVal (void) const { return _val;  }

      inline auto isFlag (void) const { return (getVal() == _s_TrueFlag ) ||
                                               (getVal() == _s_FalseFlag); }
      inline auto isEnbl (void) const { return  getVal() == _s_TrueFlag;   }

      Arg & desc(str  const Desc       );
      Arg & val (str  const DefaultVal );
      Arg & abbr(char const Abbr       );
      Arg & flag(bool const DefaultEnbl);

   private:
      static constexpr str _s_TrueFlag  = "1";
      static constexpr str _s_FalseFlag = "0";

      void setVal(str  const Val );
      void enable(bool const Enbl);

      // no consts! std::sort needs to have assignables
      str _name; // arg name (used as the key)
      str _desc; // description
      str _val;  // value
   };

   explicit ArgParser(void);

public:
   YM_NO_COPY  (ArgParser)
   YM_NO_ASSIGN(ArgParser)

   static ArgParser * getInstancePtr(void);

   inline Arg arg(str const Name);

   void parse(std::vector<Arg> && args_uref,
              int const           Argc,
              str const * const   Argv_Ptr);

   Arg * getArgPtr(str const Key);

   Arg * operator[](str const Key);

   YM_DECL_YMCEPT(ArgParserError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ParseError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ArgError  )

private:
   Arg * getArgPtrFromPrefix(str  const Key );
   Arg * getArgPtrFromAbbr  (char const Abbr);

   uint32 getAbbrIdx(char const Abbr);

   std::vector<Arg>          _args;
   std::array<Arg *, 62_u64> _abbrs;
};

/**
 * TODO
 */
inline auto ym::ArgParser::arg(str const Name) -> Arg
{
   return Arg(Name);
}

} // ym
