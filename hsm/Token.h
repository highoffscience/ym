/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#define YM_TOKEN_UNION_DECL_MEMBER(ValueType) \
   inline /* implicit */ Data_T(ValueType const Value) : _value_##ValueType {Value} {} \
   ValueType _value_##ValueType

namespace ym::parse
{

/**
 * Stores lexemes.
 */
class Token
{
public:
   enum class Type_T
   {
      Plus,
      Minus,
      Times,
      Divide,
      Number,
      LeftParen,
      RightParen,
      Invalid,
      Lambda
   };

   static char const * asString(Type_T const Type);

private:
   union Data_T
   {
      YM_TOKEN_UNION_DECL_MEMBER(float32);
      YM_TOKEN_UNION_DECL_MEMBER(char   );
   };

public:
   inline explicit Token(Type_T const Type);
   inline explicit Token(Type_T const Type, Data_T const Data);

   inline Type_T getType(void) const { return _type; }

   inline float32 getFloat32(void) const { return _data._value_float32; }
   inline char    getChar   (void) const { return _data._value_char;    }

private:
   Type_T _type;
   Data_T _data;
};

/**
 * Initializes variables.
 */
Token::Token(Type_T const Type)
   : Token(Type, {'\0'})
{
}

/**
 * Initializes variables.
 */
Token::Token(Type_T const Type, Data_T const Data)
   : _type {Type},
     _data {Data}
{
}

} // ym::parse

#undef YM_TOKEN_UNION_DECL_MEMBER
