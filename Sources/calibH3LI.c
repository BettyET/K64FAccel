/*
 * calibH3LI.c
 *
 *  Created on: 12.04.2016
 *      Author: User
 */

#include "calibH3LI.h"
#include "WAIT1.h"
#include "H3LIS331DL.h"
#include "LED_R.h"
#include "LED_G.h"
#include "LED_B.h"

static int16_t accelMemory[MEMORY_SIZE]={0};
state_kinds status = START_CALIB;

int16_t zerGOff = 295;				/* zero g offset, substract from raw value */
int16_t gain = 173;					/* number of digits corresponding to 1g */


/* Prototype */
void blinkRed(void);
void blinkGreen(void);
void blinkBlue(void);

void calibrateH3LI(void){
	static int16_t accelPos;
	static int16_t accelNeg;
	int32_t accelSum =0;
	switch(status){
		case START_CALIB:
			blinkRed();								/* ready to calibrate */
			status = READ_POS_DIR;
			break;
		case READ_POS_DIR:							/* read acceleration positive direction */
			LED_G_On();
			WAIT1_Waitms(5000);
			LED_G_Off();
			for (int i=MEMORY_SIZE; i>0;i--){
				WAIT1_Waitms(10);
				accelMemory[i-1] = getRawData();		/* Werte speichern */
				accelSum += (int32_t)accelMemory[i-1];		/* Werte addieren */
			}
			accelPos = (uint16_t)(accelSum / MEMORY_SIZE);/* Durchschnitt */
			blinkGreen();								/* turn the device */
			status = READ_NEG_DIR;
			break;
		case READ_NEG_DIR:			/* read acceleration negative direction */
			LED_B_On();
			WAIT1_Waitms(5000);
			LED_B_Off();
			for (int i=MEMORY_SIZE; i>0;i--){
				WAIT1_Waitms(10);
				accelMemory[i-1] = getRawData();		/* Werte speichern */
				accelSum += (int32_t)accelMemory[i-1];		/* Werte addieren */
			}
			accelNeg = (int16_t)(accelSum / MEMORY_SIZE);/* Durchschnitt */
			gain = (accelPos- accelNeg)/2;				 /* sensitivity */
			zerGOff = accelPos-gain;					 /* zero g offset */
			status = START_CALIB;
			blinkBlue();								/* end of calibration */
			break;
	}

}

void blinkRed(void){
	for(int i=8; i>0;i--){	/* start calibration modus */
		LED_R_Neg();
		WAIT1_Waitms(100);
	}
}

void blinkGreen(void){
	for(int i=8; i>0;i--){	/* start calibration modus */
		LED_G_Neg();
		WAIT1_Waitms(100);
	}
}

void blinkBlue(void){
	for(int i=8; i>0;i--){	/* start calibration modus */
		LED_B_Neg();
		WAIT1_Waitms(100);
	}
}
