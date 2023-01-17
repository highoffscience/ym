/**
 * @file    textlogger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "textlogger.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <memory>

/** TextLogger
 *
 * @brief Constructor.
 */
ym::TextLogger::TextLogger(void)
   : TextLogger(TextLogger::TimeStampMode_T::RecordTimeStamp)
{
}

/** TextLogger
 *
 * @brief Constructor.
 */
ym::TextLogger::TextLogger(TimeStampMode_T const TimeStampMode)
   : _buffer        {'\0'                     },
     _vGroups       {/*default*/              },
     _writer        {/*default*/              },
     _timer         {/*default*/              },
     _availableSem  {getMaxNMessagesInBuffer()},
     _messagesSem   {0_i32                    },
     _readPos       {0_u32                    },
     _writePos      {0_u32                    },
     _writerMode    {WriterMode_T::Closed     },
     _TimeStampMode {TimeStampMode            }
{
}

/** ~TextLogger
 *
 * @brief Destructor.
 */
ym::TextLogger::~TextLogger(void)
{
   close();
}

/** isOpen
 *
 * @brief Returns if outfile is open and able to be written to.
 *
 * @return True if outfile is open and able to be written to, false otherwise.
 */
bool ym::TextLogger::isOpen(void) const
{
   return _writerMode.load(std::memory_order_relaxed) == WriterMode_T::Open;
}

/** getGlobalInstance
 *
 * @brief Gets the global text logger instance.
 *
 * @note Used as the global logger for the program. Not expected to close until the end.
 */
auto ym::TextLogger::getGlobalInstance(void) -> TextLogger *
{
   static TextLogger s_instance(TimeStampMode_T::RecordTimeStamp);

   if (!s_instance.isOpen())
   {
      auto const Opened = s_instance.open("global.txt", TimeStampFilenameMode_T::Append);

      ymAssert<TextLoggerError_GlobalFailureToOpen>(Opened,
         "Global text logger instance failed to open");
   }

   return &s_instance;
}

/** open
 *
 * @brief Opens and prepares the logger to be written to.
 * 
 * @note Defaults to appending time stamp.
 *
 * @param Filename -- Name of outfile to open.
 *
 * @return bool -- Whether the outfile was opened successfully, false otherwise.
 */
bool ym::TextLogger::open(str const Filename)
{
   return open(Filename, TimeStampFilenameMode_T::Append);
}

/** open
 *
 * @brief Opens and prepares the logger to be written to.
 *
 * @param Filename       -- Name of outfile to open.
 * @param TSFilenameMode -- Mode whether to append time stamps to filename.
 *
 * @return bool -- Whether the outfile was opened successfully, false otherwise.
 */
bool ym::TextLogger::open(str    const Filename,
                          TSFM_T const TSFilenameMode)
{
   bool opened = openOutfile(Filename, TSFilenameMode);

   if (opened)
   { // file opened successfully
      _writer     = std::thread(&TextLogger::writeMessagesToFile, this); // starts the thread
      _writerMode = WriterMode_T::Open;
   }

   return opened;
}

/** close
 *
 * @brief Closes the outfile and shuts the logger down.
 */
void ym::TextLogger::close(void)
{
   auto       expected = WriterMode_T::Open;
   auto const Desired  = WriterMode_T::PreparingToClose;

   if (_writerMode.compare_exchange_strong(expected, Desired,
                                           std::memory_order_acquire,  // success
                                           std::memory_order_relaxed)) // failure
   { // file open

      close_Helper("File closing...");
      _writerMode.store(WriterMode_T::Closing, std::memory_order_release);
      _writer.join(); // wait until all messages have been written before closing the file

      closeOutfile();

      _writerMode.store(WriterMode_T::Closed, std::memory_order_relaxed);
   }
}

/** close_Helper
 * 
 * @brief Wrapper (helper) function to call printf_Producer().
 * 
 * @param msg -- Message to print.
 * @param ... -- Arguments.
 */
void ym::TextLogger::close_Helper(str          msg,
                                  /*variadic*/ ...)
{
   std::va_list args;
   va_start(args, msg);
   printf_Producer(msg, args); // this call is necessary to wake the semaphore and check the closing flag
   va_end(args);
}

/** enable
 *
 * @brief Enables specified verbosity group.
 *
 * @param VG -- Verbosity group to enable.
 */
void ym::TextLogger::enable(VGM_T const VG)
{
   using VGM = VerbosityGroupMask;

   _vGroups[VGM::getGroup(VG)] |= VGM::getMask_asByte(VG);
}

/** disable
 *
 * @brief Disables specified verbosity group.
 *
 * @param VG -- Verbosity group to disable.
 */
void ym::TextLogger::disable(VGM_T const VG)
{
   using VGM = VerbosityGroupMask;

   _vGroups[VGM::getGroup(VG)] &= ~VGM::getMask_asByte(VG);
}

