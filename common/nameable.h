/**
 * @file    nameable.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

namespace ym
{

/*
 * Available variants
 */
class Named_NV;
class PermaNamed_NV;

/** Named_NV
 * 
 * @brief Provides a mutably named object.
 */
class Named_NV
{
public:
   /** Named_NV
    * 
    * @brief Constructor.
    * 
    * @param Name_Ptr -- Initial name.
    */
   explicit inline Named_NV(str const Name_Ptr)
      : _Name_ptr {Name_Ptr}
   { }

   /** getName
    *
    *  @brief Returns the current name.
    * 
    * @return auto -- Current name.
    */
   inline auto getName(void) const { return _Name_ptr; }

   /** setName
    * 
    * @brief Sets the current name.
    * 
    * @param Name_Ptr -- Desired name.
    */
   inline void setName(str const Name_Ptr) { _Name_ptr = Name_Ptr; }

private:
   str /* non-const */ _Name_ptr;
};

/** PermaNamed_NV
 * 
 * @brief Provides a permanently named object.
 */
class PermaNamed_NV
{
public:
   /** PermaNamed_NV
    * 
    * @brief Constructor.
    * 
    * @param Name_Ptr -- Name.
    */
   explicit constexpr PermaNamed_NV(str const Name_Ptr)
      : _Name_Ptr {Name_Ptr}
   { }

   /** getName
    *
    *  @brief Returns the name.
    * 
    * @return auto -- Name.
    */
   constexpr auto getName(void) const { return _Name_Ptr; }

private:
   str const _Name_Ptr;
};

} // ym
