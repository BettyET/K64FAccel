/*
 * H3LIS331DL.c
 *
 *  Created on: 28.03.2016
 *      Author: User
 */


#include "H3LIS331DL.h"
#include "calibH3LI.h"
#include "CI2C1.h"
#include "WAIT1.h"
#include "PE_Types.h"

/* Device Adress */
#define DEV_ADD 0x18
/* Device identification register */
#define WHO_AM_I 0x0F

/* External 3-axis accelerometer control register addresses */
#define CTRL_REG_1 0x20
#define CTRL_REG_4 0x23

/* Acceleration data registers */
#define OUT_Z_L 0x2C			/* Ein Register auslesen */
#define OUT_Z_L_MSB 0xAC		/* Mehrere Register auslesen */
#define OUT_Z_H 0x2D


static uint8_t accelZ[2]={0,0};

uint8_t H3LI_ReadReg(uint8_t addr, uint8_t *data, short dataSize) {
  uint8_t res;

  /* Send I2C address plus register address to the I2C bus *without* a stop condition */
  res = CI2C1_MasterSendBlock(deviceData.handle, &addr, 1U, LDD_I2C_NO_SEND_STOP);
  if (res!=ERR_OK) {
    return ERR_FAILED;
  }
  while (!deviceData.dataTransmittedFlg) {} /* Wait until data is sent */
  deviceData.dataTransmittedFlg = FALSE;

  /* Receive InpData (1 byte) from the I2C bus and generates a stop condition to end transmission */
  res = CI2C1_MasterReceiveBlock(deviceData.handle, data, dataSize, LDD_I2C_SEND_STOP);
  if (res!=ERR_OK) {
    return ERR_FAILED;
  }
  while (!deviceData.dataReceivedFlg) {} /* Wait until data is received received */
  deviceData.dataReceivedFlg = FALSE;
  return ERR_OK;
}

uint8_t H3LI_WriteReg(uint8_t addr, uint8_t val) {
  uint8_t buf[2], res;

  buf[0] = addr;
  buf[1] = val;
  res = CI2C1_MasterSendBlock(deviceData.handle, &buf, 2U, LDD_I2C_SEND_STOP); /* Send OutData (3 bytes with address) on the I2C bus and generates not a stop condition to end transmission */
  if (res!=ERR_OK) {
    return ERR_FAILED;
  }
  while (!deviceData.dataTransmittedFlg) {}  /* Wait until date is sent */
  deviceData.dataTransmittedFlg = FALSE;
  return ERR_OK;
}

void initI2C(void){
	/* Initialisierung*/
	deviceData.handle = CI2C1_Init(&deviceData);
}

void readIfImMe(void){
	uint8_t me = 0;
	/* Register lesen */
	H3LI_ReadReg(WHO_AM_I, (uint8_t*)&me, 1);
	if (me != 0x32){							/* error */
		LED_G_On();
	}
}

void setPowerState(void){
	uint8_t res;
	uint8_t check;
	res = H3LI_WriteReg(CTRL_REG_1, 0x27);
	if (res != ERR_OK){
			LED_G_On();							/* error */
		}
	WAIT1_Waitms(10);
	res = H3LI_ReadReg(CTRL_REG_1,(uint8_t*)&check, 1);
	if (res != ERR_OK || check != 0x27){		/* power state correct? */
			LED_G_On();							/* error */
		}
}

void configRange(void){
	uint8_t res;
	res = H3LI_WriteReg(CTRL_REG_4, 0x80);
	if (res != ERR_OK){
			LED_G_On();							/* error */
		}
}

int16_t getRawData(void){
	uint8_t res;
	WAIT1_Waitms(10);
	res = H3LI_ReadReg(OUT_Z_L_MSB, (int8_t*)&accelZ, 2);
	if (res != ERR_OK){
			LED_G_On();							/* error */
		}
	return ((int16_t)accelZ[1]<<8)| ((int16_t)accelZ[0]);
}

int16_t getAccData(void){
	int16_t accel = getRawData();
	return (accel-zerGOff);
}

void initH3LI(void){
	initI2C();
	readIfImMe();				/* communication correct? */
	WAIT1_Waitms(10);
	setPowerState();			/* normal power mode */
	WAIT1_Waitms(10);
	configRange();				/* range 100g */
}
