/**
 * @file    jsonio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <span>

namespace ym
{

struct LinkedCarrier
{
   LinkedCarrier(class JsonIO * primary_ptr) : _primary_ptr {primary_ptr} {}
   ~LinkedCarrier(void) {
      if (_primary_ptr) {
         _primary_ptr->destroyLinkedCarrier();
      }
   }

   class JsonIO *  _primary_ptr;
   std::span<char> _data;
};

/** JsonIO
 *
 * TODO
 */
class JsonIO
{
public:
   // this class takes ownership of the buffer
   explicit inline JsonIO(BoundPtr<LinkedCarrier> lc_bptr) noexcept
      : _lc_bptr {lc_bptr}
   { }
   ~JsonIO(void) {

   }

   void parseInSitu(void);

private:
   BoundPtr<LinkedCarrier> _lc_bptr; // This should be a free ptr - or std::optional
};

} // ym
