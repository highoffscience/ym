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
#include <vector>

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

   auto const Val_float32 = Ops::castTo<float32 >("1.175e-37"            ) == 1.175e-37F;
   auto const Val_float64 = Ops::castTo<float64 >("2.225e-307"           ) == 2.225e-307;
   auto const Val_float80 = Ops::castTo<floatext>("3.362e-4931"          ) == 3.362e-4931L;

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

   std::vector<bool> badCasts_char{false, false}; // until told otherwise
   try { (void)Ops::castToChar("");   } catch (Ops::OpsError_BadCast const & E) { badCasts_char[0] = true; }
   try { (void)Ops::castToChar("it"); } catch (Ops::OpsError_BadCast const & E) { badCasts_char[1] = true; }

   std::vector<bool> badCasts_int8{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<int8>("");     } catch (Ops::OpsError_BadCast const & E) { badCasts_int8[0] = true; }
   try { (void)Ops::castTo<int8>("-");    } catch (Ops::OpsError_BadCast const & E) { badCasts_int8[1] = true; }
   try { (void)Ops::castTo<int8>("-129"); } catch (Ops::OpsError_BadCast const & E) { badCasts_int8[2] = true; }

   std::vector<bool> badCasts_int16{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<int16>("");       } catch (Ops::OpsError_BadCast const & E) { badCasts_int16[0] = true; }
   try { (void)Ops::castTo<int16>("-");      } catch (Ops::OpsError_BadCast const & E) { badCasts_int16[1] = true; }
   try { (void)Ops::castTo<int16>("-32769"); } catch (Ops::OpsError_BadCast const & E) { badCasts_int16[2] = true; }

   std::vector<bool> badCasts_int32{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<int32>("");            } catch (Ops::OpsError_BadCast const & E) { badCasts_int32[0] = true; }
   try { (void)Ops::castTo<int32>("-");           } catch (Ops::OpsError_BadCast const & E) { badCasts_int32[1] = true; }
   try { (void)Ops::castTo<int32>("-2147483649"); } catch (Ops::OpsError_BadCast const & E) { badCasts_int32[2] = true; }

   std::vector<bool> badCasts_int64{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<int64>("");                     } catch (Ops::OpsError_BadCast const & E) { badCasts_int64[0] = true; }
   try { (void)Ops::castTo<int64>("-");                    } catch (Ops::OpsError_BadCast const & E) { badCasts_int64[1] = true; }
   try { (void)Ops::castTo<int64>("-9223372036854775809"); } catch (Ops::OpsError_BadCast const & E) { badCasts_int64[2] = true; }

   std::vector<bool> badCasts_uint8{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<uint8>("");     } catch (Ops::OpsError_BadCast const & E) { badCasts_uint8[0] = true; }
   try { (void)Ops::castTo<uint8>("+");    } catch (Ops::OpsError_BadCast const & E) { badCasts_uint8[1] = true; }
   try { (void)Ops::castTo<uint8>("+256"); } catch (Ops::OpsError_BadCast const & E) { badCasts_uint8[2] = true; }

   std::vector<bool> badCasts_uint16{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<uint16>("");       } catch (Ops::OpsError_BadCast const & E) { badCasts_uint16[0] = true; }
   try { (void)Ops::castTo<uint16>("+");      } catch (Ops::OpsError_BadCast const & E) { badCasts_uint16[1] = true; }
   try { (void)Ops::castTo<uint16>("+65536"); } catch (Ops::OpsError_BadCast const & E) { badCasts_uint16[2] = true; }

   std::vector<bool> badCasts_uint32{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<uint32>("");            } catch (Ops::OpsError_BadCast const & E) { badCasts_uint32[0] = true; }
   try { (void)Ops::castTo<uint32>("+");           } catch (Ops::OpsError_BadCast const & E) { badCasts_uint32[1] = true; }
   try { (void)Ops::castTo<uint32>("+4294967296"); } catch (Ops::OpsError_BadCast const & E) { badCasts_uint32[2] = true; }

   std::vector<bool> badCasts_uint64{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<uint64>("");                      } catch (Ops::OpsError_BadCast const & E) { badCasts_uint64[0] = true; }
   try { (void)Ops::castTo<uint64>("+");                     } catch (Ops::OpsError_BadCast const & E) { badCasts_uint64[1] = true; }
   try { (void)Ops::castTo<uint64>("+18446744073709551616"); } catch (Ops::OpsError_BadCast const & E) { badCasts_uint64[2] = true; }

   std::vector<bool> badCasts_flt32{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<float32>("");           } catch (Ops::OpsError_BadCast const & E) { badCasts_flt32[0] = true; }
   try { (void)Ops::castTo<float32>(".");          } catch (Ops::OpsError_BadCast const & E) { badCasts_flt32[1] = true; }
   try { (void)Ops::castTo<float32>("+1.175e-38"); } catch (Ops::OpsError_BadCast const & E) { badCasts_flt32[2] = true; }

   std::vector<bool> badCasts_flt64{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<float64>("");            } catch (Ops::OpsError_BadCast const & E) { badCasts_flt64[0] = true; }
   try { (void)Ops::castTo<float64>(".");           } catch (Ops::OpsError_BadCast const & E) { badCasts_flt64[1] = true; }
   try { (void)Ops::castTo<float64>("+2.225e-308"); } catch (Ops::OpsError_BadCast const & E) { badCasts_flt64[2] = true; }

   std::vector<bool> badCasts_flt80{false, false, false}; // until told otherwise
   try { (void)Ops::castTo<floatext>("");             } catch (Ops::OpsError_BadCast const & E) { badCasts_flt80[0] = true; }
   try { (void)Ops::castTo<floatext>(".");            } catch (Ops::OpsError_BadCast const & E) { badCasts_flt80[1] = true; }
   try { (void)Ops::castTo<floatext>("+3.362e-4932"); } catch (Ops::OpsError_BadCast const & E) { badCasts_flt80[2] = true; }

   return {
      {"BadCasts_char",   badCasts_char  },

      {"BadCasts_int8",   badCasts_int8  },
      {"BadCasts_int16",  badCasts_int16 },
      {"BadCasts_int32",  badCasts_int32 },
      {"BadCasts_int64",  badCasts_int64 },

      {"BadCasts_uint8",  badCasts_uint8 },
      {"BadCasts_uint16", badCasts_uint16},
      {"BadCasts_uint32", badCasts_uint32},
      {"BadCasts_uint64", badCasts_uint64},

      {"BadCasts_flt32",  badCasts_flt32 },
      {"BadCasts_flt64",  badCasts_flt64 },
      {"BadCasts_flt80",  badCasts_flt80 }
   };
}
