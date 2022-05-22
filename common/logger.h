/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <fstream>

namespace ym
{

/**
 *
 */
class Logger
{
public:
   explicit Logger(void);
   virtual ~Logger(void) = default;

   virtual bool isOpen(void) const;

   YM_NO_COPY  (Logger);
   YM_NO_ASSIGN(Logger);

protected:
   virtual bool open(str const Filename);
   virtual void close(void);

   std::ofstream _outfile;
};

} // ym
