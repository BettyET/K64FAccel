/*
 * Timer.h
 *
 *  Created on: 18.04.2016
 *      Author: User
 */

#ifndef SOURCES_TIMER_H_
#define SOURCES_TIMER_H_

#include "PE_Types.h"

/* Timer-Einstellung */
#define TMR_TICK_MS  1

extern int32_t counter;

void TMR_OnInterrupt(void);

#endif /* SOURCES_TIMER_H_ */
