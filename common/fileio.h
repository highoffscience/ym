/**
 * @file    fileio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <cstdio>
#include <optional>
#include <span>
#include <string>
#include <tuple>

namespace ym
{

/**
 * @brief Provides file IO and manipulation functions.
 */
class FileIO
{
public:
   explicit inline FileIO(void) noexcept = default;
   implicit inline FileIO(
      str const Filename,
      str const Mode = "rb") noexcept;

   static bool exists(str const Filename) noexcept;

   inline bool isOpen(void) const noexcept { return _file; }
   inline operator bool(void) const noexcept { return isOpen(); }

   inline auto getSize(void) const noexcept { return _size; }

   bool reset(
      str const Filename,
      str const Mode = "rb") noexcept;

   /// @name FileIO Getters.
   /// @{
   /// @brief Getter. Guaranteed not null.
   /// @throws ym_NullPtrError -- If underlying file handle is null.
   /// @returns std::FILE * -- Raw underlying file handle. Guaranteed not null.
   inline auto * get                  (this auto && self) { return self._file.unwrap().get(); }
   inline        operator std::FILE * (this auto && self) { return self.get(); }
   inline auto * operator *           (this auto && self) { return self.get(); }
   /// @}

#if (YM_USE_HEAP_AS_FALLBACK)
   std::optional<std::string> createAndFillBuffer(void) noexcept;
#endif
   bool                           fillBuffer         (std::span<char> buffer, bool const AppendNull = true) noexcept;
   std::optional<std::span<char>> fillBufferPiecewise(std::span<char> buffer) noexcept;

private:
   LoosePtr<std::FILE> _file{};
   std::size_t        _size{};
};

/**
 * @brief Constructor.
 */
inline FileIO::FileIO(
   str const Filename,
   str const Mode) noexcept
{
   std::ignore = reset(Filename, Mode);
}

} // ym
