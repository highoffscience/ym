/**
 * @author Forrest Jablonski
 */

#include "ymception.h"

std::atomic<ym::uint32> ym::Ymception::_s_tagCount = 0u;

/**
 *
 */
ym::Ymception::Ymception(str const Msg)
   : std::exception {Msg                                                },
     _Tag           {_s_tagCount.fetch_add(1, std::memory_order_relaxed)}
{
}
