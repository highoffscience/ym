/**
 * @author Forrest Jablonski
 */

#include "global_text_logger.h"

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
