/**
 * @file    ymutils.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymutils.h"

#include "fmt/format.h"

/**
 * TODO
 */
auto fmt::formatter<ym::str>::format(ym::str s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator
{
    return fmt::formatter<fmt::string_view>::format(fmt::string_view(s.get()), ctx_ref);
}

/**
 * TODO
 */
auto fmt::formatter<ym::strlit>::format(ym::strlit s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator
{
    return fmt::formatter<fmt::string_view>::format(fmt::string_view(s.get()), ctx_ref);
}
