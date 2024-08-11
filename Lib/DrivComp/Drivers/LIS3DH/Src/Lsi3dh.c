#include "Lsi3dh.h"

int Lis3dhCheckFree(DriverTypes_t *Driver);

static int Lis3dhInit(InterfaceTypes_t *Interface, uint32_t *Param);
static void Lis3dhSetInterfaceType(InterfaceTypes_t *Interface,
				   uint8_t *InterfaceType);
static inline int Lis3dhSpiInit(InterfaceTypes_t *Interface, uint32_t *Param);
static int Lis3dhSetDefaultParam(InterfaceTypes_t *Interface, uint32_t *Param);
static inline void Lis3dhSpiSetDefault(uint32_t *Param);
static int Lis3dhRequest(InterfaceTypes_t *Interface, uint32_t *Param,
			 uint32_t *RxBuf);
static inline int Lis3dhSpiRequest(InterfaceTypes_t *Interface, uint32_t *Param,
				   uint32_t *RxBuf);
static inline void Lis3dhRawDataInterpreter(uint32_t *Param, uint32_t *RawData,
					    uint32_t *CpltData);
static inline int Lis3dhCpltToCharInterpreter(uint16_t count,
					      uint32_t *CpltData, char *name,
					      char *data);
static inline int Lis3dhRebootMem(InterfaceTypes_t *Interface);

int Lis3dhDriverGetSupport(DriverTypes_t *AllDrivers, uint16_t MaxDrivers)
{
	DriverTypes_t *Driver = NULL;
	for (uint16_t i = 0; i < MaxDrivers; i++) {
		if (Lis3dhCheckFree(&AllDrivers[i])) {
			continue;
		} else {
			Driver = &AllDrivers[i];
			break;
		}
	}
	if (Driver == NULL) {
		return -1;
	}
	memcpy(Driver->type, LIS3DH_NAME, sizeof(LIS3DH_NAME));
	memcpy(Driver->InterfaceSupport, LIS3DH_SUPPORT_INTERFACE_SPI,
	       sizeof(LIS3DH_SUPPORT_INTERFACE_SPI));
	strcat(Driver->InterfaceSupport, LIS3DH_SUPPORT_INTERFACE_I2C);

	Driver->Init = Lis3dhInit;
	Driver->SetDefault = Lis3dhSetDefaultParam;
	Driver->Request = Lis3dhRequest;
	Driver->RaWDataInterpreter = Lis3dhRawDataInterpreter;
	Driver->CpltToCharInterpreter = Lis3dhCpltToCharInterpreter;
	return 0;
}

int Lis3dhCheckFree(DriverTypes_t *Driver)
{
	for (uint8_t j = 0; j < sizeof(Driver->type) / sizeof(Driver->type[0]);
	     j++) {
		if (Driver->type[j] != 0) {
			return -1;
		}
	}
	return 0;
}

static int Lis3dhInit(InterfaceTypes_t *Interface, uint32_t *Param)
{
	uint8_t InterfaceType = LIS3DH_NULL_TYPE;
	Lis3dhSetInterfaceType(Interface, &InterfaceType);

	switch (InterfaceType) {
	case LIS3DH_SPI_TYPE:
		if (Lis3dhSpiInit(Interface, Param)) {
			return -1;
		}
		break;
	case LIS3DH_I2C_TYPE:
		break;
	default:
		return -1;
	}

	return 0;
}

static void Lis3dhSetInterfaceType(InterfaceTypes_t *Interface,
				   uint8_t *InterfaceType)
{
	if (strstr(Interface->type, LIS3DH_SUPPORT_INTERFACE_SPI) != NULL) {
		*InterfaceType = LIS3DH_SPI_TYPE;
		return;
	}
	if (strstr(Interface->type, LIS3DH_SUPPORT_INTERFACE_SPI) != NULL) {
		*InterfaceType = LIS3DH_I2C_TYPE;
		return;
	}
}

