/**
 * @author Forrest Jablonski
 */

#include "hoso.h"

#include <cstdio>
#include <exception>
#include <vector>

namespace hoso
{

/**
 * Arg names must be string literals
 */
class ArgParser
{
public:
   /**
    *
    */
   class Arg
   {
   public:
      explicit Arg(str const Name);

      inline auto getName (void) const { return _Name;                                  }
      inline auto getDesc (void) const { return _desc;                                  }
      inline auto getVal  (void) const { return isBinary() ? _binVal : _args[_val_idx]; }
      inline auto getAbbr (void) const { return _abbr;                                  }
      inline auto isBinary(void) const { return _val_idx == 0;                          }

      Arg * desc      (str  const Desc      );
      Arg * defaultVal(str  const DefaultVal);
      Arg * defaultVal(bool const DefaultVal); // sets the arg to be binary
      Arg * abbr      (char const Abbr      );
      Arg * binary    (void                 );

   private:
      str const _Name;    // arg name (used as the key)
      str const _desc;    // description

      // TODO because these structures are in vectors which do reallocate this
      //      *cannot* be a pointer
      uint32      _val_idx; // index of value (0 indicates binary value)
      char        _abbr;    // abbreviation of name
      bool        _binVal;  // value, if applicable
   };

   /**
    *
    */
   class ParseError : public std::exception
   {
   public:
      template<typename... Args_T>
      explicit ParseError(Str_T  const    Format,
                          Args_T const... Args);
      virtual ~ParseError(void) = default;

      virtual Str_T what(void) const noexcept override;

   private:
      static constexpr uint32 MsgSize_bytes = 128;
      char _msg[MsgSize_bytes];
   };

   static ArgParser * getInstancePtr(void);

   Arg * add(Str_T const Name);

   void parse(int   const         Argc,
              Str_T const * const Argv_Ptr);

   Arg * get(Str_T const Key);

private:
   explicit ArgParser(void) = default;

   static std::vector<Arg> _args;
   static uint16           _abbrs[52];
};

/**
 *
 */
template<typename... Args_T>
ArgParser::ParseError::ParseError(Str_T  const    Format,
                                  Args_T const... Args)
   : _msg {'\0'}
{
   (void)std::snprintf(_msg, MsgSize_bytes, Format, Args...);
}

} // hoso
