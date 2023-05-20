/**
 * @file    utception.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "utception.h"

#include <utility>

/** Utception
 *
 * @brief Constructor.
 */
ym::ut::Utception::Utception(std::string msg)
   : _Msg {std::move(msg)}
{
}

/** Utception
 *
 * @brief Returns the stored message.
 * 
 * @note We return a char const * instead of str to keep in line with function definition
 *       of what() first declared in std::exception.
 * 
 * @returns char const * -- The stored message.
 */
char const * ym::ut::Utception::what(void) const noexcept
{
   return _Msg.c_str();
}