static inline int Lis3dhSpiInit(InterfaceTypes_t *Interface, uint32_t *Param)
{
	uint8_t TXbuf = READ | LIS3DH_ADD_REFERENCE;
	uint8_t RXbuf = 0;
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)Interface->Handle;
	if (hspi == NULL) {
		return -1;
	}
	if (Interface->Gpio.Port == NULL) {
		return -1;
	}
	HAL_StatusTypeDef ret = 0;
	HAL_GPIO_WritePin(Interface->Gpio.Port, Interface->Gpio.Pin, 1);
	HAL_GPIO_WritePin(Interface->Gpio.Port, Interface->Gpio.Pin, 0);

	TXbuf = READ | LIS3DH_ADD_WHO_AM_I;
	ret = HAL_SPI_Transmit(hspi, &TXbuf, 1, 10);
	if (ret) {
		return -1;
	}

	ret = HAL_SPI_Receive(hspi, &RXbuf, 1, 10);
	if (ret) {
		return -1;
	}
	HAL_GPIO_WritePin(Interface->Gpio.Port, Interface->Gpio.Pin, 1);
	if (RXbuf != LIS3DH_VAL_WHO_AM_I) {
		return -1;
	}

	for (uint8_t i = 0; i < ACTUAT_MECH_TYPE_PARAM_SIZE; i++) {
		Lsi3dhParamType_t *RegParam = (Lsi3dhParamType_t *)&Param[i];
		if (RegParam->type == LIS3DH_INIT_PARAM) {
			RegParam->addr |= WRITE;
			HAL_GPIO_WritePin(Interface->Gpio.Port,
					  Interface->Gpio.Pin, 0);
			ret = HAL_SPI_Transmit(hspi, &RegParam->addr, 1, 10);
			if (ret) {
				return -1;
			}
			ret = HAL_SPI_Transmit(hspi, (uint8_t *)&RegParam->data,
					       1, 10);
			HAL_GPIO_WritePin(Interface->Gpio.Port,
					  Interface->Gpio.Pin, 1);
			if (ret) {
				return -1;
			}
		}
	}

	HAL_GPIO_WritePin(Interface->Gpio.Port, Interface->Gpio.Pin, 0);
	ret = HAL_SPI_Transmit(hspi, &TXbuf, 1, 10);
	if (ret) {
		return -1;
	}
	RXbuf = 0;
	ret = HAL_SPI_Receive(hspi, &RXbuf, 1, 10);
	HAL_GPIO_WritePin(Interface->Gpio.Port, Interface->Gpio.Pin, 1);
	if (ret) {
		return -1;
	}
	if (RXbuf != LIS3DH_VAL_WHO_AM_I) {
		return -1;
	}
	return 0;
}

static int Lis3dhSetDefaultParam(InterfaceTypes_t *Interface, uint32_t *Param)
{
	uint8_t InterfaceType = LIS3DH_NULL_TYPE;
	Lis3dhSetInterfaceType(Interface, &InterfaceType);

	switch (InterfaceType) {
	case LIS3DH_SPI_TYPE:
		Lis3dhSpiSetDefault(Param);
		break;
	case LIS3DH_I2C_TYPE:
		break;
	default:
		return -1;
	}

	return 0;
}

static inline void Lis3dhSpiSetDefault(uint32_t *Param)
{
	Param[LIS3DH_DEF_SYSTEM_INIT_0] =
		(LIS3DH_INIT_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_CTRL_REG5 << LIS3DH_ADDR) |
		(0b10000000 << LIS3DH_DATA); // reboot mem
	Param[LIS3DH_DEF_SYSTEM_INIT_1] =
		(LIS3DH_INIT_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_TEMP_CFG_REG << LIS3DH_ADDR) |
		(0b11000000 << LIS3DH_DATA); // en Temperature sensor
	Param[LIS3DH_DEF_SYSTEM_INIT_2] =
		(LIS3DH_INIT_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_CTRL_REG1 << LIS3DH_ADDR) |
		(0b10010111
		 << LIS3DH_DATA); // Turn normal mode and 1.344kHz data rate on
	Param[LIS3DH_DEF_SYSTEM_INIT_3] =
		(LIS3DH_INIT_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_CTRL_REG4 << LIS3DH_ADDR) |
		(0b10000000
		 << LIS3DH_DATA); // Turn block data update on (for temperature sensing)

	Param[LIS3DH_DEF_REQUEST_0] =
		(LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_STATUS_REG_AUX << LIS3DH_ADDR); // request status

	Param[LIS3DH_DEF_REQUEST_1] =
		(LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_OUT_X_H << LIS3DH_ADDR); // request adc1 (xh)
	Param[LIS3DH_DEF_REQUEST_2] =
		(LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_OUT_X_L << LIS3DH_ADDR); // request adc1 (xl)

	Param[LIS3DH_DEF_REQUEST_3] =
		(LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_OUT_Y_H << LIS3DH_ADDR); // request adc1 (yh)
	Param[LIS3DH_DEF_REQUEST_4] =
		(LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_OUT_Y_L << LIS3DH_ADDR); // request adc1 (yl)

	Param[LIS3DH_DEF_REQUEST_5] =
		(LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_OUT_Z_H << LIS3DH_ADDR); // request adc1 (zh)
	Param[LIS3DH_DEF_REQUEST_6] =
		(LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
		(LIS3DH_ADD_OUT_Z_L << LIS3DH_ADDR); // request adc1 (zl)
}

static int Lis3dhRequest(InterfaceTypes_t *Interface, uint32_t *Param,
			 uint32_t *RxBuf)
{
	uint8_t InterfaceType = LIS3DH_NULL_TYPE;
	Lis3dhSetInterfaceType(Interface, &InterfaceType);

	switch (InterfaceType) {
	case LIS3DH_SPI_TYPE:
		if (Lis3dhSpiRequest(Interface, Param, RxBuf)) {
			return -1;
		}
		// todo don't update data
		// Lis3dhRebootMem(Interface);
		break;
	case LIS3DH_I2C_TYPE:
		break;
	default:
		return -1;
	}
	return 0;
}

static inline int Lis3dhSpiRequest(InterfaceTypes_t *Interface, uint32_t *Param,
				   uint32_t *RxBuf)
{
	int ret = 0;
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)Interface->Handle;
	if (hspi == NULL) {
		return -1;
	}

	for (uint8_t i = 0; i < ACTUAT_MECH_TYPE_PARAM_SIZE; i++) {
		Lsi3dhParamType_t *RegParam = (Lsi3dhParamType_t *)&Param[i];
		if (RegParam->type == LIS3DH_REQUEST_PARAM) {
			uint8_t addr = RegParam->addr | READ;
			HAL_GPIO_WritePin(Interface->Gpio.Port,
					  Interface->Gpio.Pin, 0);
			ret = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 10);
			if (ret) {
				return -1;
			}

			ret = HAL_SPI_Receive(hspi, (uint8_t *)&RxBuf[i], 1,
					      10);
			HAL_GPIO_WritePin(Interface->Gpio.Port,
					  Interface->Gpio.Pin, 1);
			if (ret) {
				return -1;
			}
		}
	}

	if (Interface->Callback._cb != NULL) {
		Interface->Callback._cb(Interface->Callback.xEventGroup);
	}

	return 0;
}

