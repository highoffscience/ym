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
 * @brief Constructor.
 */
auto ym::ut::Utception::what(void) const noexcept -> utstr
{
   return _Msg.c_str();
}
