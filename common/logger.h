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
   YM_NO_COPY  (Logger);
   YM_NO_ASSIGN(Logger);

protected:
   explicit Logger(void);

   bool openOutfile(str const Filename);

   using FileDeleter_T = void(*)(std::FILE * const);
   std::unique_ptr<std::FILE, FileDeleter_T> _outfile_uptr;
};

} // ym
