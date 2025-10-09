/**
 * @file    verbogroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <bit>
#include <utility>

namespace ym
{

/** VerboGroup
 *
 * @brief Top-level group of registered objects (verbosity group).
 *
 * @note This grouping is a two-tiered mechanism to organize, to a minimal extent,
 *       groups of related functionality. The Group is used for the higher
 *       level groups and the Mask is used for the finer groups within
 *       those higher level groups. They occupy different enums to make switching
 *       groups of bits, not just individual bits, easier to implement.
 *
 * @note This is a wrapped scoped enum to prevent accidental bitwise or'ing with
 *       masks, as this enum represents groups of masks - it is not a mask itself.
 *
 * @note Keep in alphabetical order.
 * 
 * @note The size of a verbosity group doesn't really matter - any primitive will do.
 *       What matters is the size of the mask, ie how many bits the mask has. The
 *       minimum number of mask bits is one, so the group can be either on or off.
 *       A mask of at least 4 bits, if you have the space, is recommended because the
 *       concept of groups and their associated masks can be useful.
 * 
 * TODO use case
 *    VerboGroup vg{};
 *    vg.enable(VG::TextLogger_UnitTest);
 *    vg.disable(...);
 *    vg.isEnabled(...);
 * 
 * If in non-lite mode we have...
 *    enum T : unsigned {
 *       General,
 *       TextLogger,
 * 
 *       TextLogger_UnitTest
 *    };
 *    enum Masks : unsigned {
 *       Global = (General << getNMaskBits()) | 0b0001,
 *       Debug  = (General << getNMaskBits()) | 0b0010,
 *       Warn   = (General << getNMaskBits()) | 0b0100,
 *       Error  = (General << getNMaskBits()) | 0b1000,
 * 
 *       TextLogger =          (TextLogger << getNMaskBits()) | 0b1111,
 *       TextLogger_Basic    = (TextLogger << getNMaskBits()) | 0b0001,
 *       TextLogger_Detail   = (TextLogger << getNMaskBits()) | 0b0010
 *    };
 * 
 * enum UnitTestGroups_T : unsigned {
 *    TextLogger_UnitTest
 * };
 * 
 * For lite builds...
 *    enum T : unsigned {
 *       Global,
 *       Debug,
 *       Warning,
 *       Error,
 * 
 *       TextLogger,
 * 
 *    #if (YM_UNITTEST_ACTIVE)
 *       TextLogger_UnitTest
 *    #endif
 *    };
 */
struct VerboGroup
{
   /// @brief Gets the count of mask bits.
   static constexpr unsigned getNMaskBits(void)
   {
      constexpr auto NMaskBits =
         #if (YM_LITE)
            1u
         #else
            4u
         #endif
         ;
      static_assert(std::has_single_bit(NMaskBits), "Must be power of 2 (bit packing reasons)");
      return NMaskBits;
   }

   /// @brief Gets the count of group bits.
   static constexpr Base_T getNGroupBits(void)
   {
      constexpr auto NTotalBits = static_cast<Base_T>(ymGetNBits<Base_T>());
      static_assert(NTotalBits > getNMaskBits(), "Not enough room for group bits");
      return NTotalBits - getNMaskBits();
   }

#if (YM_LITE)
   #define YM_TMP_INIT_VGMASK(Group_, Mask_) = ((Groups_T::Group_ << getNMaskBits()) | Mask_)
#else
   #define YM_TMP_INIT_VGMASK(Group_, Mask_)
#endif

   enum class Groups_T : unsigned
   {
      General,
      TextLogger,

      NGroups
   };

   enum class GroupMasks_T : unsigned
   {
      Global YM_TMP_INIT_VGMASK(General, 0b0001u),
      Debug,
      Warning,
      Error,

      TextLogger,
   };

   

   /// @brief Verbosity groups.
   enum class T : Base_T
   {
   // #if (YM_LITE)
      Global,
      Debug,
      Warning,
      Error,

      ArgParser,       UnitTest_ArgParser,
      DataLogger,      UnitTest_DataLogger,
      FileIO,          UnitTest_FileIO,
      Logger,          UnitTest_Logger,
      TextLogger,      UnitTest_TextLogger,
      ThreadSafeProxy, UnitTest_ThreadSafeProxy,
      MemIO,           UnitTest_MemIO,
      Ops,             UnitTest_Ops,
      Rng,             UnitTest_Rng,
      Timer,           UnitTest_Timer,
      YmAssert,        UnitTest_YmAssert,
      YmDefs,          UnitTest_YmDefs,
      YmUtils,         UnitTest_YmUtils,
      
      NGroups
   };

   /// @brief Convenience method to get the # of verbosity groups.
   static constexpr auto getNGroups(void) { return std::to_underlying(T::NGroups); }
};

