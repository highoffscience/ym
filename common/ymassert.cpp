/**
 * @file    ymassert.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymassert.h"

#include "globallogger.h"

#include "fmt/format.h"

#if (YM_EXCEPTIONS_ENABLED == 0)
   #include <csignal>
#endif

#if (YM_CPP_STANDARD >= 23) && (YM_LITE == 0)
   #include <stacktrace>
#endif

#if (YM_EXCEPTIONS_ENABLED)

   /**
    * @brief Returns an identifying message.
    *
    * @returns rawstr -- Stored message.
    */
   auto ym::ymassert_Base::what(void) const noexcept -> rawstr
   {
      return _msg.data();
   }

#else // exceptions disabled

   /**
    * @brief Logs the error message and raises interrupt.
    *
    * @param E -- Raised error.
    */
   void ym::ymassert_Base::defaultNoExceptHandler(ymassert_Base const & E) noexcept
   {
      logAssert(E);
      std::raise(SIGTERM);
   }

   /**
    * @brief Logs the assert to the global logger.
    *
    * @param E -- Assert to log.
    */
   void ym::ymassert_Base::logAssert(ymassert_Base const & E) noexcept
   {
      ymLog(VF::Error, E.what());
   }

#endif

/**
 * @brief Write message into buffer.
 *
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
void ym::ymassert_Base::write_Helper(
   rawstr const     Format,
   fmt::format_args args) noexcept
{
#if (YM_EXCEPTIONS_ENABLED)
   try
   { // formatting may fail - library functions are not marked noexcept
#endif
      auto result = fmt::vformat_to_n(
         _msg.data(),
         _msg.size() - 1uz,
         Format,
         args);
      *result.out = '\n';
      result.size++;

   #if (YM_CPP_STANDARD >= 23) && (YM_LITE == 0)
      result.out++;
      if (_msg.size() > result.size)
      {
         result = fmt::format_to_n(
            result.out,
            _msg.size() - result.size - 1uz,
            "{}",
            std::stacktrace::current());
         *result.out = '\0';
         result.size = std::distance(_msg.data(), result.out);
      }
   #endif
   }
#if (YM_EXCEPTIONS_ENABLED)
   catch (std::exception const & E)
   { // formatting message failed - try printing to error stream
      ymLog(VF::Warning,
         "Exception message could not be formatted! Format was '{}'. E.what() is '{}'", Format, E.what());
   }
#endif
}
