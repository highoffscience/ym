/**
 * @file    litetextlogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "textlogger.h"
#include "ymglobals.h"

#include "fmt/base.h"

#include <array>
#include <atomic>
#include <concepts>
#include <utility>

namespace ym
{

/** LiteTextLogger
 *
 * @brief Logs text to the given outfile - similary to std::printf.
 */
class LiteTextLogger : public TextLogger
{
public:
   /** Options_T
    * 
    * @brief Options surrounding file configs.
    */
   struct Options_T
   {
      /// @brief Options.
      TextLogger::Options_T _textLogOptions{TextLogger::getDefaultOptions()};

      /// @brief Convenience cast to pass to base Logger functions.
      constexpr operator TextLogger::Options_T(void) const { return _textLogOptions; }
   };

   static constexpr Options_T getDefaultOptions(void) { return {}; }

   explicit LiteTextLogger(
      str       const   Filename,
      Options_T const & Options = getDefaultOptions());

   YM_NO_COPY  (LiteTextLogger)
   YM_NO_ASSIGN(LiteTextLogger)

   YM_DECL_YMASSERT(Error)

   inline auto const & getOptions (void) const { return _Options;  }

protected:
   virtual void producer(
      str const        Format,
      fmt::format_args args) override;

private:
   Options_T const _Options{getDefaultOptions()};
};

} // ym
