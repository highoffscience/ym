/**
 * @file    datashuttle.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "utdefs.h"

#include <any>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace ym::ut
{

/** DataShuttle
 * 
 * @brief A dictionary to aid in communicating data between the server (source code) and
 *        the client (python script).
 */
class DataShuttle
{
public:
   using Data_T = std::unordered_map<std::string, std::any>;

   /*implicit*/ DataShuttle(void) = default;
   /*implicit*/ DataShuttle(std::initializer_list<Data_T::value_type> && data_uref);

   inline auto * operator -> (void) { return &_data; }

   // TODO can these be const?
   template <typename T> T get(std::string const & Name);
   template <typename T> T get(std::string const & Name,
                               T           const   DefaultValue);

private:
   Data_T _data;
};

/** get
 * 
 * @brief Returns value of named variable.
 * 
 * @throws std::out_of_range -- If variable doesn't exist.
 * @throws std::bad_any_cast -- If casting variable to desired type fails.
 * 
 * @tparam T -- Type to cast named variable to.
 * 
 * @param Name -- Name of variable.
 * 
 * @returns Value of named variable as type T.
 */
template <typename T>
T DataShuttle::get(std::string const & Name)
{
   return std::any_cast<T>(_data.at(Name));
}

/** get
 * 
 * @brief Returns value of named variable. If the variable doesn't exists return the
 *        specified default value.
 * 
 * @throws std::bad_any_cast -- If casting variable to desired type fails.
 * 
 * @tparam T -- Type to cast named variable to.
 * 
 * @param Name         -- Name of variable.
 * @param DefaultValue -- Value to return if named variable cannot be found.
 * 
 * @returns Value of named variable as type T.
 */
template <typename T>
T DataShuttle::get(std::string const & Name,
                   T           const   DefaultValue)
{
   auto val = DefaultValue;

   try
   { // try to retrieve and cast variable
      val = get<T>(Name);
   }
   catch(std::out_of_range const & E)
   { // variable not found
      // do nothing - val already set to default
   }

   return val;
}

} // ym::ut
