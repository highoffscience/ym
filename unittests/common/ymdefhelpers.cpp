/**
 * @file    ymdefs.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"

#include "../../common/globallogger.h"

/** ym_unit_cleanup_GlobalLogger
 *
 * @brief Hook to let unittests close the logger, otherwise the testsuite will hang waiting
 *        for the global logger to close, which is waiting for the testsuite to close... so
 *        this is how we break the deadlock.
 */
void ym_unit_cleanup_GlobalLogger(void)
{
   ym::GlobalLogger::getGlobalInstance()->close();
}
