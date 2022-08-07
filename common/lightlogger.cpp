/**
 * @author Forrest Jablonski
 */

#include "lightlogger.h"

/**
 * TODO timestamp filename
 */
auto ym::LightLogger::getGlobalInstance(void) -> LightLogger *
{
   static LightLogger s_instance;

   s_instance.openOutfile("global_light.log");

   return &s_instance;
}