/** VerboGroupMask
 *
 * @brief Verbosity Group Mask definitions. Sub-groups represented as masks.
 *
 * @note We can have a maximum of 2^24 groups, and each group can contain a maximum of 8 flags.
 * @note These masks are a scoped enum so the user cannot accidentally bitwise-or them.
 * @note                      x = group
 *                            y = mask
 *       -------------------------------------------------
 *       | xxxx'xxxx | xxxx'xxxx | xxxx'xxxx | yyyy'yyyy |
 *       -------------------------------------------------
 * 
 * TODO
 *       -------------------------
 *       | xxxx'xxxx | xxxx'yyyy |
 *       -------------------------
 * 
 * TODO
 *       -------------
 *       | xxxx'xxxx |
 *       -------------
 *
 * @note Example use:
 *
 *       void set(VGMask const Mask);       set(VGMask::Logger_Basic);
 */
struct VerboGroupMask
{
   using Base_T = VerboGroup::Base_T;

   /**
    * @brief Underlying mask definitions.
    */
   enum class T : Base_T
   {
   #if (YM_LITE)
      
   #else

   #endif


   /// @brief Macro to facilitate macro overloading.
   #define YM_FMT_MSK(...) YM_MACRO_OVERLOAD(YM_FMT_MSK, __VA_ARGS__)

   /// @brief Convenience macros.
   #define YM_FMT_MSK2(Group_, Mask_) ((std::to_underlying(VerboGroup::T::Group_) << VerboGroup::getNMaskBits()) | static_cast<Base_T>(Mask_))
   #define YM_FMT_MSK1(Group_       ) YM_FMT_MSK2(Group_, (Base_T(1) << VerboGroup::getNMaskBits()) - Base_T(1))

   #define YM_MAKE_MSK_AND_UNIT_MSK(Name_)             \
                 Name_ = YM_FMT_MSK(           Name_), \
      UnitTest_##Name_ = YM_FMT_MSK(UnitTest_##Name_)

      General = YM_FMT_MSK(General),
      Debug   = YM_FMT_MSK(Debug  ),
      Warning = YM_FMT_MSK(Warning),
      Error   = YM_FMT_MSK(Error  ),

      YM_MAKE_MSK_AND_UNIT_MSK(ArgParser      ),
      YM_MAKE_MSK_AND_UNIT_MSK(DataLogger     ),
      YM_MAKE_MSK_AND_UNIT_MSK(FileIO         ),
      YM_MAKE_MSK_AND_UNIT_MSK(Logger         ),
      YM_MAKE_MSK_AND_UNIT_MSK(TextLogger     ),
         TextLogger_Basic  = YM_FMT_MSK(TextLogger, 0b0000'0001),
         TextLogger_Detail = YM_FMT_MSK(TextLogger, 0b0000'0010),
      YM_MAKE_MSK_AND_UNIT_MSK(ThreadSafeProxy),
      YM_MAKE_MSK_AND_UNIT_MSK(MemIO          ),
      YM_MAKE_MSK_AND_UNIT_MSK(Ops            ),
      YM_MAKE_MSK_AND_UNIT_MSK(Rng            ),
         Rng_Prng = YM_FMT_MSK(Rng, 0b0000'0001),
         Rng_Trng = YM_FMT_MSK(Rng, 0b0000'0010),
      YM_MAKE_MSK_AND_UNIT_MSK(Timer          ),
      YM_MAKE_MSK_AND_UNIT_MSK(YmAssert       ),
         YmAssert_Strong = YM_FMT_MSK(YmAssert, 0b0000'0001),
         YmAssert_Weak   = YM_FMT_MSK(YmAssert, 0b0000'0010),
      YM_MAKE_MSK_AND_UNIT_MSK(YmDefs         ),
      YM_MAKE_MSK_AND_UNIT_MSK(YmUtils        ),

   // don't pollute namespace
   #undef YM_MAKE_MSK_AND_UNIT_MSK
   #undef YM_FMT_MSK1
   #undef YM_FMT_MSK2
   #undef YM_FMT_MSK
   };

   /**
    * @brief Convenience functions to grab the group or mask as the underlying type.
    * 
    * @param VG -- The verbosity group mask
    * 
    * @returns auto -- Desired underlying type
    */
   static constexpr auto getGroup(T const VG) { return std::to_underlying(VG) >> VerboGroup::getNMaskBits();    }
   static constexpr auto getMask (T const VG) { return std::to_underlying(VG) & ((Base_T(1) << VerboGroup::getNMaskBits()) - Base_T(1)); }
};

/// @brief Convenience alias (no _T suffix because of common usage).
using VG = VerboGroupMask::T;

} // ym
