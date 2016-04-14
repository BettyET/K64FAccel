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

typedef struct {
  volatile bool dataReceivedFlg; 	/* set to TRUE by the interrupt if we have received data */
  volatile bool dataTransmittedFlg; /* set to TRUE by the interrupt if we have set data */
  LDD_TDeviceData *handle; /* pointer to the device handle */
} H3LI_TDataState;

/* \brief Run the demo application */
void readIfImMe(void);
void initI2C(void);
void setPowerState(void);
void configRange(void);
int16_t getRawData(void);
int16_t getAccData(void);
void initH3LI(void);

H3LI_TDataState deviceData;

#endif /* SOURCES_H3LIS331DL_H_ */
