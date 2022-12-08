/**
 * @file    datashuttle.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

#include <any>
#include <initializer_list>
#include <string>
#include <unordered_map>

namespace ym::ut
{

/** DataShuttle
 * 
 * @brief TODO.
 */
class DataShuttle
{
public:
   using Data_T = std::unordered_map<std::string, std::any>;

   /*implicit*/ DataShuttle(std::initializer_list<Data_T::value_type> && data_uref);

   auto * operator -> (void) { return &_data; }

   template <typename T> T get(std::string const & Name);
   template <typename T> T get(std::string const & Name,
                               T           const   DefaultValue);

private:
   Data_T _data;
};

/** get
 * 
 * @brief TODO.
 */
template <typename T>
T DataShuttle::get(std::string const & Name)
{
   return std::any_cast<T>(_data.at(Name));
}

/** get
 * 
 * @brief TODO.
 */
template <typename T>
T DataShuttle::get(std::string const & Name,
                   T           const   DefaultValue)
{
   return (_data.count() > 0) ? get(Name) : DefaultValue;
}

} // ym::ut
