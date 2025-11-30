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
   /// @brief Options surrounding file configs.
   using Options_T = TextLogger::Options_T;

   explicit LiteTextLogger(
      strlit    const   Filename,
      Options_T const & Options = getDefaultOptions());

   YM_NO_COPY  (LiteTextLogger)
   YM_NO_ASSIGN(LiteTextLogger)

protected:
   virtual void closeOutfile(void) override;
   virtual void producer(
      strlit const     Format,
      fmt::format_args args) override;
};

} // ym
