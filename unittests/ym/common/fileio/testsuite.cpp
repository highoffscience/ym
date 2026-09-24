/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "fileio.h" // Structures under test

#include "fmt/format.h"

#include <array>
#include <cstdio>
#include <cstring>

/**
 * @brief Constructor.
 */
ym::unit::fileio::TestSuite::TestSuite(void) :
   TestSuiteBase("FileIO")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::fileio::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::fileio::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   strlit const Filename = "ym/common/fileio/data.txt";
   strlit const TestData = "Go! Torchic!";

   { // write to file
      FileIO outfile(Filename, "wb");
      std::fputs(TestData, outfile);
   }

   auto const Exists = FileIO::exists(Filename);
   auto const NotExists = FileIO::exists("ym/common/fileio/no_exists.txt");

   std::array<char, 1024uz> buffer_1{'\0'};
   { // read file
      FileIO infile(Filename);
      std::ignore = infile.fillBuffer(buffer_1);
   }

   std::array<char, buffer_1.size()> buffer_2{'\0'};
   { // read file in a different way
      FileIO infile(Filename);
      for (
         std::optional<std::span<char>> data{{buffer_2.data(), 100uz}};
         (data = infile.fillBufferPiecewise(*data));
         data = {data->data() + data->size(), data->size()})
      { // read file in one chunk at a time
      }
   }

   auto const Equaled =
      (std::strncmp(buffer_1.data(), buffer_2.data(), buffer_2.size()) == 0) &&
      (std::strncmp(buffer_1.data(), TestData, buffer_2.size()) == 0);

   return {
      {"Exists", Exists},
      {"NotExists", NotExists},
      {"BuffersEqualTestData", Equaled}
   };
}
