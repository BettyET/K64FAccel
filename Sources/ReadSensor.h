/*
 * ReadSensor.h
 *
 *  Created on: 18.04.2016
 *      Author: User
 */

#ifndef SOURCES_READSENSOR_H_
#define SOURCES_READSENSOR_H_


void ReadAccelSensorTask(void *pvParameters);

typedef enum {
  SENS_STATE_INITACCEL,
  SENS_STATE_STARTUP,
  SENS_STATE_IDLE,
  SENS_STATE_MEASURE,
  SENS_START_CALIB,
  SENS_READ_POS_DIR,
  SENS_READ_NEG_DIR,

} SensStateType;

void setSensState(SensStateType state);
SensStateType getSensState(void);

void doCalibrationStep(void);

void ReadSensor_Init(void);

#endif /* SOURCES_READSENSOR_H_ */