/** printf_Handler
 *
 * @brief Conditionally prints the requested message.
 *
 * @note printf_Producer is not marked noexcept because terminate should not be called.
 *
 * @param VG     -- Verbosity group.
 * @param Format -- Format string.
 * @param ...    -- Arguments.
 */
void ym::TextLogger::printf_Handler(VGM_T const  VG,
                                    str   const  Format,
                                    /*variadic*/ ...)
{
   if (isOpen())
   { // ok to print

      using VGM = VerbosityGroupMask;
      auto const IsEnabled = (_vGroups[VGM::getGroup(VG)] & VGM::getMask_asByte(VG)) > 0_u8;

      if (IsEnabled)
      { // verbose enough to print this message

         std::va_list args;
         va_start(args, Format);
         printf_Producer(Format, args); // doesn't throw so va_end will be called
         va_end(args);
      }
   }
}

/** printf_Producer
 *
 * @brief Prints the requested message to the internal buffer.
 *
 * @note The system call to get the timestamp is usually optimized at runtime.
 *
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
void ym::TextLogger::printf_Producer(str const    Format,
                                     std::va_list args)
{
   _availableSem.acquire();

   // _writePos doesn't need to wrap, so just incrementing until it rolls over is ok
   auto const WritePos         = _writePos.fetch_add(1_u32, std::memory_order_acquire) % getMaxNMessagesInBuffer();
   auto *     write_ptr        = _buffer + (WritePos * getMaxMessageSize_bytes());
   auto       maxMsgSize_bytes = getMaxMessageSize_bytes();

   if (_TimeStampMode == TimeStampMode_T::RecordTimeStamp)
   { // populate time stamp
      auto const TimeStampSize_bytes = populateFormattedTime(write_ptr);

      write_ptr        += TimeStampSize_bytes;
      maxMsgSize_bytes -= TimeStampSize_bytes;
   }

   // vsnprintf writes a null terminator for us
   auto const NCharsWrittenInTheory = std::vsnprintf(write_ptr, maxMsgSize_bytes, Format, args);

   if (NCharsWrittenInTheory < 0)
   { // snprintf hit an internal error
      printfInternalError("std::vsnprintf failed with error code %d! "
                          "Message was '%s'\n",
                          NCharsWrittenInTheory,
                          write_ptr);

      std::strncpy(write_ptr, "error in printf (internal vsnprintf error)", maxMsgSize_bytes);

      // don't fail here - just keep going
   }
   else if (static_cast<uint32>(NCharsWrittenInTheory) >= maxMsgSize_bytes)
   { // not everything was printed to the buffer
      printfInternalError("Failed to write everything to the buffer! NCharsWrittenInTheory = %ld. "
                          "Msg size = %lu bytes. Message = '%s'\n",
                          NCharsWrittenInTheory,
                          maxMsgSize_bytes,
                          write_ptr);

      if (_TimeStampMode == TimeStampMode_T::RecordTimeStamp)
      { // do some cleanup in record mode
         if (maxMsgSize_bytes >= 2_u32)
         { // place trailing newline since space allows
            write_ptr[maxMsgSize_bytes - 2_u32] = '\n';
            // last index already null
         }
      }

      // don't fail here - just keep going
   }
   else if (_TimeStampMode == TimeStampMode_T::RecordTimeStamp)
   { // do some cleanup in record mode
      write_ptr[NCharsWrittenInTheory    ] = '\n';
      write_ptr[NCharsWrittenInTheory + 1] = '\0';
   }

   TODO // I think I need to reintroduce the bitfield of completed messages
        // -----------------
        // | 0 | 1 | 0 | 0 |
        // -----------------
        // In the above bitfield message #2 (index 1) is ready, we need to convey this to the consumer thread
   _readPos.store(WritePos, std::memory_order_release);

   std::fprintf(stdout, "--> TODO A <%s> <%u> <%u>\n", write_ptr, WritePos, _readPos.load());

   _messagesSem.release();
}

/** writeMessagesToFile
 *
 * @brief Writes all pending messages in the buffer to the outfile.
 *
 * @note This is the consumer thread.
 */
