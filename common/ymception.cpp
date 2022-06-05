/**
 * @author Forrest Jablonski
 */

#include "Ymception.h"

std::atomic<ym::uint32> ym::Ymception::_s_tagCount = 0;

/**
 *
 */
ym::Ymception::Ymception(SStr_T const Msg)
   : std::exception {Msg                                                },
     _Tag           {_s_tagCount.fetch_add(1, std::memory_order_relaxed)}
{
}
