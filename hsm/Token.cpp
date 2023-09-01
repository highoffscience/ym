/**
 * @author Forrest Jablonski
 */

#include "Token.h"

/**
 * Stringify the enum.
 */
#pragma warning(push)
#pragma warning(disable:4715) // not all control paths return a value
char const * ym::parse::Token::asString(Type_T const Type)
#pragma warning(pop)
{

   switch (Type)
   {
      case Type_T::Plus:       return "Plus";
      case Type_T::Minus:      return "Minus";
      case Type_T::Times:      return "Times";
      case Type_T::Divide:     return "Divide";
      case Type_T::Number:     return "Number";
      case Type_T::LeftParen:  return "LeftParen";
      case Type_T::RightParen: return "RightParen";
      case Type_T::Invalid:    return "Invalid";
      case Type_T::Lambda:     return "Lambda";
   }

   // UNREACHABLE
}
