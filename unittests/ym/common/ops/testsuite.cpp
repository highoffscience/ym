/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ym.h"
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
   addTestCase<Casting>();
}

/** run
 *
 * @brief TODO.
 *
 * @return DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::Casting::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const Val_char    = Ops::castToChar   ("A"                     ) == 'A';

   auto const Val_int8    = Ops::castTo<int8 >("-128"                  ) == std::numeric_limits<int8  >::min();
   auto const Val_int16   = Ops::castTo<int16>("-32768"                ) == std::numeric_limits<int16 >::min();
   auto const Val_int32   = Ops::castTo<int32>("-2147483648"           ) == std::numeric_limits<int32 >::min();
   auto const Val_int64   = Ops::castTo<int64>("-9223372036854775808"  ) == std::numeric_limits<int64 >::min();

   auto const Val_uint8   = Ops::castTo<uint8  >("255"                 ) == std::numeric_limits<uint8 >::max();
   auto const Val_uint16  = Ops::castTo<uint16 >("65535"               ) == std::numeric_limits<uint16>::max();
   auto const Val_uint32  = Ops::castTo<uint32 >("4294967295"          ) == std::numeric_limits<uint32>::max();
   auto const Val_uint64  = Ops::castTo<uint64 >("18446744073709551615") == std::numeric_limits<uint64>::max();

   auto const Val_float32 = Ops::castTo<float32>("1.175e-37"           ) == 1.175e-37F;
   auto const Val_float64 = Ops::castTo<float64>("2.225e-307"          ) == 2.225e-307;
   auto const Val_float80 = Ops::castTo<float80>("3.362e-4931"         ) == 3.362e-4931L;

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
