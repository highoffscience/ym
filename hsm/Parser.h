/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Lexer.h"
#include "Token.h"

#include <string>

namespace ym::parse
{

namespace terminal
{
   class Node;
   class Number;
   class Binary;
   class Unary;
} // terminal

/**
 *
 */
class Parser
{
public:
   explicit Parser(void);

   YM_NO_COPY(Parser);
   YM_NO_ASSIGN(Parser);

   terminal::Node * parse(std::string const & Stream);

private:
   Lexer  _lexer;
   Token  _token;

   terminal::Node * visitUnit      (uint32 const Depth);
   terminal::Node * visitExpression(uint32 const Depth);
   terminal::Node * visitTerm      (uint32 const Depth);
   terminal::Node * visitTermTail  (uint32 const Depth, terminal::Node * const P_lhs);
   terminal::Node * visitFactor    (uint32 const Depth);
   terminal::Node * visitFactorTail(uint32 const Depth, terminal::Node * const P_lhs);
   terminal::Node * visitPrimary   (uint32 const Depth);

   terminal::Node * termTailHelper  (uint32             const Depth,
                                     terminal::Node   * const P_lhs,
                                     terminal::Binary * const P_op);
   terminal::Node * factorTailHelper(uint32             const Depth,
                                     terminal::Node   * const P_lhs,
                                     terminal::Binary * const P_op);
};

} // ym
