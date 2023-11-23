/**
 * @file    commanddefs.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "signal.h"

#include "../common/ymdefs.h"

/**
 *
 */
#define YM_CMD_DECL_CLASS(ClassName,                                       \
                          Domain_T, DomainValue,                           \
                          Signal_T, SignalValue)                           \
   class ClassName : public TaggedMessage<Domain_T, Domain_T::DomainValue, \
                                          Signal_T, Signal_T::SignalValue>

/**
 *
 */
#define YM_CMD_DECL_DEFN_EMPTY_CLASS(ClassName,             \
                                     Domain_T, DomainValue, \
                                     Signal_T, SignalValue) \
   YM_CMD_DECL_CLASS(ClassName,                             \
                     Domain_T, DomainValue,                 \
                     Signal_T, SignalValue)                 \
   {                                                        \
   public:                                                  \
      explicit inline ClassName(void)                       \
         : TaggedMessage(#ClassName)                        \
      {}                                                    \
   }
