/**
 * @file    ymassert.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymassert.h"

#if (YM_NO_EXCEPTIONS)
   #include "textlogger.h"
   #include <csignal>
#endif

/** write
 *
 * @brief Write message into buffer.
 *
 * @param Format -- Format string.
 * @param args   -- Arguments.
 *
 * @todo std::stacktrace.
 */
void ym::ymassert_Base::write(
   rawstr const     Format,
   fmt::format_args args)
{
   auto const Result = fmt::vformat_to_n(
      _msg,
      getMaxMsgSize_bytes() - std::size_t(1u),
      Format,
      args);
   *Result.out = '\0';
}

/** what
 *
 * @brief Returns an identifying message.
 */
auto ym::ymassert_Base::what(void) const noexcept -> rawstr
{
   return _msg;
}

#if (YM_NO_EXCEPTIONS)

/** defaultNoExceptHandler
 *
 * @brief Logs the error message and raises interrupt.
 *
 * @param E -- Raised error.
 */
void ym::ymassert_Base::defaultNoExceptHandler(ymassert_Base const & E)
{
   ymLog(E.what());
   std::raise(SIGTERM);
}

#endif
