/**
 * @file    litelogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "logger.h"
#include "ymglobals.h"

#include "fmt/base.h"

namespace ym
{

/**
 * @brief Logs text to the given outfile - similary to std::printf.
 */
class LiteLogger : public Logger
{
public:
   /// @brief Options surrounding file configs.
   using Options_T = Logger::Options_T;

   /// @brief Gets this logger's options.
   inline virtual Options_T const & getOptions(void) const noexcept override { return _Options; }

   explicit LiteLogger(
      strlit    const   Filename,
      Options_T const & Options = {}) noexcept;
   virtual ~LiteLogger(void) noexcept = default;

   YM_NO_COPY  (LiteLogger)
   YM_NO_ASSIGN(LiteLogger)

   // YM_DECL_YMASSERT(Logger::Error, Error)

   /// @brief Returns name of file.
   inline auto getFilename(void) const noexcept { return _Filename; }

   bool isOpen(void) const noexcept;
   bool open  (void) noexcept;
   void close (void) noexcept;

protected:
   virtual void producer(
      strlit const     Format,
      fmt::format_args args) noexcept override;

private:
   strlit    const _Filename{"unnamed_ll.uhoh"};
   Options_T const _Options {  /* default */  };
};

} // ym
