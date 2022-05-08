/**
 * @author Forrest Jablonski
 */

#include "textlogger.h"

#include <ctime>

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
   : _buffer        {'\0'                     },
     _writer        {/*default*/              },
     _msgReady_bf   {0ul                      },
     _availableSem  {getMaxNMessagesInBuffer()},
     _messagesSem   {0                        },
     _readPos       {0u                       },
     _writePos      {0u                       },
     _verbosityCap  {0u                       },
     _TimeStampMode {TimeStampMode            },
     _writerStarted {/*default*/              } // defaults to cleared
{
}

/**
 * Follows the singleton pattern.
 */
auto ym::TextLogger::getGlobalInstancePtr(void) -> TextLogger *
{
   static char globalName[6 + // global
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

   static TextLogger s_globalInstance(globalName, true);

   if (!s_globalInstance.isOpen())
   {
      { // finish populating name
         auto t = std::time(nullptr);
         std::tm timeinfo = {0};
         localtime_s(&timeinfo, &t); // vs doesn't have the standard localtime_s function
         auto const NBytesWritten =
            std::strftime(       globalName,
                          sizeof(globalName),
                          "global_%Y_%b_%d_%H_%M_%S.txt",
                          &timeinfo);

         ymAssert(NBytesWritten > 0, "Failure to store datetime!");
      }

      auto const DidOpen = s_globalInstance.open();

      ymAssert(DidOpen, "%s could not open!", globalName);
   }

   return &s_globalInstance;
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
      _writerStarted.wait(false);
   }

   return wasOpened;
}

/**
 * TODO 
 */
void ym::TextLogger::close(void)
{
   if (_writerStarted.test())
   { // file open
      _writerStarted.clear();
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
 * TODO populateFormattedTime, we want to do it in this thread
 */
void ym::TextLogger::writeMessagesToFile(void)
{
   _writerStarted.test_and_set();
   _writerStarted.notify_one();

   do
   { // wait for messages to print while logger is still active

      while ((_msgReady_bf.load(std::memory_order_acquire) & (1ul << _readPos)) == 0ul)
      { // wait until *this* message is in the buffer

         // when we close an ending message will be printed, waking the semaphore,
         //  so no need to check for closing here, it can be safely done at the
         //  end of this function
         _messagesSem.acquire(); // waits until *a* message is in the buffer
      }

      auto const * const Read_Ptr = _buffer + (_readPos * getMaxMessageSize_bytes());

      // TODO record time stamp appends newline, otherwise not
      auto const NCharsWritten = std::fprintf(_outfile_ptr, "%s\n", Read_Ptr);

      if (NCharsWritten < 0)
      { // fprintf hit an internal error
         printfError("std::fprintf failed with error code %d!\n", NCharsWritten);
         printfError(" Message was \"%s\"\n", Read_Ptr);

         // don't fail here - just keep going
      }

   #if defined(YM_PRINT_TO_SCREEN)
      if (isGlobalInstance())
      {
         std::fprintf(stdout, "%s\n", Read_Ptr);
      }
   #endif // YM_PRINT_TO_SCREEN

      _msgReady_bf.fetch_and(~(1ul << _readPos), std::memory_order_release);

      _readPos = (_readPos + 1) % getMaxNMessagesInBuffer();

      _availableSem.release();

   } while (_writerStarted.test() ||                            // still open for business?
            _msgReady_bf.load(std::memory_order_release) != 0); // messages still in the buffer?
}

/**
 * Returns the current time, in microseconds, since the creation of the log in the format
 *  (xxxxxxxxxxxx) xxx:xx:xx.xxx'xxx
 */
void ym::TextLogger::populateFormattedTime(char * const write_Ptr) const
{
   auto const ElapsedTime_us  = ymGetGlobalTime<std::micro>();
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
