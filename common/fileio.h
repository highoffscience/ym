/**
 * @file    fileio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include "memio.h"

#include <cstdio>
#include <optional>
#include <span>
#include <string>
#include <tuple>

namespace ym
{

/**
 * @brief Provides file IO and manipulation functions.
 *
 * TODO usage outline
 *
 * auto filename = StackStrLit("ym/common/fileio/data.txt");
 * auto file = FileIO(&filename, "w");
 */
class FileIO : public StackBufferUser
{
public:
   explicit inline FileIO(void) noexcept = default;
   implicit inline FileIO(
      bound<StackBuffer_Base> const Filename,
      str                     const Mode = "rb") noexcept;

   /// @brief Access mode bit positions.
   enum AccessModeFlags_T {
      Read,
      Write
   };

   static bool exists(str const Filename) noexcept;

   /// @name FileIO Queryers.
   /// @{
   /// @brief Getters.
   /// @returns auto -- Self explanatory.
   inline bool isOpen(void) const noexcept { return _file; }
   inline operator bool(void) const noexcept { return isOpen(); }
   /// @}

   bool reset(
      bound<StackBuffer_Base> const Filename,
      str                     const Mode = "rb") noexcept;

   std::size_t getSize(void) noexcept;
   std::optional<std::size_t> calculateSize(void) const noexcept;

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
   loose<std::FILE>        _file_ptr {nullptr};
   bound<StackBuffer_Base> _filename_ptr;
   std::size_t             _size     {  0uz  };
   ByteBitset              _flags    {       };
};

/**
 * @brief Constructor.
 *
 * @param Filename -- Name of file to open.
 * @param Mode     -- Opening mode (read/write/append, etc.)
 */
inline FileIO::FileIO(
   str const Filename,
   str const Mode) noexcept
{
   std::ignore = reset(Filename, Mode);
}

} // ym
