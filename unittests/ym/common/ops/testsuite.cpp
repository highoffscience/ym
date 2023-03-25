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

// TODO delete...I think
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
   TextLogger::getGlobalInstance()->enable(VG::General);

   // TODO this is calling int8! they are technically the same type so...
   auto const Val_char    = Ops::castTo<signed char   >("f"                   );
   // perhaps this?
   enum e_int8 : unsigned char {};
   ymLog(VG::General, "s(e_int8) = %u", sizeof(e_int8));
   ymLog(VG::General, "val = %d", (int)Val_char);

   // auto const Val_int8    = Ops::castTo<int8   >("-128"                );
   // auto const Val_int16   = Ops::castTo<int16  >("-32768"              );
   // auto const Val_int32   = Ops::castTo<int32  >("-2147483648"         );
   // auto const Val_int64   = Ops::castTo<int64  >("-9223372036854775808");

   // auto const Val_uint8   = Ops::castTo<uint8  >("255"                 );
   // auto const Val_uint16  = Ops::castTo<uint16 >("65535"               );
   // auto const Val_uint32  = Ops::castTo<uint32 >("4294967295"          );
   // auto const Val_uint64  = Ops::castTo<uint64 >("18446744073709551615");

   // auto const Val_float32 = Ops::castTo<float32>("1.17549e-38"         );
   // auto const Val_float64 = Ops::castTo<float64>("2.22507e-308"        );
   // auto const Val_float80 = Ops::castTo<float80>("3.3621e-4932"        );

   return {
      {"Val_char"   , Val_char   },

      // {"Val_int8"   , Val_int8   },
      // {"Val_int16"  , Val_int16  },
      // {"Val_int32"  , Val_int32  },
      // {"Val_int64"  , Val_int64  },

      // {"Val_uint8"  , Val_uint8  },
      // {"Val_uint16" , Val_uint16 },
      // {"Val_uint32" , Val_uint32 },
      // {"Val_uint64" , Val_uint64 },

      // {"Val_float32", Val_float32},
      // {"Val_float64", Val_float64},
      // {"Val_float80", Val_float80}
   };
}
