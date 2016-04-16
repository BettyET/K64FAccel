/*
 * H3LIS331DL.h
 *
 *  Created on: 28.03.2016
 *      Author: User
 */

#ifndef SOURCES_H3LIS331DL_H_
#define SOURCES_H3LIS331DL_H_

#include "PE_Types.h"
#include "PE_LDD.h"

/* Flags for I2C communication */
typedef struct {
  volatile bool dataReceivedFlg; 	/* set to TRUE by the interrupt if we have received data */
  volatile bool dataTransmittedFlg; /* set to TRUE by the interrupt if we have set data */
  LDD_TDeviceData *handle; /* pointer to the device handle */
} H3LI_TDataState;

/* Measurement range  H3LIS331DL_H */
typedef enum {
    RANGE_100g    = 0x00,
    RANGE_200g    = 0x01,
    RANGE_400g    = 0x03
} RANGE_t;

/* Sampling rate H3LIS331DL_H */
typedef enum {
	RATE_50Hz	= 0x00,
	RATE_100Hz	= 0x01,
	RATE_400Hz 	= 0x10,
	RATE_1000Hz	= 0x11
}SRATE_t;

/* Driver Functions */

int16_t getRawData(void);
int16_t getAccData(void);
void initH3LI(void);

H3LI_TDataState deviceData;



#endif /* SOURCES_H3LIS331DL_H_ */
