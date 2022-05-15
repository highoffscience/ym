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
     _msgReady_bf   {0ul                                              },
     _buffer_Ptr    {MemoryPool<char>::allocate(getBufferSize_bytes())},
     _timer         {                                                 },
     _availableSem  {static_cast<int32>(getMaxNMessagesInBuffer())    },
     _messagesSem   {0                                                },
     _readPos       {0u                                               },
     _writePos      {0u                                               },
     _verbosityCap  {0u                                               },
     _TimeStampMode {TimeStampMode                                    },
     _writerEnabled {false                                            }
{
}

/**
 *
 */
ym::TextLogger::~TextLogger(void)
{
   close();
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
      _writerEnabled.wait(false); // writer thread officially started
      std::this_thread::yield();  // allow writer thread to acquire semaphore
   }

   return wasOpened;
}

/**
 * TODO
 */
void ym::TextLogger::close(void)
{
   bool       expected = true;
   bool const Desired = false;

   if (_writerEnabled.compare_exchange_strong(expected, Desired, std::memory_order_relaxed))
   { // file open
      printf_Helper("File closing..."); // this call is necessary to wake the semaphore and check the closing flag
      _writer.join(); // wait until all messages have been written before closing the file

      Logger::close();
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
   _writerEnabled.store(true);
   _writerEnabled.notify_one();

   bool writerEnabled = true;

   while (writerEnabled)
   { // wait for messages to print while logger is still active

      do
      {
         // when we close an ending message will be printed, waking the semaphore,
         //  so no need to check for closing here, it can be safely done at the
         //  end of this function
         _messagesSem.acquire(); // waits until *a* message is in the buffer

      } // wait until *this* message is in the buffer
      while (_msgReady_bf.load(std::memory_order_acquire) & (1ul << _readPos) == 0ul);

      _msgReady_bf.fetch_and(~(1ul << _readPos), std::memory_order_relaxed);

      auto * const read_Ptr = _buffer_Ptr + (_readPos * getMaxMessageSize_bytes());

      _readPos = (_readPos + 1) % getMaxNMessagesInBuffer();

      if (_TimeStampMode == RecordTimeStamp)
      {
         populateFormattedTime(read_Ptr);
      }

      try
      {
         _outfile.write(read_Ptr, getMaxMessageSize_bytes());
      }
      catch (std::exception const & Exc)
      {
         printfError("Logger write failed. Exc = '%s'\n", Exc.what());
      }

   #if defined(YM_PRINT_TO_SCREEN)
      std::fprintf(stdout, read_Ptr);
   #endif // YM_PRINT_TO_SCREEN

      if (!_writerEnabled.load(std::memory_order_relaxed))
      { // close requested
         if (_msgReady_bf.load(std::memory_order_relaxed) == 0ul)
         { // no more messages in buffer
            writerEnabled = false;
         }
      }

      _availableSem.release();
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
