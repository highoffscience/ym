/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "ymassert.h" // Structures under test

#include "fmt/format.h"

#include <cstring>
#include <string>

/**
 * @brief Constructor.
 */
ym::unit::ymassert::TestSuite::TestSuite(void) :
   TestSuiteBase("YmAssert")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<What>();
   addTestCase<Assertion>();
   addTestCase<Class_ymassert_Base>();
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymassert::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   YM_DECL_YMASSERT(Error)

   auto const I = 9;
   auto const J = 5;

   auto errored = false; // until told otherwise

   try
   {
      YMASSERT(I < J, Error, YM_DAH, "I ({}) is NOT less than J ({})", I, J);
   }
   catch (ymassert_Base const & E)
   {
      errored = true;
   }

   return {
      {"Errored", errored}
   };
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymassert::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymassert::TestSuite::What::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   bool expectedMsg = false;

   YM_DECL_YMASSERT(Error)

   try
   {
      YMASSERT(false, Error, YM_DAH, "Go! {}!", "Torchic");
   }
   catch (std::exception const & E)
   {
      // str const ExpectedMsg =
      //    "Assert @ \"/home/forrest/code/ym/unittests/ym/common/ymassert/testsuite.cpp:76\": Go! Torchic!";
      // expectedMsg = std::strcmp(E.what(), ExpectedMsg) == 0;
   }

   return {
      {"ExpectedMsg", expectedMsg}
   };
}

/**
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymassert::TestSuite::Assertion::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   bool expectedFalseAssert = false;
   bool expectedTrueAssert  = true;

   YM_DECL_YMASSERT(Error)

   try
   {
      YMASSERT(false, Error, YM_DAH, "");
   }
   catch (std::exception const & E)
   {
      expectedFalseAssert = true;
   }

   try
   {
      YMASSERT(true, Error, YM_DAH, "");
   }
   catch (std::exception const & E)
   {
      expectedTrueAssert = false;
   }

   return {
      {"ExpectedFalseAssert", expectedFalseAssert},
      {"ExpectedTrueAssert",  expectedTrueAssert }
   };
}

/**
 * - @ref ym::ymassert_Base Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymassert::TestSuite::Class_ymassert_Base::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}
