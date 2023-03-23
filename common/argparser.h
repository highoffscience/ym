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
      inline auto getFlag(void) const { return _flag; }
      inline auto getEnbl(void) const { return _enbl; }

      Arg & desc      (str  const Desc      );
      Arg & defaultVal(str  const DefaultVal);
      Arg & abbr      (char const Abbr      );
      Arg & flag      (void                 );
      Arg & enable    (bool const Enable    );

   private:
      void setVal(str const Val);

      // no consts! std::sort needs to have assignables
      ArgParser * _ap_Ptr;
      str         _name; // arg name (used as the key)
      str         _desc; // description
      str         _val;  // value
      char        _abbr; // abbreviation
      bool        _flag; // if binary value
      bool        _enbl; // enabled/disabled
   };

public:
   explicit ArgParser(void);

   Arg arg(str const Name);

   void parse(std::vector<Arg> && args_uref,
              int const           Argc,
              str const * const   Argv_Ptr);

   Arg * getArgPtr(str const Key);

   Arg * operator[](str const Key);

   YM_DECL_YMCEPT(ArgParserError)
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NameEmpty   )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NameInvalid )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescEmpty   )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_DescInUse   )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValEmpty    )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValInUse    )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AbbrInvalid )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AbbrInUse   )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AmbigPrefix )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NoArgFound  )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NoAbbrFound )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NoAbbrReg   )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_NoValFound  )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_ValWithFlag )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_AbbrNoFlag  )
   YM_DECL_YMCEPT(ArgParserError, ArgParserError_KeyInvalid  )

private:
   Arg *   getArgPtrFromPrefix (str  const Key );
   Arg *   getArgPtrFromAbbr   (char const Abbr);
   Arg * * getArgPtrPtrFromAbbr(char const Abbr);

   std::vector<Arg>          _args;
   std::array<Arg *, 62_u64> _abbrs;
};

} // ym
