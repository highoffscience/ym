/**
 * @author Forrest Jablonski
 */

#pragma once

#include "verbose.h"

static inline unsigned const StepperCmd  = atomicLeftShift(&s_vGroupCounter);
static inline unsigned const StepperOpto = atomicLeftShift(&s_vGroupCounter);
