/**
 * @author Forrest Jablonski
 */

#include "Parser.h"

#include "Assert.h"
#include "Logger.h"

#include "terminal/TerminalNodeDeclarations.h"

#define YM_VERBOSITY 0
#define YM_ANNOUNCE_ENTRY(Depth)                                      \
   { /* new scope to hide temp vars */                                \
      std::string tmp__spaces__;                                      \
      for (uint32 i = 0; i < (Depth); ++i)                            \
      {                                                               \
         tmp__spaces__ += " ";                                        \
      }                                                               \
      YM_LOG(YM_VERBOSITY, "|%s%s", tmp__spaces__.c_str(), __func__); \
   } (void)0

/*
 * Feed the grammar below into <http://hackingoff.com/compilers/ll-1-parser-generator>

Unit -> Expr

Expr -> Term TermTail

TermTail -> + Term TermTail
TermTail -> - Term TermTail
TermTail -> $

Term -> Fact FactTail

FactTail -> * Fact FactTail
FactTail -> / Fact FactTail
FactTail -> $

Fact ->   Primary
Fact -> + Primary
Fact -> - Primary

Primary -> Number
Primary -> ( Expr )

 */

/**
 *
 */
ym::parse::Parser::Parser(void)
   : _lexer {},
     _token {Token::Type_T::Invalid}
{
}

/**
 *
 */
auto ym::parse::Parser::parse(std::string const & Stream) -> terminal::Node *
{
   _lexer = Lexer(Stream);
   _token = _lexer.getNextToken();

   return visitUnit(0);
}

/**
 *
 */
auto ym::parse::Parser::visitUnit(uint32 const Depth) -> terminal::Node *
{
   YM_ANNOUNCE_ENTRY(Depth);

   switch (_token.getType())
   {
      case Token::Type_T::Plus:
      case Token::Type_T::Minus:
      case Token::Type_T::Number:
      case Token::Type_T::LeftParen:
      {
         return visitExpression(Depth + 1);
      }

      default:
      {
         // TODO error
         return nullptr;
      }
   }
}

/**
 *
 */
auto ym::parse::Parser::visitExpression(uint32 const Depth) -> terminal::Node *
{
   YM_ANNOUNCE_ENTRY(Depth);

   switch (_token.getType())
   {
      case Token::Type_T::Plus:
      case Token::Type_T::Minus:
      case Token::Type_T::Number:
      case Token::Type_T::LeftParen:
      {
         return visitTermTail(Depth + 1, visitTerm(Depth + 1));
      }

      default:
      {
         // TODO error
         return nullptr;
      }
   }
}

/**
 *
 */
auto ym::parse::Parser::visitTerm(uint32 const Depth) -> terminal::Node *
{
   YM_ANNOUNCE_ENTRY(Depth);

   switch (_token.getType())
   {
      case Token::Type_T::Plus:
      case Token::Type_T::Minus:
      case Token::Type_T::Number:
      case Token::Type_T::LeftParen:
      {
         return visitFactorTail(Depth + 1, visitFactor(Depth + 1));
      }

      default:
      {
         // TODO error
         return nullptr;
      }
   }
}

/**
 *
 */
auto ym::parse::Parser::visitTermTail(uint32 const Depth, terminal::Node * const P_lhs) -> terminal::Node *
{
   YM_ANNOUNCE_ENTRY(Depth);

   switch (_token.getType())
   {
      case Token::Type_T::Plus:
      {
         return termTailHelper(Depth, P_lhs, new terminal::Plus());
      }

      case Token::Type_T::Minus:
      {
         return termTailHelper(Depth, P_lhs, new terminal::Minus());
      }
   
      default: // epsilon
      {
         return P_lhs;
      }
   }
}

/**
 *
 */
auto ym::parse::Parser::termTailHelper(uint32             const Depth,
                                       terminal::Node   * const P_lhs,
                                       terminal::Binary * const P_op) -> terminal::Node *
{
   _token = _lexer.getNextToken();

   P_op->setLhs(P_lhs);
   P_op->setRhs(visitTerm(Depth + 1));

   return visitTermTail(Depth + 1, P_op);
}

/**
 *
 */
auto ym::parse::Parser::visitFactor(uint32 const Depth) -> terminal::Node *
{
   YM_ANNOUNCE_ENTRY(Depth);

   switch (_token.getType())
   {
      case Token::Type_T::Number:
      case Token::Type_T::LeftParen:
      case Token::Type_T::Plus:
      {
         return visitPrimary(Depth + 1);
      }

      case Token::Type_T::Minus:
      {
         using terminal::Negate;
         Negate * const P_negate = new Negate();
         P_negate->setOperand(visitPrimary(Depth + 1));
         return P_negate;
      }

      default:
      {
         // TODO error
         return nullptr;
      }
   }
}

/**
 *
 */
auto ym::parse::Parser::visitFactorTail(uint32 const Depth, terminal::Node * const P_lhs) -> terminal::Node *
{
   YM_ANNOUNCE_ENTRY(Depth);

   switch (_token.getType())
   {
      case Token::Type_T::Times:
      {
         return termTailHelper(Depth, P_lhs, new terminal::Times());
      }

      case Token::Type_T::Divide:
      {
         return termTailHelper(Depth, P_lhs, new terminal::Divide());
      }
   
      default: // epsilon
      {
         return P_lhs;
      }
   }
}

/**
 *
 */
auto ym::parse::Parser::factorTailHelper(uint32             const Depth,
                                         terminal::Node   * const P_lhs,
                                         terminal::Binary * const P_op) -> terminal::Node *
{
   _token = _lexer.getNextToken();

   P_op->setLhs(P_lhs);
   P_op->setRhs(visitFactor(Depth + 1));

   return visitFactorTail(Depth + 1, P_op);
}

/**
 *
 */
auto ym::parse::Parser::visitPrimary(uint32 const Depth) -> terminal::Node *
{
   YM_ANNOUNCE_ENTRY(Depth);

   switch (_token.getType())
   {
      case Token::Type_T::Number:
      {
         using terminal::Number;
         Number * const P_number = new Number(_token.getFloat32());
         _token = _lexer.getNextToken();
         return P_number;
      }

      case Token::Type_T::LeftParen:
      {
         _token = _lexer.getNextToken();
         terminal::Node * const P_expr = visitExpression(Depth + 1);
         if (_token.getType() == Token::Type_T::RightParen)
         {
            _token = _lexer.getNextToken();
            return P_expr;
         }
         else
         {
            // TODO error
            return nullptr;
         }
      }

      default:
      {
         // TODO error
         return nullptr;
      }
   }
}
