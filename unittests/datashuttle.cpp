/**
 * @file    datashuttle.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "datashuttle.h"

#include <utility>

/** DataShuttle
 * 
 * @brief Constructor.
 */
ut::DataShuttle::DataShuttle(std::initializer_list<Data_T::value_type> && data_uref)
  : _data {std::move(data_uref)}
{
}
