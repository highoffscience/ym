/**
 * @file    datashuttle.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "datashuttle.h"

#include <utility>

/**
 * @brief Constructor.
 *
 * @param data -- List of elements to populate data shuttle with.
 */
ym::intg::DataShuttle::DataShuttle(std::initializer_list<Data_T::value_type> && data) noexcept :
   _data {std::move(data)}
{ }
