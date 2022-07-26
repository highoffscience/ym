/**
 * @author Forrest Jablonski
 */

#include "logger.h"

/**
 *
 */
ym::Logger::Logger(void)
   : _outfile_ptr {nullptr}
{
}

/**
 *
 */
ym::Logger::~Logger(void)
{
   closeOutfile();
}

/**
 *
 */
bool ym::Logger::openOutfile(str const Filename)
{
   if (!_outfile_ptr)
   {
      _outfile_ptr = std::fopen(Filename, "w");
   }

   return _outfile_ptr != nullptr;
}

/**
 * TODO make this private and have the form of a custom deleter for unique_ptr,
 *  since going out of scope will trigger the closing of the file we dont need
 *  this function in its current form anyway...just call outfile.reset()
 */
void ym::Logger::closeOutfile(void)
{
   std::fclose(_outfile_ptr);
}
