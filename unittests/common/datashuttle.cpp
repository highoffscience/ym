/**
 * @file    datashuttle.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 * 
 * @note File used in unittests - maximum standard C++20.
 */

#include "datashuttle.h"

#include <initializer_list>
#include <utility>

/** DataShuttle
 * 
 * @brief Constructor.
 * 
 * @param data_uref -- List of elements to populate data shuttle with.
 */
ym::ut::DataShuttle::DataShuttle(std::initializer_list<Data_T::value_type> && data_uref)
  : _data {std::move(data_uref)}
{}
