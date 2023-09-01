/**
 * @author Forrest Jablonski
 */

#include "Lexer.h"

#include "Assert.h"
#include "Logger.h"

#include <cstdlib>

#define YM_GET_NEXT_CHAR() _p_Stream[_index]
#define YM_GET_NEXT_CHAR_AND_ADVANCE_INDEX() _p_Stream[_index++]
#define YM_ADVANCE_INDEX_AND_GOTO(Label_State) ++_index; goto Label_State

/**
 * Initialize members.
 *
 * Default state should be unusable.
 */
ym::parse::Lexer::Lexer(void)
   : _p_Stream   {nullptr},
     _index      {0},
     _streamSize {0}
{
}

/**
 * Initialize members.
 */
ym::parse::Lexer::Lexer(std::string const & Stream)
   : _p_Stream   {Stream.c_str()},
     _index      {0},
     _streamSize {static_cast<uint32>(Stream.size()) + 1} // +1 for null terminator which is a valid character
{
}

/**
 * Lexer state machine. Tokenizes and returns the associated node.
 */
auto ym::parse::Lexer::getNextToken(void) -> Token
{
   YM_ASSERT(!isStreamExhausted(), YM_LOG,
      "Stream has been exhausted! Trying to access index %lu, max is %lu",
      _index, static_cast<int32>(_streamSize) - 1);

   uint32 startIndex = 0;
   char c = '\0';

// -----------------------------------------------------------------------------

Label_State_0:
{
   startIndex = _index;
   c = YM_GET_NEXT_CHAR_AND_ADVANCE_INDEX();

   if (c == ' '  ||
       c == '\n' ||
       c == '\t' ||
       c == '\r') { goto Label_State_0; }
   if (c == '\0') { return Token(Token::Type_T::Lambda    ); }
   if (c == '+' ) { return Token(Token::Type_T::Plus      ); }
   if (c == '-' ) { return Token(Token::Type_T::Minus     ); }
   if (c == '*' ) { return Token(Token::Type_T::Times     ); }
   if (c == '/' ) { return Token(Token::Type_T::Divide    ); }
   if (c == '(' ) { return Token(Token::Type_T::LeftParen ); }
   if (c == ')' ) { return Token(Token::Type_T::RightParen); }
   if (c >= '0' &&
       c <= '9' ) { goto Label_State_1; }

   return Token(Token::Type_T::Invalid, {c});
}

// -----------------------------------------------------------------------------

Label_State_1:
{
   c = YM_GET_NEXT_CHAR();

   if (c >= '0' &&
       c <= '9') { YM_ADVANCE_INDEX_AND_GOTO(Label_State_1); }
   if (c == '.') { YM_ADVANCE_INDEX_AND_GOTO(Label_State_2); }

   return Token(Token::Type_T::Number, {std::strtof(_p_Stream + startIndex, nullptr)});
}

// -----------------------------------------------------------------------------

Label_State_2:
{
   c = YM_GET_NEXT_CHAR();

   if (c >= '0' &&
       c <= '9') { YM_ADVANCE_INDEX_AND_GOTO(Label_State_2); }

   return Token(Token::Type_T::Number, {std::strtof(_p_Stream + startIndex, nullptr)});
}

   // UNREACHABLE
}
