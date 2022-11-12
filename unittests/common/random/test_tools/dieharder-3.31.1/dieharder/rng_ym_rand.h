/**
 * @file    rng_ym_rand.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 * 
 * @note If modifying the implementation don't forget to update
 *       < @link random.h @endlink >.
 */

#ifndef __YM_DIEHARDER_RNG_YM_RAND__
#define __YM_DIEHARDER_RNG_YM_RAND__

#include "dieharder.h"

typedef unsigned long int uint64;
typedef double            float64;

static uint64  ym_rand_get       (void * vstate);
static float64 ym_rand_get_double(void * vstate);
static void    ym_rand_set       (void * vstate,
                                  uint64 seed);

typedef struct
{
   uint64 s0;
   uint64 s1;
   uint64 s2;
   uint64 s3;
} ym_rand_state_t;

static uint64 ym_rand_get(void * vstate)
{
   ym_rand_state_t * const state_Ptr = (ym_rand_state_t *)vstate;

   uint64 const Result = state_Ptr->s0 + state_Ptr->s3;

   uint64 const Tmp = state_Ptr->s1 << 17ul;

   state_Ptr->s2 ^= state_Ptr->s0;
   state_Ptr->s3 ^= state_Ptr->s1;
   state_Ptr->s1 ^= state_Ptr->s2;
   state_Ptr->s0 ^= state_Ptr->s3;

   state_Ptr->s2 ^= Tmp;

   state_Ptr->s3 ^= (state_Ptr->s3 << 45ul) | (state_Ptr->s3 >> (64ul - 45ul)); // rotate left

   return Result;
}

static float64 ym_rand_get_double(void * vstate)
{
   ym_rand_state_t * const state_Ptr = (ym_rand_state_t *)vstate;

   typedef union
   {
      uint64  u64;
      float64 f64;
   } UF_t;
   
   UF_t uf;
   uf.u64 = state_Ptr->s0 + state_Ptr->s3;

   uint64 const Tmp = state_Ptr->s1 << 17ul;

   state_Ptr->s2 ^= state_Ptr->s0;
   state_Ptr->s3 ^= state_Ptr->s1;
   state_Ptr->s1 ^= state_Ptr->s2;
   state_Ptr->s0 ^= state_Ptr->s3;

   state_Ptr->s2 ^= Tmp;

   state_Ptr->s3 ^= (state_Ptr->s3 << 45ul) | (state_Ptr->s3 >> (64ul - 45ul)); // rotate left

   uf.u64  = uf.u64 >> (64ul - 52ul); // move highest bits into mantissa
   uf.u64 |= 1023ul << 52ul; // bias exponent
   uf.f64 -= 1.0;

   return uf.f64;
}

static void ym_rand_set(void * vstate,
                        uint64 seed)
{
   ym_rand_state_t * const state_Ptr = (ym_rand_state_t *)vstate;

   // the additions guarantee the state isn't zero everywhere
   state_Ptr->s0 = seed;
   state_Ptr->s1 = seed + 1ul;
   state_Ptr->s2 = seed + 2ul;
   state_Ptr->s3 = seed + 3ul;
}

static const gsl_rng_type ym_rand_type =
{
   "ym_rand",
   ULONG_MAX, // RAND_MAX
   0,         // RAND_MIN
   sizeof(ym_rand_state_t),
   &ym_rand_set,
   &ym_rand_get,
   &ym_rand_get_double
};

const gsl_rng_type *gsl_rng_ym_rand = &ym_rand_type;

#endif // __YM_DIEHARDER_RNG_YM_RAND__
