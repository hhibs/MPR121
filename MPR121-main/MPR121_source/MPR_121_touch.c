/*
 * MPR_121_touch.c
 *
 *  Created on: Apr 4, 2021
 *      Author: katyk
 */


#include "MPR_121_touch.h"

HAL_StatusTypeDef readRegister8( I2C_HandleTypeDef * hi2c, uint8_t slave_address ,uint8_t reg , uint8_t * readData )
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read( hi2c , slave_address , reg , I2C_MEMADD_SIZE_8BIT , readData , 1 , 50 );

	return status;
}

HAL_StatusTypeDef readRegister16( I2C_HandleTypeDef * hi2c, uint8_t slave_address ,uint8_t reg , uint16_t* readData )
{
	HAL_StatusTypeDef status;

	uint8_t tmp[4] = {0x00 , 0x00};

	status = HAL_I2C_Mem_Read( hi2c , slave_address , reg , I2C_MEMADD_SIZE_16BIT , tmp , 2 , 50 );

	*readData = (uint16_t)( (tmp[1]<<8) | tmp[0]);

	return status;
}

HAL_StatusTypeDef writeRegister( I2C_HandleTypeDef * hi2c, uint8_t slave_address ,uint8_t reg, uint8_t value)
{
	HAL_StatusTypeDef status;

	uint8_t tmp = value;

	status =  HAL_I2C_Mem_Write( hi2c , slave_address , reg , I2C_MEMADD_SIZE_8BIT , &tmp , 1 , 50 );

	return status;
}

HAL_StatusTypeDef MPR_121_begin( I2C_HandleTypeDef * hi2c, uint8_t slave_address , uint8_t touchThreshold , uint8_t releaseThreshold )
{
	HAL_StatusTypeDef status;

	/* Software Reset */
	status = writeRegister( hi2c , slave_address , MPR121_SOFTRESET, 0x63);

	HAL_Delay(5);

	if(status == HAL_OK)
	status = writeRegister( hi2c , slave_address , MPR121_ECR, 0x0);

	if(status == HAL_OK)
	{
		uint8_t c = 0;
		status = readRegister8( hi2c, slave_address , MPR121_CONFIG2 , &c );
		if(status == HAL_OK)
		{
			if (c != 0x24)
			{
				status =  HAL_ERROR;
			}
		}
	}

	if(status == HAL_OK)
	{
		status = MPR_121_setThresholds( hi2c , slave_address , touchThreshold, releaseThreshold);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_MHDR , 0x01);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_NHDR, 0x01);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_NCLR, 0x0E);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_FDLR, 0x00);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_MHDF, 0x01);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_NHDF, 0x05);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_NCLF, 0x01);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_FDLF, 0x00);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_NHDT, 0x00);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_NCLT, 0x00);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_FDLT, 0x00);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_DEBOUNCE, 0);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_CONFIG1, 0x10);
	}

	if(status == HAL_OK)
	{
		status = writeRegister( hi2c , slave_address , MPR121_CONFIG2, 0x20);
	}

#ifdef AUTOCONFIG
	status = writeRegister( hi2c , slave_address , MPR121_AUTOCONFIG0, 0x0B);

	// correct values for Vdd = 3.3V
	status = writeRegister( hi2c , slave_address , MPR121_UPLIMIT, 200);     // ((Vdd - 0.7)/Vdd) * 256
	status = writeRegister( hi2c , slave_address , MPR121_TARGETLIMIT, 180); // UPLIMIT * 0.9
	status = writeRegister( hi2c , slave_address , MPR121_LOWLIMIT, 130);    // UPLIMIT * 0.65
#endif

	if(status == HAL_OK)
	{
		/* enable X electrodes and start MPR1215 bits for baseline tracking & proximity disabled + amount of electrodes running (12) */
		uint8_t ECR_SETTING = 0b10000000 + 12;

		status = writeRegister( hi2c , slave_address , MPR121_ECR, ECR_SETTING); // start with above ECR setting
	}

	return status;
}

HAL_StatusTypeDef MPR_121_filteredData(I2C_HandleTypeDef * hi2c, uint8_t slave_address , uint8_t t , uint16_t * readData)
{
	HAL_StatusTypeDef status;

	if (t > 12)
	{
		status = HAL_ERROR;
	}
	else
	{
		status = readRegister16( hi2c, slave_address , MPR121_FILTDATA_0L + t * 2 , readData );
	}

	return status;
}

uint16_t MPR_121_baselineData(I2C_HandleTypeDef * hi2c, uint8_t slave_address , uint8_t t , uint8_t * readData)
{
	HAL_StatusTypeDef status;
	if (t > 12)
	{
		status = HAL_ERROR;
	}
	else
	{
		status = readRegister8( hi2c, slave_address , MPR121_BASELINE_0 + t , readData );

		if(status == HAL_OK)
		{
			*readData = *readData << 2;
		}
	}

	return status;

}

uint16_t MPR_121_touched( I2C_HandleTypeDef * hi2c, uint8_t slave_address , uint16_t* touches)
{
	HAL_StatusTypeDef status;

	status = readRegister16( hi2c, slave_address , MPR121_TOUCHSTATUS_L , touches );

	return status;
}

HAL_StatusTypeDef MPR_121_setThresholds(I2C_HandleTypeDef * hi2c, uint8_t slave_address ,uint8_t touch, uint8_t release)
{
	HAL_StatusTypeDef status;

	uint8_t i=0;
	for (i = 0 ; i < 12 ; i++ )
	{
		status = writeRegister( hi2c , slave_address ,MPR121_TOUCHTH_0 + 2 * i, touch);

		if(status != HAL_OK )
		{
			break;
		}

		status = writeRegister( hi2c , slave_address ,MPR121_RELEASETH_0 + 2 * i, release);

		if(status != HAL_OK )
		{
			break;
		}
	}

	return status;
}

