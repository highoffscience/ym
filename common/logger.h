/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <cstdio>
#include <memory>

namespace ym
{

/**
 *
 */
class Logger
{
public:
   explicit Logger(void);

   YM_NO_COPY  (Logger);
   YM_NO_ASSIGN(Logger);

protected:
   bool openOutfile(str const Filename);

   std::unique_ptr<std::FILE, void(*)(std::FILE * const)> _outfile_uptr;
};

} // ym
