/**
 * @file    keyable.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "../common/ymdefs.h"

#include <type_traits>

namespace ym::hsm
{

// TODO make a permakeyable variant (similar to nameable)
// TODO rename concepts from "Topicable" to "TopicConcept"
// make unit tests for header only files too

/** Keyable_NV
 * 
 * @brief TODO
 * 
 * @note NV = Non-Virtual
 */
template <typename Key_T = uint64>
requires(std::is_unsigned_v<Key_T>)
class Keyable_NV
{
public:
   /**
    * TODO
    */
   explicit inline Keyable_NV(Key_T const Key)
      : _key {Key}
   { }

   /**
    * TODO
    */
   inline auto getKey(void) const { return _Key; }

private:
   Key_T const _Key;
};

} // ym::hsm
