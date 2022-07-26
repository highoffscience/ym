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
   ~Logger(void);

   YM_NO_COPY  (Logger);
   YM_NO_ASSIGN(Logger);

protected:
   bool openOutfile(str const Filename);
   void closeOutfile(void);

   std::unique_ptr<FILE> _outfile_ptr;
};

} // ym
