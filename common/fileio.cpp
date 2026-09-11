// /**
//  * @file    fileio.cpp
//  * @version 1.0.0
//  * @author  Forrest Jablonski
//  */

// #include "fileio.h"

// #include "globallogger.h"

// #include <fcntl.h>
// #include <filesystem>
// #include <sys/stat.h>
// #include <tuple>
// #include <unistd.h>

// /**
//  * @brief Resets the file handle.
//  *
//  * @param Filename -- Name of file to open.
//  */
// bool ym::FileIO::exists(str const Filename) noexcept
// {
//    auto found = false;

//    struct stat st;
//    if (stat(Filename, &st) == 0)
//    { // found file
//       found = true;
//    }
// #if (YM_USE_HEAP_AS_FALLBACK)
//    else
//    {
//       found = std::filesystem::is_regular_file(Filename.get());
//    }
// #endif

//    return found;
// }

// /**
//  * @brief Resets the file handle.
//  *
//  * @param Filename -- Name of file to open.
//  * @param Mode     -- Opening mode (read/write/append, etc.)
//  */
// bool ym::FileIO::reset(
//    str const Filename,
//    str const Mode) noexcept
// {
//    if (isOpen())
//    { // close current file
//       std::ignore = std::fclose(get());
//    }

//    _file = std::fopen(Filename, Mode);

//    if (_file)
//    { // file successfully opened
//       if (auto Size = calculateSize(); Size)
//       { // got size
//          _size = *Size;

//          switch (fcntl(fileno(_file.unwrap()), F_GETFL) & O_ACCMODE)
//          { // get file permissions
//             case O_RDONLY:
//             { // read permissions
//                _flags.set(AccessModeFlags_T::Read);
//                break;
//             }

//             case O_WRONLY:
//             { // write permissions
//                _flags.set(AccessModeFlags_T::Write);
//                break;
//             }

//             case O_RDWR:
//             { // read/write permissions
//                _flags.set(AccessModeFlags_T::Read);
//                _flags.set(AccessModeFlags_T::Write);
//                break;
//             }

//             default:
//             { // error
//                ymLog(VF::Warning, "Could not get permissions for file {}", Filename);
//                std::ignore = std::fclose(get());
//             }
//          }
//       }
//       else
//       { // error getting size
//          ymLog(VF::Warning, "Could not get size for file {}", Filename);
//          std::ignore = std::fclose(get());
//       }
//    }
//    else
//    { // file not opened
//       ymLog(VF::Warning, "Could not open file {}", Filename);
//    }

//    return isOpen();
// }

// /**
//  * @brief Gets the size of the current file.
//  *
//  * @returns std::size_t -- Size of file.
//  */
// std::size_t ym::FileIO::getSize(void) noexcept
// {
//    if (_flags.test(AccessModeFlags_T::Write))
//    { // size can change - recalculate
//       if (auto const Size = calculateSize(); Size)
//       { //
//          _size = *Size;
//       }
//       else
//       {
//          ymLog(VF::Warning, "Could not get size of file");
//       }
//    }

//    // TODO
//    // we can pass in a stackbuffer that stores the filename. file handles are often
//    // only opened in one scope, so just create a stackbuffer in that scope as the name.

//    return _size;
// }

// /** TODO we can probably absorb this into just getSize()
//  *
//  * @brief Gets the size of the current file.
//  *
//  * @returns std::optional<std::size_t> -- Size of file or nullopt if an error occurred.
//  */
// std::optional<std::size_t> ym::FileIO::calculateSize(void) const noexcept
// {
//    std::optional<std::size_t> size{std::nullopt};

//    if (_file)
//    { // valid file
//       struct stat st;
//       if (fstat(fileno(_file.unwrap()), &st) == 0)
//       { // got file size
//          size = static_cast<std::size_t>(st.st_size);
//       }
//    }

//    return size;
// }

// /**
//  * @brief Reads in file contents into an std::string.
//  *
//  * @returns std::optional<std::string> -- File contents, or null if an error occured.
//  */
// #if (YM_USE_HEAP_AS_FALLBACK)
//    std::optional<std::string> ym::FileIO::createAndFillBuffer(void) noexcept
//    {
//       std::optional<std::string> buffer{std::nullopt};

//       if (isOpen())
//       { // file opened
//          std::string contents;
//          contents.resize_and_overwrite(getSize(), [this](char * const buf_Ptr, std::size_t const N) {
//             return std::fread(buf_Ptr, 1uz, N, this->get());
//          });
//          buffer = std::move(contents);
//          std::rewind(get());
//       }

//       return buffer;
//    }
// #endif

// /**
//  * @brief Reads entire file contents into a supplied buffer.
//  *
//  * @note The size of the file is already calculated and query-able. If and only if
//  *       this function returns true has all the data been read.
//  *
//  * @param buffer     -- Buffer to read file into.
//  * @param AppendNull -- Whether or not to apply null terminator.
//  *
//  * @returns bool -- True if file was read and copied successfully, false if an error occured.
//  */
// bool ym::FileIO::fillBuffer(
//    std::span<char> buffer,
//    bool const      AppendNull) noexcept
// {
//    auto success = false;

//    if (isOpen())
//    { // file opened
//       if (buffer.size() >= getSize())
//       { // tentatively enough room to store data
//          auto const NRead = std::fread(buffer.data(), 1uz, buffer.size(), get());
//          success = (NRead == getSize());

//          if (AppendNull)
//          { // apply null terminator
//             if (buffer.size() > getSize())
//             { // we have room
//                buffer[NRead] = '\0';
//                fmt::println("-->buffer_1_place ({})({})({})<--", buffer.data(), buffer.size(), getSize());
//             }
//             else
//             { // not enough room afterall
//                success = false;
//             }
//          }

//          std::rewind(get());
//       }
//    }

//    return success;
// }

// /**
//  * @brief Reads in file contents incrementally into a supplied buffer.
//  *
//  * @note Since the user has the file size available to them, it is the user's responsibility
//  *       to track how much file data has bbeen read thus far. And return value of true means
//  *       that data has been read, a return value of false means that either EOF or an error
//  *       has occured.
//  *
//  * @param buffer -- Buffer to read file into.
//  *
//  * @returns std::optional<std::span<char>> -- If true, all available or requested amount of data was read.
//  *                                            If false, EOF or an error occured.
//  */
// std::optional<std::span<char>> ym::FileIO::fillBufferPiecewise(std::span<char> buffer) noexcept
// {
//    std::optional<std::span<char>> data{std::nullopt};

//    if (isOpen())
//    { // file opened
//       auto const NRead = std::fread(buffer.data(), 1uz, buffer.size(), get());
//       if (NRead > 0uz)
//       { // at least some data was available
//          data = buffer.subspan(0uz, NRead);
//       }
//    }

//    return data;
// }
