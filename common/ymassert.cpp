/**
 * @file    ymassert.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymassert.h"

#include "textlogger.h"

#include "fmt/format.h"

#if defined(YM_DEBUG)
   #include <stacktrace>
#endif

#if (YM_NO_EXCEPTIONS)
   #include <csignal>
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
   auto const Result = fmt::vformat_to_n(
      _msg,
      _s_MaxMsgSize_bytes - 1uz,
      Format,
      args);
   *Result.out = '\0';

#if defined(YM_DEBUG)
   // std::cerr << std::stacktrace::current() << std::endl;

   // char* out = result.out;

   // auto append = [&](fmt::format_string<std::stacktrace> fmtstr,
   //                   std::stacktrace const& st)
   // {
   //    auto r = fmt::format_to_n(
   //       out,
   //       (_msg + _s_MaxMsgSize_bytes - 1) - out,
   //       fmtstr,
   //       st);

   //    out = r.out;
   // };

   // append("\n\nStack trace:\n    {}\n",
   //        std::stacktrace::current());

   // *out = '\0';
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
