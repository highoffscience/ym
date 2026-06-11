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

/** JsonIO
 *
 * TODO
 */
class JsonIO
{
public:
   // this class takes ownership of the buffer
   explicit inline JsonIO(std::span<char> buffer) noexcept;

   void parseInSitu();

private:
   std::unique_ptr<char> _buffer_uptr{};
};

} // ym
