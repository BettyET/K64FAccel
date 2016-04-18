/*
 * Timer.c
 *
 *  Created on: 18.04.2016
 *      Author: User
 */


#include "Timer.h"
#include "PE_Types.h"

int32_t counter = 0;

void TMR_OnInterrupt(void)
{
	counter++;
}
