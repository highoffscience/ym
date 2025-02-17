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

/**
 * TODO
 *
 * @todo std::stacktrace.
 */
std::string ym::ymassert_Base::format(
   rawstr const     Format,
   fmt::format_args args)
{
   std::string msg(getMaxMsgSize_bytes(), '\0');

   [[maybe_unused]]
   auto const Result = fmt::vformat_to_n(
      msg.data(),
      msg.size(),
      Format,
      args);

   return msg;
}

/** what
 *
 * @brief Returns an identifying message.
 */
auto ym::ymassert_Base::what(void) const noexcept -> rawstr
{
   return _Msg.c_str();
}

#if (YM_NO_EXCEPTIONS)

/**
 * TODO
 */
void ym::ymassert_Base::defaultNoExceptHandler(ymassert_Base const & E)
{
   ymLog(E.what());
   std::raise(SIGTERM);
}

#endif
