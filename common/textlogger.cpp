/**
 * @author Forrest Jablonski
 */

#include "TextLogger.h"

#include "TimerDefs.h"
#include "Ymception.h"

#include <ctime>

#define YM_PRINT_TO_SCREEN

/**
 * Semaphore                 _availableSem;
   Semaphore                 _messagesSem;
   std::thread               _writer;
   std::atomic<MsgReadyBF_T> _msgReady_bf;
   char * const              _buffer_Ptr;
   uint32                    _readPos;  // these positions are slot numbers [0.._s_MaxNMessagesInBuffer)
   std::atomic<uint32>       _writePos; //  :
   uint32                    _verbosityCap;
   TimeStampMode_T const     _TimeStampMode;
 */
ym::TextLogger::TextLogger(TimeStampMode_T const TimeStampMode)
   : Logger             {                                             },
     _outfile_ptr       {nullptr                                      },
     _readPos           {0                                            },
     _writePos          {0                                            },
     _msgReady_bf       {0                                            },
     _availableSem      {static_cast<int32>(getMaxNMessagesInBuffer())},
     _messagesSem       {0                                            },
     _writer            {                                             },
     _isOpen            {false                                        },
     _activeVerbosities {0                                            },
     _openingGuard      {                                             },
     _isGlobalInstance  {IsGlobalInstance                             },
     _buffer            {'\0'                                         }
{
}

/**
 * 
 */
ym::TextLogger::TextLogger(SStr_T const Filename)
   : TextLogger {Filename, false}
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
bool ym::TextLogger::open(void)
{
   std::scoped_lock const Lock(_openingGuard);

   auto const [outfile_Ptr, ErrorCode] = Logger::open(getName());

   if (ErrorCode == 0)
   { // file opened successfully
      _outfile_ptr = outfile_Ptr;
      _isOpen.store(true, std::memory_order_release);
      _writer = std::thread(&TextLogger::writeMessagesToFile, this); // starts the thread
   }
   else
   { // something went wrong
      printfError("Failed to open %s! Error code %d.\n", getName(), ErrorCode);
   }

   return _isOpen.load(std::memory_order_relaxed);
}

/**
 *
 */
bool ym::TextLogger::close(void)
{
   bool didCloseSuccessfully = false;

   bool       expected = true;
   bool const Desired  = false;

   if (_isOpen.compare_exchange_strong(expected, Desired, std::memory_order_relaxed))
   { // file open

      // this call is necessary to wake the semaphore and check the closing flag
      printf_Helper("File %s closing...", getName());

      _writer.join(); // wait until all messages have been written before closing the file

      auto const ErrorCode = Logger::close(_outfile_ptr);
      didCloseSuccessfully = (ErrorCode == 0);

      if (!didCloseSuccessfully)
      {
         // _outfile_ptr is still flushed and dissociated
         printfError("Failed to close %s! With error code %d!\n", getName(), ErrorCode);
      }

      _outfile_ptr = nullptr;
   }

   return didCloseSuccessfully;
}

/**
 *
 */
void ym::TextLogger::enableClients(uint64 const Clients_mask)
{
   ymAssert(Clients_mask >= 4, "Clients mask must be >= 4 (last 2 bits reserved for verbosity)!");

   _activeVerbosities |= Clients_mask;
}

/**
 *
 */
void ym::TextLogger::disableClients(uint64 const Clients_mask)
{
   ymAssert(Clients_mask >= 4, "Clients mask must be >= 4 (last 2 bits reserved for verbosity)!");

   _activeVerbosities &= ~Clients_mask;
}

/**
 *
 */
auto ym::TextLogger::getVerbosityCap(void) const -> uint64
{
   return _activeVerbosities.load(std::memory_order_relaxed) & 3ull;
}

/**
 *
 */
void ym::TextLogger::setVerbosityCap(uint64 const VerbosityCap)
{
   ymAssert(VerbosityCap <= 3, "Verbosity cap must be <= 3!");

   _activeVerbosities.fetch_and(~3ull,        std::memory_order_relaxed);
   _activeVerbosities.fetch_or (VerbosityCap, std::memory_order_relaxed);
}

/**
 * _verbosities has the form
 * -------------
 * |00|00|00|00|
 * -------------
 * Last 2 bits are the verbosity level.
 */
bool ym::TextLogger::willPrint(uint64 const Verbosities)
{
   auto const Clients_mask       = Verbosities & ~3ull;
   auto const Verbosity          = Verbosities &  3ull;
   auto const ActiveClients_mask = _activeVerbosities.load(std::memory_order_relaxed) & ~3ull;

   auto const IsActive           = (Clients_mask == 0) ||                   // global verbosity
                                   (Clients_mask & ActiveClients_mask) > 0; // group  verbosity

   return IsActive ? (Verbosity <= getVerbosityCap())
                   : false;
}

/**
 * TODO populateFormattedTime, we want to do it in this thread
 */
void ym::TextLogger::writeMessagesToFile(void)
{
   do
   { // wait for messages to print while logger is still active

      while ((_msgReady_bf.load(std::memory_order_acquire) & (1 << _readPos)) == 0)
      { // wait until THIS message is in the buffer

         // when we close an ending message will be printed, waking the semaphore,
         //  so no need to check for closing here, it can be safely done at the
         //  end of this function
         _messagesSem.acquire(); // waits until A message is in the buffer
      }

      auto const * const Read_Ptr = _buffer + (_readPos * getMaxMessageSize_bytes());

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

      _msgReady_bf.fetch_and(~(1 << _readPos), std::memory_order_relaxed);

      _readPos = (_readPos + 1) & (getMaxNMessagesInBuffer() - 1);

      _availableSem.release();

   } while (_isOpen     .load(std::memory_order_relaxed) ||     // still open for business?
            _msgReady_bf.load(std::memory_order_relaxed) != 0); // messages still in the buffer?
}

/**
 * Returns the current time, in microseconds, since the creation of the log in the format
 *  (xxxxxxxxxxxx) xxx:xx:xx.xxx'xxx
 */
auto ym::TextLogger::populateFormattedTime(char * const write_Ptr) const -> uint32
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
   
   // -1 to ignore null terminator
   return sizeof("(000000000000) 000:00:00.000'000: ") - 1;
}
