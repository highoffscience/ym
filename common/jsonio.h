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

   void parseInSitu(void);

private:
   std::span<char> _buffer{};
};

} // ym
