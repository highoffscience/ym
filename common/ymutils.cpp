/**
 * @file    ymutils.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymutils.h"

#include "fmt/format.h"

/** formatter
 *
 * @brief Helper class to format ym::str types for use in the fmt library.
 */
auto fmt::formatter<ym::str>::format(ym::str s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator
{
   return fmt::formatter<fmt::string_view>::format(s.get(), ctx_ref);
}

/** formatter
 *
 * @brief Helper class to format ym::strlit types for use in the fmt library.
 */
auto fmt::formatter<ym::strlit>::format(ym::strlit s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator
{
   return fmt::formatter<fmt::string_view>::format(s.get(), ctx_ref);
}

/** formatter
 *
 * @brief Helper class to format ym::mutstr types for use in the fmt library.
 */
auto fmt::formatter<ym::mutstr>::format(ym::mutstr s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator
{
   return fmt::formatter<fmt::string_view>::format(s.get(), ctx_ref);
}

#if defined(YM_DEBUG)

   /** formatter
    *
    * @brief Helper class to format ym::mutstr types for use in the fmt library.
    */
   auto fmt::formatter<std::stacktrace>::format(std::stacktrace s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator
   {
      return fmt::formatter<fmt::string_view>::format(std::to_string(s), ctx_ref);
   }

#endif
