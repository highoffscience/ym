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
#include <memory>
#include <span>
#include <string>
#include <variant>

namespace ym
{

/**
 * @brief Provides file IO and manipulation functions.
 *
 * - Example usage:
 *   @code{cpp}
 *      auto filename = StackStrLit("ym/common/fileio/data.txt");
 *      auto file = FileIO(&filename, "w");
 *      // or
 *      auto file = FileIO("ym/common/fileio/data.txt", "w");
 *   @endcode
 *
 * __Unit Test__
 * - @ref ym::unit::fileio::TestSuite::Class_FileIO.
 *
 * @test Shall ?
 */
class FileIO : public StackBufferUser
{
private:
   using Filename_T = std::variant<strlit, bound<StackBuffer_Base>>;

public:
   implicit inline FileIO(
      bound<StackBuffer_Base> const Filename,
      str                     const Mode = "rb") noexcept;

   implicit inline FileIO(
      strlit const Filename,
      str    const Mode = "rb") noexcept;



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
   inline bool isOpen(void) const noexcept { return _file_ptr != nullptr; }
   inline operator bool(void) const noexcept { return isOpen(); }
   /// @}

   bool reset(
      Filename_T const Filename,
      str        const Mode = "rb") noexcept;

   std::optional<std::size_t> getSize(bool const Force = false) const noexcept;

   /// @name FileIO Getters.
   /// @{
   /// @brief Getter. Guaranteed not null.
   /// @throws ym_NullPtrError -- If underlying file handle is null.
   /// @returns std::FILE * -- Raw underlying file handle. Guaranteed not null.
   inline auto * get                  (this auto && self) { return self._file_ptr->get(); }
   inline        operator std::FILE * (this auto && self) { return self.get(); }
   inline auto * operator *           (this auto && self) { return self.get(); }
   /// @}

   /// @brief Gets filename.
   /// @returns str -- Filename.
   inline str getFilename(void) const noexcept {
      return std::visit(ym_visit_overloaded_t{
         [](strlit                  const Arg) -> str { return Arg;           },
         [](bound<StackBuffer_Base> const Arg) -> str { return Arg->getStr(); }
      }, _filename);
   }

#if (YM_USE_HEAP_AS_FALLBACK)
   std::optional<std::string> createAndFillBuffer(void) noexcept;
#endif
   bool                           fillBuffer         (std::span<char> buffer, bool const AppendNull = true) noexcept;
   std::optional<std::span<char>> fillBufferPiecewise(std::span<char> buffer) noexcept;

private:
   /// @cond INTERNAL
   struct Deleter {
      using pointer = loose<std::FILE>;
      inline void operator () (pointer f) const noexcept {
         std::ignore = f.invoke(std::fclose);
      }
   };
   /// @endcond

   std::unique_ptr<
      std::FILE,
      Deleter>  _file_ptr {nullptr};
   Filename_T   _filename;
   std::size_t  _size     {  0uz  };
   ByteBitset   _flags    {       };
};

/**
 * @brief Constructor.
 *
 * @param Filename -- Name of file to open.
 * @param Mode     -- Opening mode (read/write/append, etc.)
 */
inline FileIO::FileIO(
   bound<StackBuffer_Base> const Filename,
   str                     const Mode) noexcept :
      StackBufferUser(Filename),
      _filename {Filename}
{
   std::ignore = reset(_filename, Mode);
}
inline FileIO::FileIO(
   strlit const Filename,
   str    const Mode) noexcept :
      _filename {Filename}
{
   std::ignore = reset(_filename, Mode);
}

} // ym
