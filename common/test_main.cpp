/**
 * @author Forrest Jablonski
 */

#include "lightlogger.h"

int main(void)
{
   using namespace ym;

   LightLogger::getGlobalInstance()->printf("ff");

   return 0;
}
