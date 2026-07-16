/**
 * @file    jsonio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include "memio.h"

namespace ym
{

/** JsonIO
 *
 * TODO
 */
class JsonIO : public StackBufferUser
{
public:
   // this class takes ownership of the buffer
   explicit inline JsonIO(BoundPtr<StackBuffer_Base> const buffer_BPtr) noexcept :
      StackBufferUser(buffer_BPtr),
      _buffer_BPtr {buffer_BPtr}
   { }

   void populateBuffer(/*some sort of stream*/);
   void parseInSitu(void);

private:
   BoundPtr<StackBuffer_Base> const _buffer_BPtr;
};

} // ym
