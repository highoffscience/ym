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
   : _writer        {/*default*/                                      },
     _buffer_Ptr    {MemoryPool<char>::allocate(getBufferSize_bytes())},
     _timer         {/*default*/                                      },
     _availableSem  {static_cast<int64>(getMaxNMessagesInBuffer())    },
     _messagesSem   {0                                                },
     _readPos       {0u                                               },
     _writePos      {0u                                               },
     _verbosityCap  {0u                                               },
     _writerMode    {WriterMode_T::Closed                             },
     _TimeStampMode {TimeStampMode                                    }
{
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
bool::ym::TextLogger::open(void)
{
   char globalName[6 + // global
                   1 + // _
                   4 + // year
                   1 + // _
                   3 + // month
                   1 + // _
                   2 + // day
                   1 + // _
                   2 + // hour
                   1 + // _
                   2 + // minute
                   1 + // _
                   2 + // second
                   4 + // .txt
                   1 ] /* null */ = {'\0'};

   auto t = std::time(nullptr);
   std::tm timeinfo = {0};
   auto * timeinfo_ptr = &timeinfo;
#if defined(_WIN32)
   localtime_s(timeinfo_ptr, &t); // vs doesn't have the standard localtime_s function
#else
   timeinfo_ptr = std::localtime(&t);
#endif // _WIN32

   auto const NBytesWritten =
      std::strftime(       globalName,
                    sizeof(globalName),
                    "global_%Y_%b_%d_%H_%M_%S.txt",
                    timeinfo_ptr);

   bool wasOpened = false;

   if (NBytesWritten > 0ul)
   {
      wasOpened = open(globalName);
   }
   else
   {
      printfError("Failure to store datetime!");
      wasOpened = false;
   }

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
auto ym::TextLogger::getVerbosityCap(void) const -> uint32
{
   return _verbosityCap.load(std::memory_order_relaxed);
}

/**
 *
 */
void ym::TextLogger::setVerbosityCap(uint32 const VerbosityCap)
{
   _verbosityCap.store(VerbosityCap, std::memory_order_relaxed);
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
      catch (std::exception const & Exc)
      {
         printfError("Logger write failed. Exc = '%s'\n", Exc.what());
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
