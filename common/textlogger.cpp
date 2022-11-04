/**
 * @file    textlogger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "textlogger.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <memory>

/** TextLogger
 * 
 * @brief Constructor.
 */
ym::TextLogger::TextLogger(void)
   : TextLogger(TextLogger::RecordTimeStamp)
{
}

/** TextLogger
 * 
 * @brief Constructor.
 */
ym::TextLogger::TextLogger(TimeStampMode_T const TimeStampMode)
   : _vGroups       {/*default*/                                  },
     _writer        {/*default*/                                  },
     _buffer        {'\0'                                         },
     _timer         {/*default*/                                  },
     _availableSem  {static_cast<int64>(getMaxNMessagesInBuffer())},
     _messagesSem   {0_i32                                        },
     _readPos       {0_u32                                        },
     _writePos      {0_u32                                        },
     _writerMode    {WriterMode_T::Closed                         },
     _TimeStampMode {TimeStampMode                                }
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

/** open
 * 
 * @brief Opens and prepares the logger to be written to.
 * 
 * @param Filename -- Name of outfile to open.
 * 
 * @return bool -- Whether the outfile was opened successfully, false otherwise.
 */
bool ym::TextLogger::open(str const Filename)
{
   bool opened = openOutfile(Filename);

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

      printf_Helper("File closing..."); // this call is necessary to wake the semaphore and check the closing flag
      _writerMode.store(WriterMode_T::Closing, std::memory_order_release);
      _writer.join(); // wait until all messages have been written before closing the file

      closeOutfile();

      _writerMode.store(WriterMode_T::Closed, std::memory_order_relaxed);
   }
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

/** printf
 *
 * Using a universal reference here would not be better (ie Args_T && ...), since
 * the arguments are going to be primitives.
 */
template <typename... Args_T>
void TextLogger::printf_Handler(uint32 const    VerbosityGroup,
                        str    const    Format,
                        Args_T const... Args)
{
   if (isOpen())
   { // ok to print
      bool isEnabled = false;

      {
         std::scoped_lock const Lock(_verbosityGroups);
         isEnabled = (_verbosityGroups.at(VGroupEnable.Slot) & VGroupEnable.Mask) > 0u;
      }

      if (isEnabled)
      { // verbose enough to print this message
         printf_Helper(Format, Args...);
      }
   }
}

/**
 *
 */
template <typename... Args_T>
void TextLogger::printf_Producer(str    const    Format,
                               Args_T const... Args)
{
   _availableSem.acquire();

   // _writePos doesn't need to wrap, so just incrementing until it rolls over is ok
   auto const WritePos         = _writePos.fetch_add(1u, std::memory_order_acquire) % getMaxNMessagesInBuffer();
   str        writePtr         = _buffer_Ptr + (WritePos * getMaxMessageSize_bytes());
   auto       maxMsgSize_bytes = getMaxMessageSize_bytes();

   if (_TimeStampMode == RecordTimeStamp)
   {
      writePtr += getTimeStampSize_bytes();

      // +1 to account for newline
      static_assert(getMaxMessageSize_bytes() > getTimeStampSize_bytes() + 1u,
         "No room for time stamp plus newline");
      maxMsgSize_bytes -= getTimeStampSize_bytes() + 1u;
   }

   // snprintf writes a null terminator for us
   auto const NCharsWrittenInTheory = std::snprintf(writePtr,
                                                    maxMsgSize_bytes,
                                                    Format,
                                                    Args...);

   if (NCharsWrittenInTheory < 0)
   { // snprintf hit an internal error
      printfError("std::snprintf failed with error code %d! "
                  "Message was '%s'\n",
                  NCharsWrittenInTheory,
                  writePtr);

      std::strncpy(writePtr, "error in printf (internal snprintf error)", getMaxMessageSize_bytes());

      // don't fail here - just keep going
   }
   else if (static_cast<uint32>(NCharsWrittenInTheory) >= maxMsgSize_bytes)
   { // not everything was printed to the buffer
      printfError("Failed to write everything to the buffer! NCharsWrittenInTheory = %ld. "
                  "Msg size = %lu bytes. Message = '%s'\n",
                  NCharsWrittenInTheory,
                  maxMsgSize_bytes,
                  writePtr);

      if (_TimeStampMode == RecordTimeStamp)
      {
         writePtr[maxMsgSize_bytes - 2u] = '\n';
         // last index already null
      }

      // don't fail here - just keep going
   }
   else if (_TimeStampMode == RecordTimeStamp)
   {
      writePtr[NCharsWrittenInTheory     ] = '\n';
      writePtr[NCharsWrittenInTheory + 1u] = '\0';
   }

   // _readPos doesn't need to wrap, so just incrementing until it rolls over is ok
   _readPos.fetch_add(1u, std::memory_order_release);

   _messagesSem.release();
}

/**
 *
 */
void ym::TextLogger::writeMessagesToFile(void)
{
   bool writerEnabled = true;

   while (writerEnabled)
   { // wait for messages to print while logger is still active

      _messagesSem.acquire(); // _readPos has been updated

      auto const ReadPos = _readPos.load(std::memory_order_acquire) % getMaxNMessagesInBuffer();
      auto * const read_Ptr = _buffer_Ptr + (ReadPos * getMaxMessageSize_bytes());

      if (_TimeStampMode == RecordTimeStamp)
      {
         populateFormattedTime(read_Ptr);
      }

      try
      {
         _outfile << read_Ptr;
      }
      catch (std::exception const & E)
      {
         printfError("Logger write failed. Exc = '%s'\n", E.what());
      }

   #if defined(YM_PRINT_TO_SCREEN)
      std::fprintf(stdout, read_Ptr);
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

/**
 * Returns the current time, in microseconds, since the creation of the log in the format
 *  (xxxxxxxxxxxx) xxx:xx:xx.xxx'xxx
 */
char * ym::TextLogger::populateFormattedTime(char * const write_Ptr) const
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

   return write_Ptr + 34_u64;
}
