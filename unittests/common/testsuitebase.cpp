/**
 * @file    testsuitebase.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuitebase.h"

#include "textlogger.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

/** TestSuiteBase
 *
 * @brief Constructor.
 */
ym::ut::TestSuiteBase::TestSuiteBase(std::string name) :
   PermaNameable_NV(std::move(name)),
   _testCases {/*default*/}
{
   ymLogEnable(VG::Error); // TODO add more setup logic
}

/** runTestCase
 *
 * @brief Runs specified test case.
 * 
 * @throws std::runtime_error -- If requested test case is not found.
 * @throws Whatever TestCase::run throws.
 * 
 * @param Name   -- Name of test case to run.
 * @param InData -- Additional input data for test case.
 * 
 * @returns DataShuttle -- Results of test case.
 */
auto ym::ut::TestSuiteBase::runTestCase(
   std::string const & Name,
   DataShuttle const & InData) -> DataShuttle
{
   DataShuttle ds{};

   auto const It = std::find_if(_testCases.begin(), _testCases.end(),
      [Name](TCArray_T::value_type const & Uptr) {
         return Uptr->getName() == Name;
      }
   );

   if (It != _testCases.end())
   { // test case found
      try
      { // catch stray exceptions
         ds = (*It)->run(InData);
      }
      catch (std::exception const & E)
      { // repackage exception cppyy can understand and throw
         throw std::runtime_error(E.what());
      }
   }
   else
   { // test case not found
      using namespace std::string_literals;
      throw std::runtime_error("Test case \""s + Name + "\" not found. "\
         "Perhaps you forgot to call addTestCase() in TestSuite constructor?"s);
   }

   return ds;
}
