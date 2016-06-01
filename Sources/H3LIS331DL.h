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
	RATE_400Hz 	= 0x02,
	RATE_1000Hz	= 0x03
}SRATE_t;

/* Axis for new data flag */
typedef enum {
	X_AXIS_DA	= 0x01,
	Y_AXIS_DA	= 0x02,
	Z_AXIS_DA	= 0x04,
	ALL_AXES_DA	= 0x08
}AXES_DA_t;

/* Axis for data overrun flag */
typedef enum {
	X_AXIS_OR	= 0x01,
	Y_AXIS_OR	= 0x02,
	Z_AXIS_OR 	= 0x04,
	ALL_AXES_OR	= 0x08
}AXES_OR_t;

/* Driver Functions */

int16_t getRawData(void);

void initH3LI(void);

bool isNewDataAvailable(AXES_DA_t ax);
bool dataOverrun(AXES_OR_t ax);
uint8_t readInt1Source(void);
uint8_t readInt2Source(void);


H3LI_TDataState deviceData;



#endif /* SOURCES_H3LIS331DL_H_ */
