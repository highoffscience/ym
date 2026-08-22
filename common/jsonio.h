/**
 * @file    jsonio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include "fileio.h"
#include "memio.h"

namespace ym
{

/**
 * TODO
 */
class JsonIO : public StackBufferUser
{
public:
   // this class takes ownership of the buffer
   explicit inline JsonIO(BoundPtr<StackBuffer_Base> const buffer_Ptr) noexcept :
      StackBufferUser(buffer_Ptr),
      _buffer_Ptr {buffer_Ptr}
   { }

   bool readFromFile(FileIO f) noexcept;

   void parseInSitu(void);

private:
   // TODO Maybe StackBuffer should actually be StackResource - it is a resource, not a buffer!
   BoundPtr<StackBuffer_Base> const _buffer_Ptr;
};

} // ym
