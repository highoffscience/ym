/**
 * @author Forrest Jablonski
 */

#include "random_unittest.h"

#include <cmath>
#include <cstdio>

/**
 *
 */
ym::unittest::Random_UnitTest::Random_UnitTest(void)
   : _rand {}
{
}

/**
 *
 */
void ym::unittest::Random_UnitTest::runTests(void)
{
   //minMaxAveTest();
   //binTest();
   piTest();
}

/**
 *
 */
void ym::unittest::Random_UnitTest::minMaxAveTest(void)
{
   using namespace ym;
   typedef float64 dim_t;

   dim_t min = 1;
   dim_t max = 0;
   dim_t ave = 0;
   
   constexpr uint32 NDataPoints = 1'000'000u;

   for (uint32 i = 0; i < NDataPoints; ++i)
   {
      auto const R = _rand.gen<dim_t>();
      if (R < min) { min = R; }
      if (R > max) { max = R; }
      ave += R;
   }
   ave /= NDataPoints;

   constexpr dim_t MaxEpsilon = 0.001;

   if (min > MaxEpsilon)
   {
      // uh oh
      std::printf("Min fail...\n");
   }

   if (max < static_cast<dim_t>(1.0) - MaxEpsilon)
   {
      // uh oh
      std::printf("Max fail...\n");
   }

   if (std::abs(static_cast<dim_t>(0.5) - ave) > MaxEpsilon)
   {
      // uh oh
      std::printf("Ave fail...\n");
   }

   std::printf("min %.10lf (~0)\n"
               "max %.10lf (~1)\n"
               "ave %.10lf (~0.5)\n",
               min,
               max,
               ave);
}

/**
 *
 */
void ym::unittest::Random_UnitTest::binTest(void)
{
   using namespace ym;
   typedef float64 dim_t;

   constexpr uint32 NDataPoints = 1'000'000'000u;
   constexpr uint32 IdealBucketSize = 1'000'000u;
   constexpr uint32 NBuckets = NDataPoints / IdealBucketSize;

   static_assert(NDataPoints % IdealBucketSize == 0, "Not all data points would fit into a bucket");

   uint32 buckets[NBuckets] = {0};

   for (uint32 i = 0; i < NDataPoints; ++i)
   {
      auto const R = _rand.gen<dim_t>();
      auto const R_i = static_cast<uint32>(R * NBuckets);
      buckets[R_i] += 1;
   }

   uint32 maxDiff = 0;
   for (uint32 i = 0; i < NBuckets; ++i)
   {
      uint32 const Diff = static_cast<uint32>(
         std::abs(static_cast<int64>(buckets[i]) - static_cast<int64>(IdealBucketSize)));

      if (Diff > maxDiff)
      {
         maxDiff = Diff;
      }
   }

   auto const MaxDiffError_percent = (static_cast<dim_t>(maxDiff) / static_cast<dim_t>(IdealBucketSize)) * 100.0;

   // TODO check max allowed error

   std::printf("Max bucket error %.5lf%% (~0)\n", MaxDiffError_percent);
}

/**
 *
 */
void ym::unittest::Random_UnitTest::piTest(void)
{
   using namespace ym;
   typedef float64 dim_t;

   constexpr uint32 NDataPoints = 1'000'000'000u;
   uint32 nPointsInCircle = 0;

   for (uint32 i = 0; i < NDataPoints; ++i)
   {
      auto const X = (_rand.gen<dim_t>() * 2.0) - 1.0;
      auto const Y = (_rand.gen<dim_t>() * 2.0) - 1.0;
      auto const R2 = (X * X) + (Y * Y);

      if (R2 < 1.0)
      {
         nPointsInCircle += 1;
      }
   }

   auto const Pi = (4.0 * nPointsInCircle) / NDataPoints;
   std::printf("Pi ~ %.15lf\n", Pi);
}