static inline int Lis3dhRebootMem(InterfaceTypes_t *Interface)
{
	int ret = 0;
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)Interface->Handle;
	uint8_t addr = LIS3DH_ADD_FIFO_SRC_REG | WRITE;
	uint8_t data = 0b00100000;
	HAL_GPIO_WritePin(Interface->Gpio.Port, Interface->Gpio.Pin, 0);
	ret = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 10);
	ret = HAL_SPI_Transmit(hspi, (uint8_t *)&data, 1, 10);
	HAL_GPIO_WritePin(Interface->Gpio.Port, Interface->Gpio.Pin, 1);
	return ret;
}

static inline void Lis3dhRawDataInterpreter(uint32_t *Param, uint32_t *RawData,
					    uint32_t *CpltData)
{
	for (uint8_t i = 0; i < ACTUAT_MECH_TYPE_PARAM_SIZE; i++) {
		switch (Param[i]) {
		case (LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
			(LIS3DH_ADD_STATUS_REG_AUX << LIS3DH_ADDR): {
			CpltData[LIS3DH_STATUS] = RawData[i];
			break;
		}
		case (LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
			(LIS3DH_ADD_OUT_X_H << LIS3DH_ADDR): {
			CpltData[LIS3DH_OUT_X] &= ~(0xff << 8);
			CpltData[LIS3DH_OUT_X] |= (RawData[i] << 8);
			break;
		}
		case (LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
			(LIS3DH_ADD_OUT_X_L << LIS3DH_ADDR): {
			CpltData[LIS3DH_OUT_X] &= ~(0xff << 0);
			CpltData[LIS3DH_OUT_X] |= (RawData[i] << 0);
			break;
		}

		case (LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
			(LIS3DH_ADD_OUT_Y_H << LIS3DH_ADDR): {
			CpltData[LIS3DH_OUT_Y] &= ~(0xff << 8);
			CpltData[LIS3DH_OUT_Y] |= (RawData[i] << 8);
			break;
		}
		case (LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
			(LIS3DH_ADD_OUT_Y_L << LIS3DH_ADDR): {
			CpltData[LIS3DH_OUT_Y] &= ~(0xff << 0);
			CpltData[LIS3DH_OUT_Y] |= (RawData[i] << 0);
			break;
		}

		case (LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
			(LIS3DH_ADD_OUT_Z_H << LIS3DH_ADDR): {
			CpltData[LIS3DH_OUT_Z] &= ~(0xff << 8);
			CpltData[LIS3DH_OUT_Z] |= (RawData[i] << 8);
			break;
		}
		case (LIS3DH_REQUEST_PARAM << LIS3DH_TYPE) |
			(LIS3DH_ADD_OUT_Z_L << LIS3DH_ADDR): {
			CpltData[LIS3DH_OUT_Z] &= ~(0xff << 0);
			CpltData[LIS3DH_OUT_Z] |= (RawData[i] << 0);
			break;
		}

		default:
			break;
		}
	}
}

static inline int Lis3dhCpltToCharInterpreter(uint16_t count,
					      uint32_t *CpltData, char *name,
					      char *data)
{
	switch (count) {
	case LIS3DH_STATUS:
		strcat(name, "/Status");
		break;
	case LIS3DH_OUT_X:
		strcat(name, "/X");
		break;
	case LIS3DH_OUT_Y:
		strcat(name, "/Y");
		break;
	case LIS3DH_OUT_Z:
		strcat(name, "/Z");
		break;
	default:
		return -1;
	}
	itoa(*CpltData, data, 10);
	return 0;
}