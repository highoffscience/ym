/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "ops.h"

#include "textlogger.h"
#include "ymception.h"

#include <limits>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("Ops")
{
   addTestCase<Casting   >();
   addTestCase<BadCasting>();
}

/** run
 *
 * @brief Tests the primitive casting functions.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::Casting::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_Ops);

   auto const Val_char    = Ops::castToChar   ("A"                      ) == 'A';

   auto const Val_int8    = Ops::castTo<int8 >("-128"                   ) == std::numeric_limits<int8  >::min();
   auto const Val_int16   = Ops::castTo<int16>("-32768"                 ) == std::numeric_limits<int16 >::min();
   auto const Val_int32   = Ops::castTo<int32>("-2147483648"            ) == std::numeric_limits<int32 >::min();
   auto const Val_int64   = Ops::castTo<int64>("-9223372036854775808"   ) == std::numeric_limits<int64 >::min();

   auto const Val_uint8   = Ops::castTo<uint8  >("+255"                 ) == std::numeric_limits<uint8 >::max();
   auto const Val_uint16  = Ops::castTo<uint16 >("+65535"               ) == std::numeric_limits<uint16>::max();
   auto const Val_uint32  = Ops::castTo<uint32 >("+4294967295"          ) == std::numeric_limits<uint32>::max();
   auto const Val_uint64  = Ops::castTo<uint64 >("+18446744073709551615") == std::numeric_limits<uint64>::max();

   auto const Val_float32 = Ops::castTo<float32>("1.175e-37"            ) == 1.175e-37F;
   auto const Val_float64 = Ops::castTo<float64>("2.225e-307"           ) == 2.225e-307;
   auto const Val_float80 = Ops::castTo<float80>("3.362e-4931"          ) == 3.362e-4931L;

   return {
      {"Val_char"   , Val_char   },

      {"Val_int8"   , Val_int8   },
      {"Val_int16"  , Val_int16  },
      {"Val_int32"  , Val_int32  },
      {"Val_int64"  , Val_int64  },

      {"Val_uint8"  , Val_uint8  },
      {"Val_uint16" , Val_uint16 },
      {"Val_uint32" , Val_uint32 },
      {"Val_uint64" , Val_uint64 },

      {"Val_float32", Val_float32},
      {"Val_float64", Val_float64},
      {"Val_float80", Val_float80}
   };
}

/** run
 *
 * @brief Tests the primitive casting functions error on invalid inputs.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BadCasting::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_Ops);

   auto badCast_char = false; // until told otherwise
   try
   {
      (void)Ops::castToChar("");
      (void)Ops::castToChar("7");
      (void)Ops::castToChar("it");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_char = true;
   }

   auto badCast_int8 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<int8>("");
      (void)Ops::castTo<int8>("-");
      (void)Ops::castTo<int8>("-129");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_int8 = true;
   }

   auto badCast_int16 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<int16>("");
      (void)Ops::castTo<int16>("-");
      (void)Ops::castTo<int16>("-32769");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_int16 = true;
   }

   auto badCast_int32 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<int32>("");
      (void)Ops::castTo<int32>("-");
      (void)Ops::castTo<int32>("-2147483649");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_int32 = true;
   }

   auto badCast_int64 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<int64>("");
      (void)Ops::castTo<int64>("-");
      (void)Ops::castTo<int64>("-9223372036854775809");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_int64 = true;
   }

   auto badCast_uint8 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<uint8>("");
      (void)Ops::castTo<uint8>("+");
      (void)Ops::castTo<uint8>("+256");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_uint8 = true;
   }

   auto badCast_uint16 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<uint16>("");
      (void)Ops::castTo<uint16>("+");
      (void)Ops::castTo<uint16>("+65536");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_uint16 = true;
   }

   auto badCast_uint32 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<uint32>("");
      (void)Ops::castTo<uint32>("+");
      (void)Ops::castTo<uint32>("+4294967296");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_uint32 = true;
   }

   auto badCast_uint64 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<uint64>("");
      (void)Ops::castTo<uint64>("+");
      (void)Ops::castTo<uint64>("+18446744073709551616");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_uint64 = true;
   }

   auto badCast_flt32 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<float32>("");
      (void)Ops::castTo<float32>(".");
      (void)Ops::castTo<float32>("+1.175e-38");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_flt32 = true;
   }

   auto badCast_flt64 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<float64>("");
      (void)Ops::castTo<float64>(".");
      (void)Ops::castTo<float64>("+2.225e-308");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_flt64 = true;
   }

   auto badCast_flt80 = false; // until told otherwise
   try
   {
      (void)Ops::castTo<float80>("");
      (void)Ops::castTo<float80>(".");
      (void)Ops::castTo<float80>("+3.362e-4932");
   }
   catch (Ops::OpsError_BadCast const & E)
   {
      badCast_flt80 = true;
   }

   return {
      {"BadCast_char",   badCast_char  },

      {"BadCast_int8",   badCast_int8  },
      {"BadCast_int16",  badCast_int16 },
      {"BadCast_int32",  badCast_int32 },
      {"BadCast_int64",  badCast_int64 },

      {"BadCast_uint8",  badCast_uint8 },
      {"BadCast_uint16", badCast_uint16},
      {"BadCast_uint32", badCast_uint32},
      {"BadCast_uint64", badCast_uint64},

      {"BadCast_flt32",  badCast_flt32 },
      {"BadCast_flt64",  badCast_flt64 },
      {"BadCast_flt80",  badCast_flt80 }
   };
}