void ym::TextLogger::writeMessagesToFile(void)
{
   bool writerEnabled = true;

   while (writerEnabled)
   { // wait for messages to print while logger is still active

      std::fprintf(stdout, "--> TODO B\n");

      _messagesSem.acquire(); // _readPos has been updated

      auto const         ReadPos  = _readPos.load(std::memory_order_acquire) % getMaxNMessagesInBuffer();
      auto const * const Read_Ptr = _buffer + (ReadPos * getMaxMessageSize_bytes());

      std::fprintf(stdout, "--> TODO C <%s> <%u>\n", Read_Ptr, ReadPos);

      auto const MsgSize_bytes         = std::strlen(Read_Ptr);
      auto const NCharsWrittenInTheory = std::fwrite(Read_Ptr,
                                                     sizeof(*Read_Ptr),
                                                     MsgSize_bytes,
                                                     _outfile_uptr.get());

      if (NCharsWrittenInTheory < MsgSize_bytes)
      { // fwrite hit an internal error
         printfInternalError("std::fprintf failed with error code %d! "
                             "Message was '%s'\n",
                             NCharsWrittenInTheory,
                             Read_Ptr);

         // don't fail here - just keep going
      }

   #if defined(YM_PRINT_TO_SCREEN)
      std::fprintf(stdout, "%s", Read_Ptr);
   #endif // YM_PRINT_TO_SCREEN

      _availableSem.release();

      if (_writerMode.load(std::memory_order_relaxed) == WriterMode_T::Closing)
      { // close requested
         if (_readPos .load(std::memory_order_acquire) ==
             _writePos.load(std::memory_order_relaxed))
         { // no more messages in buffer
            writerEnabled = false;
         }
      }
   }
}

/** populateFormattedTime
 *
 * @brief Writes the elapsed time in the specified buffer.
 *
 * @note Not to be confused with @link Logger::populateTimeStamp @endlink.
 *
 * @note Returns the current time, in microseconds, since the creation of the log in the format
 *       (xxxxxxxxxxxx) xxx:xx:xx.xxx'xxx
 *
 * @param write_Ptr -- Buffer to write time stamp into.
 *
 * @return uint64 -- Size of written timestamp in bytes.
 */
auto ym::TextLogger::populateFormattedTime(char * const write_Ptr) const -> uint64
{
   auto const ElapsedTime_us  = _timer.getElapsedTime<std::micro>();
   auto const ElapsedTime_sec = (ElapsedTime_us /  1'000'000ll      ) % 60;
   auto const ElapsedTime_min = (ElapsedTime_us / (1'000'000ll * 60)) % 60;
   auto const ElapsedTime_hrs =  ElapsedTime_us / (1'000'000ll * 3'600);

   write_Ptr[ 0] = '(';
   write_Ptr[ 1] = ((ElapsedTime_us / 100'000'000'000ll) % 10) + '0';
   write_Ptr[ 2] = ((ElapsedTime_us /  10'000'000'000ll) % 10) + '0';
   write_Ptr[ 3] = ((ElapsedTime_us /   1'000'000'000ll) % 10) + '0';
   write_Ptr[ 4] = ((ElapsedTime_us /     100'000'000ll) % 10) + '0';
   write_Ptr[ 5] = ((ElapsedTime_us /      10'000'000ll) % 10) + '0';
   write_Ptr[ 6] = ((ElapsedTime_us /       1'000'000ll) % 10) + '0';
   write_Ptr[ 7] = ((ElapsedTime_us /         100'000ll) % 10) + '0';
   write_Ptr[ 8] = ((ElapsedTime_us /          10'000ll) % 10) + '0';
   write_Ptr[ 9] = ((ElapsedTime_us /           1'000ll) % 10) + '0';
   write_Ptr[10] = ((ElapsedTime_us /             100ll) % 10) + '0';
   write_Ptr[11] = ((ElapsedTime_us /              10ll) % 10) + '0';
   write_Ptr[12] = ((ElapsedTime_us /               1ll) % 10) + '0';
   write_Ptr[13] = ')';
   write_Ptr[14] = ' ';
   write_Ptr[15] = ((ElapsedTime_hrs / 100) % 10) + '0';
   write_Ptr[16] = ((ElapsedTime_hrs /  10) % 10) + '0';
   write_Ptr[17] = ((ElapsedTime_hrs /   1) % 10) + '0';
   write_Ptr[18] = ':';
   write_Ptr[19] = ((ElapsedTime_min / 10) % 10) + '0';
   write_Ptr[20] = ((ElapsedTime_min /  1) % 10) + '0';
   write_Ptr[21] = ':';
   write_Ptr[22] = ((ElapsedTime_sec / 10) % 10) + '0';
   write_Ptr[23] = ((ElapsedTime_sec /  1) % 10) + '0';
   write_Ptr[24] = '.';
   write_Ptr[25] = write_Ptr[ 7];
   write_Ptr[26] = write_Ptr[ 8];
   write_Ptr[27] = write_Ptr[ 9];
   write_Ptr[28] = '\'';
   write_Ptr[29] = write_Ptr[10];
   write_Ptr[30] = write_Ptr[11];
   write_Ptr[31] = write_Ptr[12];
   write_Ptr[32] = ':';
   write_Ptr[33] = ' ';

   constexpr auto TimeStampSize_bytes = 34_u64;

   static_assert(getMaxMessageSize_bytes() > TimeStampSize_bytes,
                 "No room for time stamp plus newline");

   return TimeStampSize_bytes;
}
