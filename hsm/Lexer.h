/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Token.h"

#include <string>

namespace ym::parse
{

/**
 * Tokenizes the input stream.
 *
 * The lexer is only used inside the parser, and is set only at
 *  a top level function so the input stream is guaranteed to live
 *  throughout the lexers life.
 */
class Lexer
{
public:
   explicit Lexer(void);
   explicit Lexer(std::string const & Stream);

   YM_NO_COPY(Lexer);
   Lexer & operator = (Lexer const & Other) = default;

   Token getNextToken(void);

   inline bool isStreamExhausted(void) const;

private:
   char const * _p_Stream;
   uint32       _index;
   uint32       _streamSize; // includes null terminator
};

/**
 *
 */
bool Lexer::isStreamExhausted(void) const
{
   return _index >= _streamSize;
}

} // ym::parse
