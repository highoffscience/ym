/**
 * @author Forrest Jablonski
 */

#include "textlogger.h"

#include "memorypool.h"

#include <ctime>
#include <exception>

// for debugging
#define YM_PRINT_TO_SCREEN

/**
 *
 */
ym::TextLogger::TextLogger(void)
   : TextLogger(TextLogger::RecordTimeStamp)
{
}

/**
 *
 */
ym::TextLogger::TextLogger(TimeStampMode_T const TimeStampMode)
   : _verbosityGuard  {/*default*/                                      },
     _verbosityGroups {/*default*/                                      },
     _writer          {/*default*/                                      },
     _buffer_Ptr      {MemoryPool<char>::allocate(getBufferSize_bytes())},
     _timer           {/*default*/                                      },
     _availableSem    {static_cast<int64>(getMaxNMessagesInBuffer())    },
     _messagesSem     {0                                                },
     _readPos         {0u                                               },
     _writePos        {0u                                               },
     _writerMode      {WriterMode_T::Closed                             },
     _TimeStampMode   {TimeStampMode                                    }
{
   _verbosityGroups.reserve(8u);
}

/**
 *
 */
ym::TextLogger::~TextLogger(void)
{
   close();

   MemoryPool<char>::deallocate(_buffer_Ptr, getBufferSize_bytes());
}

/**
 * 
 */
bool ym::TextLogger::isOpen(void) const
{
   return _writerMode.load(std::memory_order_relaxed) == WriterMode_T::Open;
}

/**
 * TODO make generic (I want to ba able to timestamp any filename)
 */
bool::ym::TextLogger::open_appendTimeStamp(str const Filename)
{
   auto const FilenameSize_bytes = std::strlen(Filename);
   auto const Ext = std::strchr(Filename, '.');
   auto const StemSize_bytes = (Ext) ? (Ext - Filename) : FilenameSize_bytes;

   auto const TimeStampSize_bytes =
      1u + // _
      4u + // year
      1u + // _
      3u + // month
      1u + // _
      2u + // day
      1u + // _
      2u + // hour
      1u + // _
      2u + // minute
      1u + // _
      2u;  // second

   auto const TimeStampedFilenameSize_bytes = FilenameSize_bytes + TimeStampSize_bytes + 1u; // +1 for null terminator
   auto * const timeStampedFilename_Ptr = MemoryPool<char>::allocate(TimeStampedFilenameSize_bytes); 

   std::strncpy(timeStampedFilename_Ptr, Filename, StemSize_bytes);
   std::strncpy(timeStampedFilename_Ptr + StemSize_bytes + TimeStampSize_bytes,
                Filename + StemSize_bytes,
                FilenameSize_bytes - StemSize_bytes);

   auto t = std::time(nullptr);
   std::tm timeinfo = {0};
   auto * timeinfo_ptr = &timeinfo;
#if defined(_WIN32)
   localtime_s(timeinfo_ptr, &t); // vs doesn't have the standard localtime_s function
#else
   timeinfo_ptr = std::localtime(&t);
#endif // _WIN32

   auto const NBytesWritten =
      std::strftime(timeStampedFilename_Ptr + StemSize_bytes,
                    TimeStampSize_bytes,
                    "_%Y_%b_%d_%H_%M_%S.txt",
                    timeinfo_ptr);

   bool wasOpened = false;

   if (NBytesWritten > 0ul)
   {
      wasOpened = open(timeStampedFilename_Ptr);
   }
   else
   {
      printfError("Failure to store datetime!");
      wasOpened = false;
   }

   MemoryPool<char>::deallocate(timeStampedFilename_Ptr, TimeStampedFilenameSize_bytes);

   return wasOpened;
}

/**
 *
 */
bool ym::TextLogger::open(str const Filename)
{
   bool wasOpened = Logger::open(Filename);

   if (wasOpened)
   { // file opened successfully
      _writer = std::thread(&TextLogger::writeMessagesToFile, this); // starts the thread
      _writerMode = WriterMode_T::Open;
   }

   return wasOpened;
}

/**
 *
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

      Logger::close();

      _writerMode.store(WriterMode_T::Closed, std::memory_order_relaxed);
   }
}

/**
 *
 */
auto ym::TextLogger::addNewVGroup(void) -> VGroup_T
{
   std::scoped_lock const Lock(_verbosityGuard);

   auto const Slot = _verbosityGroups.size();

   _verbosityGroups.emplace_back(0u);

   return Slot;
}

/**
 *
 */
void ym::TextLogger::setVGroupEnable(VGroup const VGroupEnable,
                                     bool   const Enable)
{
   try
   {
      if (Enable)
      {
         _verbosityGroups.at(VGroupEnable.Slot) |= VGroupEnable.Mask;
      }
      else
      {
         _verbosityGroups.at(VGroupEnable.Slot) &= ~VGroupEnable.Mask;
      }
   }
   catch (std::exception const & E)
   {
      // TODO throw ymception
   }
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
 * 
 * Note! If changing this function don't forget to update getTimeStampSize_bytes()
 */
void ym::TextLogger::populateFormattedTime(char * const write_Ptr) const
{
   auto const ElapsedTime_us  = _timer.getStartTime<std::micro>();
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
}
