/**
 * @file    nameable.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

namespace ym
{

/*
 * Available variants
 */
class Nameable_NV;
class PermaNameable_NV;

/** Nameable_NV
 * 
 * @brief Provides a mutably named object.
 */
class Nameable_NV
{
public:
   /** Nameable_NV
    * 
    * @brief Constructor.
    * 
    * @param Name_Ptr -- Initial name.
    */
   explicit inline Nameable_NV(str const Name_Ptr)
      : _Name_ptr {Name_Ptr}
   { }

   /** getName
    *
    *  @brief Returns the current name.
    * 
    * @returns auto -- Current name.
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

/** PermaNameable_NV
 * 
 * @brief Provides a permanently named object.
 */
class PermaNameable_NV
{
public:
   /** PermaNameable_NV
    * 
    * @brief Constructor.
    * 
    * @param Name_Ptr -- Name.
    */
   explicit constexpr PermaNameable_NV(str const Name_Ptr)
      : _Name_Ptr {Name_Ptr}
   { }

   /** getName
    *
    *  @brief Returns the name.
    * 
    * @returns auto -- Name.
    */
   constexpr auto getName(void) const { return _Name_Ptr; }

private:
   str const _Name_Ptr;
};

} // ym
