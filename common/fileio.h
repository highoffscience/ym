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
 *      auto filename = <some objectd derived from StackBuffer_Base>;
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
   /**
    * @{
    * @brief Constructor.
    *
    * @param Filename -- Name of file to open.
    * @param Mode     -- Opening mode (read/write/append, etc.)
    */
   implicit inline FileIO(
      bound<StackBuffer_Base> const Filename,
      str                     const Mode = "rb") noexcept :
         StackBufferUser(Filename),
         _filename {Filename}
   {
      std::ignore = reset(_filename, Mode);
   }

   implicit inline FileIO(
      strlit const Filename,
      str    const Mode = "rb") noexcept :
         _filename {Filename}
   {
      std::ignore = reset(_filename, Mode);
   }
   /// @}

   /// @brief Access mode bit positions.
   enum AccessModeFlags_T {
      Read,
      Write
   };

   static bool exists(str const Filename) noexcept;

   /**
    * @{
    * @brief Determines if file is opened.
    *
    * @returns auto -- True if file opened, false otherwise.
    */
   inline bool isOpen(void) const noexcept { return _file_ptr != nullptr; }
   inline operator bool(void) const noexcept { return isOpen(); }
   /// @}

   bool reset(
      Filename_T const Filename,
      str        const Mode = "rb") noexcept;

   std::optional<std::size_t> getSize(bool const Force = false) const noexcept;

   /**
    * @{
    * @brief Getter. Guaranteed not null.
    *
    * @throws ym_NullPtrError -- If underlying file handle is null.
    *
    * @returns std::FILE * -- Raw underlying file handle. Guaranteed not null.
    */
   inline auto * get                  (this auto && self) { return self._file_ptr->get(); }
   inline        operator std::FILE * (this auto && self) { return self.get(); }
   inline auto * operator *           (this auto && self) { return self.get(); }
   /// @}

   /**
    * @brief Gets filename.
    *
    * @return str -- Filename.
    */
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
      using pointer = loose<std::FILE>; // must define fancy pointer
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

} // ym
