/**
 * @file    litetextlogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "logger.h"
#include "ymglobals.h"

#include "fmt/base.h"

namespace ym
{

/** LiteLogger
 *
 * @brief Logs text to the given outfile - similary to std::printf.
 */
class LiteLogger : public Logger
{
public:
   /// @brief Options surrounding file configs.
   using Options_T = Logger::Options_T;

   /// @brief Returns default options.
   static constexpr Options_T getDefaultOptions(void) { return {}; }
   inline virtual Options_T const & getOptions(void) const override { return _Options; }

   explicit LiteLogger(
      strlit    const   Filename,
      Options_T const & Options = getDefaultOptions());

   YM_NO_COPY  (LiteLogger)
   YM_NO_ASSIGN(LiteLogger)

   virtual bool isOpen(void) const override;
   virtual bool open  (void) override;
   virtual void close (void) override;

protected:
   virtual void producer(
      strlit const     Format,
      fmt::format_args args) override;
   
private:
   Options_T const _Options{};
};

} // ym
