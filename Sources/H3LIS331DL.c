/*
 * H3LIS331DL.c
 *
 *  Created on: 28.03.2016
 *      Author: User
 */


#include "H3LIS331DL.h"
#include "Error.h"
#include "CI2C1.h"
#include "WAIT1.h"
#include "PE_Types.h"

/* Device adress */
#define DEV_ADD 0x18
/* Device identification register */
#define WHO_AM_I 0x0F

/* Status register */
#define STATUS_REG 	0x27

/* Interrupt control register */
#define CTRL_REG_3 	0x22

/* Interrupt source register */
#define INT1_SRC 	0x31
#define INT2_SRC	0x35

/* Interrupt configuration register */
#define INT1_CFG 	0x30
#define INT2_CFG 	0x34
#define INT1_THS	0x32
#define INT2_THS	0x36
#define NT1_DURATION 0x33
#define NT2_DURATION 0x37

/* External 3-axis accelerometer control register addresses */
#define CTRL_REG_1 	0x20
#define CTRL_REG_4 	0x23

/* Acceleration data registers */
#define OUT_Z_L 	0x2C		/* read 1 register */
#define OUT_Z_H 	0x2D
#define OUT_Z_L_MSB 0xAC		/* read more than 1 registers */



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
	uint8_t res;
	/* Register lesen */
	res = H3LI_ReadReg(WHO_AM_I, (uint8_t*)&me, 1);
	if (res != ERR_OK){
			for(;;);									/* error */
	}
	if (me != 0x32){							/* error */
		Err();
	}
}

void setNormalPowerMode(void){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(CTRL_REG_1,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
	reg &= 0x1F;								/* mask */
	reg |= 0x20;								/* normal power mode */
	WAIT1_WaitOSms(1);
	res = H3LI_WriteReg(CTRL_REG_1, reg);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setRange(RANGE_t rg){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(CTRL_REG_4,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
	reg &= 0xCF;									/* mask */
	reg |= rg <<4;
	WAIT1_WaitOSms(1);								/* strange error: need to wait short moment after reading a register before writing */
	res = H3LI_WriteReg(CTRL_REG_4, reg);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setSamplingRate(SRATE_t sr){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(CTRL_REG_1,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
	reg &= 0xE7;								/* mask */
	reg |= sr << 3;
	WAIT1_WaitOSms(1);
	res = H3LI_WriteReg(CTRL_REG_1, reg);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setBlockDataUpdate(void){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(CTRL_REG_4,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
	reg &= 0x7F;								/* mask */
	reg |= 0x80;
	WAIT1_WaitOSms(1);
	res = H3LI_WriteReg(CTRL_REG_4, reg);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setIntCntrReg(void){
	uint8_t res;
	uint8_t reg = 0x24;								/* latch interrupt request */
	res = H3LI_WriteReg(CTRL_REG_3, reg);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setInt1(void){
	uint8_t res;
	uint8_t reg = 0x20;								/* interrupt on high z event */
	res = H3LI_WriteReg(INT1_CFG, reg);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setInt2(void){
	uint8_t res;
	uint8_t reg = 0x10;								/* interrupt on low z event */
	res = H3LI_WriteReg(INT2_CFG, reg);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setInt1Threshold(int ths){
	uint8_t res;
	uint8_t reg = ths;
	if(ths > 127){
		ths = 127;
	}
	res = H3LI_WriteReg(INT1_THS, ths);				/* set threshold for high z event */
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setInt2Threshold(uint8_t ths){
	uint8_t res;
	if(ths > 127){
		ths = 127;
	}
	res = H3LI_WriteReg(INT2_THS, ths);				/* set threshold for low z event */
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setInt1Duration(uint8_t dur){
	uint8_t res;
	if(dur > 127){
		dur = 127;
	}
	res = H3LI_WriteReg(NT1_DURATION, dur);				/* set threshold for low z event */
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

void setInt2Duration(uint8_t dur){
	uint8_t res;
	if(dur > 127){
		dur = 127;
	}
	res = H3LI_WriteReg(NT2_DURATION, dur);				/* set threshold for low z event */
	if (res != ERR_OK){
		for(;;);									/* error */
	}
}

uint8_t readInt1Source(void){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(INT1_SRC,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);								/* error */
	}
	reg &= 0x40;								/* mask */
	WAIT1_Waitus(2);
	return (reg);
}

uint8_t readInt2Source(void){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(INT2_SRC,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);								/* error */
	}
	reg &= 0x40;								/* mask */
	WAIT1_Waitus(2);
	return (reg);
}


bool isNewDataAvailable(AXES_DA_t ax){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(STATUS_REG,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
	reg &= ax;									/* mask */
	return (reg == ax);
}

bool dataOverrun(AXES_OR_t ax){
	uint8_t res;
	uint8_t reg;
	res = H3LI_ReadReg(STATUS_REG,(uint8_t*)&reg, 1);
	if (res != ERR_OK){
		for(;;);								/* error */
	}
	reg &= (ax<<4);								/* mask */
	return(reg == (ax<<4));
}

int16_t getRawData(void){
	uint8_t res;
	res = H3LI_ReadReg(OUT_Z_L_MSB, (int8_t*)&accelZ, 2);
	if (res != ERR_OK){
		for(;;);									/* error */
	}
	return ((int16_t)accelZ[1]<<8)| ((int16_t)accelZ[0]);
}


void initH3LI(void){
	initI2C();
	readIfImMe();								/* communication correct? */
	WAIT1_WaitOSms(1);
	setNormalPowerMode();						/* normal power mode */
	WAIT1_WaitOSms(1);
	setRange(RANGE_200g);						/* range 200g */
	WAIT1_WaitOSms(1);
	setSamplingRate(RATE_400Hz);				/* sampling rate 400Hz */
	WAIT1_WaitOSms(1);
	setBlockDataUpdate();						/* block data update */
	WAIT1_WaitOSms(1);
	setIntCntrReg();							/* interrupt configurations */
	WAIT1_WaitOSms(1);
	setInt1();									/* high z event */
	WAIT1_WaitOSms(1);
	setInt2();									/* low z event */
	WAIT1_WaitOSms(1);
	setInt1Threshold(4);						/* set threshold high z event*/
	WAIT1_WaitOSms(1);
	setInt1Duration(1);							/* set minimal duration for high z event */
	WAIT1_WaitOSms(1);
}

