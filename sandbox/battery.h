/**
 * @author Forrest Jablonski
 */

#pragma once

#include "verbose.h"

static inline unsigned const BatteryGauge   = atomicLeftShift(&s_vGroupCounter);
static inline unsigned const BatteryCharger = atomicLeftShift(&s_vGroupCounter);
