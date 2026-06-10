/**
 * @file    ymassert.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymassert.h"

#include "textlogger.h"

#include "fmt/format.h"

#if (YM_NO_EXCEPTIONS)
   #include <csignal>
#endif

#if defined(YM_DEBUG)
   #include <stacktrace>
#endif

/** write_Helper
 *
 * @brief Write message into buffer.
 *
 * @param Format -- Format string.
 * @param args   -- Arguments.
 *
 * @todo std::stacktrace.
 */
void ym::ymassert_Base::write_Helper(
   rawstr const     Format,
   fmt::format_args args)
{
   auto result = fmt::vformat_to_n(
      _msg,
      _s_MaxMsgSize_bytes - 1uz,
      Format,
      args);
   *result.out = '\n';
   result.size++;

#if defined(YM_DEBUG)

   result.out++;
   if (_s_MaxMsgSize_bytes > result.size)
   {
      result = fmt::format_to_n(
         result.out,
         _s_MaxMsgSize_bytes - result.size - 1uz,
         "{}",
         std::stacktrace::current());
      *result.out = '\0';
      result.size = std::distance(_msg, result.out);
   }

#endif
}

#if (YM_YES_EXCEPTIONS)

   /** what
    *
    * @brief Returns an identifying message.
    */
   auto ym::ymassert_Base::what(void) const noexcept -> rawstr
   {
      return _msg;
   }

#else // YM_NO_EXCEPTIONS

   /** defaultNoExceptHandler
    *
    * @brief Logs the error message and raises interrupt.
    *
    * @param E -- Raised error.
    */
   void ym::ymassert_Base::defaultNoExceptHandler(ymassert_Base const & E)
   {
      logAssert(E);
      std::raise(SIGTERM);
   }

   /** logAssert
    *
    * @brief Logs the assert to the global logger.
    *
    * @param E -- Assert to log.
    */
   void ym::ymassert_Base::logAssert(ymassert_Base const & E)
   {
      ymLog(VF::Error, E.what());
   }

#endif
