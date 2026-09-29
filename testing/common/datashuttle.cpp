/**
 * @file    datashuttle.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note File used in unittests - maximum standard C++20.
 */

#include "datashuttle.h"

#include <utility>

/** DataShuttle
 *
 * @brief Constructor.
 *
 * @param data -- List of elements to populate data shuttle with.
 */
ym::unit::DataShuttle::DataShuttle(std::initializer_list<Data_T::value_type> && data) noexcept :
   _data {std::move(data)}
{ }
