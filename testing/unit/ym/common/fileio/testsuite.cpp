/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "fileio.h" // Structures under test

#include "catch2/catch_test_macros.hpp"
#include "fmt/format.h"

#include <array>
#include <cstdio>
#include <cstring>

/**
 * - @ref ym::common::FileIO Shall ?
 */
TEST_CASE("Class_FileIO", "[fileio][smoketest]")
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

   INFO("Testing that file exists");
   REQUIRE(Exists);
}
