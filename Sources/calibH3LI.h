/*
 * calibH3LI.h
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#ifndef SOURCES_CALIBH3LI_H_
#define SOURCES_CALIBH3LI_H_

#include "PE_Types.h"

void calibrateH3LI(void);		/* calibrates the sensor in 3 steps */

#define MEMORY_SIZE 10			/* number of calibrating measurements */

typedef enum state_kinds{		/* states */
	START_CALIB,
	READ_POS_DIR,
	READ_NEG_DIR
} state_kinds;

extern int16_t zerGOff;
extern int16_t gain;

#endif /* SOURCES_CALIBH3LI_H_ */
