// /**
//  * @file    rng.cpp
//  * @version 1.0.0
//  * @author  Forrest Jablonski
//  */

// #include "rng.h"

// /** Prng
//  * 
//  * @brief Constructor.
//  */
// ym::Prng::Prng(void)
//    : Prng(0x763b'15c2'1847'ea8d_u64)
// {
// }

// /** Prng
//  * 
//  * @brief Constructor.
//  * 
//  * @param Seed -- Seed.
//  */
// ym::Prng::Prng(State_T const Seed)
//    : _seed  {Seed},
//      _state {Seed}
// {
// }

// /** setSeed
//  * 
//  * @brief Sets the seed for the PRNG.
//  * 
//  * @param Seed -- Seed.
//  */
// void ym::Prng::setSeed(State_T const Seed)
// {
//    _seed  = Seed;
//    _state = Seed;
// }

// /** jump
//  * 
//  * @brief Advances the state by nJumps.
//  * 
//  * @param nJumps -- # of jumps to perform.
//  */
// void ym::Prng::jump(uint64 nJumps)
// {
//    auto acc_mult = 1_u64;
//    auto acc_plus = 0_u64;
//    auto cur_mult = _s_Mult;
//    auto cur_plus = _s_Plus;

//    while (nJumps > 0_u64)
//    {
//       if (nJumps & 1_u64)
//       {
//          acc_mult *= cur_mult;
//          acc_plus = acc_plus * cur_mult + cur_plus;
//       }
//       cur_plus = (cur_mult + 1_u64) * cur_plus;
//       cur_mult *= cur_mult;
//       nJumps /= 2_u64;
//    }

//    _state = (_state * acc_mult) + acc_plus;
// }

// // -----------------------------------------------------------------------------
// // -----------------------------------------------------------------------------

// /** Trng
//  * 
//  * @brief Constructor.
//  */
// ym::Trng::Trng(void)
//    : _prng {}
// {
// }

// /** Trng
//  * 
//  * @brief Constructor.
//  * 
//  * @param Seed -- Seed.
//  */
// ym::Trng::Trng(Prng::State_T const Seed)
//    : _prng {Seed}
// {
// }
