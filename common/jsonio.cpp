/**
 * @file    jsonio.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "jsonio.h"

#include "globallogger.h"

/**
 * @brief TODO
 */
bool ym::JsonIO::readFromFile(FileIO f) noexcept
{
   auto success = false; // until told otherwise

   // if (f)
   // { // open file
   //    try
   //    { // allocate() can throw
   //       auto const Size_bytes = f.getSize() + 1uz; // +1 for null terminator
   //       auto const buf_Ptr = BoundPtr<char>(_buffer_Ptr->allocate(Size_bytes));
   //       f.fillBuffer({buf_Ptr, Size_bytes});
   //    }
   //    catch (std::exception const & E)
   //    { // exception

   //       // TODO is there a way to store the name of File?
   //       ymLog(VF::Warning, "JsonIO failed to allocate memory for file {}", f.getName());
   //    }
   // }

   return success;
}

/**
 * @brief TODO
 */
void ym::JsonIO::parseInSitu(void)
{

}
