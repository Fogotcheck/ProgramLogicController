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

static inline int Lis3dhSpiInit(InterfaceTypes_t *Interface,
				__attribute__((unused)) uint32_t *Param)
{
	uint8_t TXbuf = READ | LIS3DH_ADD_WHO_AM_I;
	uint8_t RXbuf = 0;
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)Interface->Handle;
	if (hspi == NULL) {
		return -1;
	}
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(hspi, &TXbuf, 1, 10);
	if (ret) {
		return -1;
	}

	ret = HAL_SPI_Receive(hspi, &RXbuf, 1, 10);
	if (ret) {
		return -1;
	}
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
	if (RXbuf != LIS3DH_VAL_WHO_AM_I) {
		return -1;
	}
	uint8_t buf[2];

	// Turn normal mode and 1.344kHz data rate on
	buf[0] = LIS3DH_ADD_CTRL_REG1;
	buf[1] = 0x97;
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);
	ret = HAL_SPI_Transmit(hspi, buf, 2, 10);
	if (ret) {
		return -1;
	}
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
	HAL_Delay(5);
	// Turn block data update on (for temperature sensing)
	buf[0] = LIS3DH_ADD_CTRL_REG4;
	buf[1] = 0x80;
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);
	ret = HAL_SPI_Transmit(hspi, buf, 2, 10);
	if (ret) {
		return -1;
	}
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
	HAL_Delay(5);

	// Turn auxiliary ADC on
	buf[0] = LIS3DH_ADD_TEMP_CFG_REG;
	buf[1] = 0xC0;
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);
	ret = HAL_SPI_Transmit(hspi, buf, 2, 10);
	if (ret) {
		return -1;
	}
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
	HAL_Delay(5);

	for (uint8_t i = 0; i < ACTUAT_MECH_TYPE_PARAM_SIZE; i++) {
		Lsi3dhParamType_t *RegParam = (Lsi3dhParamType_t *)&Param[i];
		if (RegParam->type == LIS3DH_INIT_PARAM) {
			RegParam->addr |= WRITE;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
			ret = HAL_SPI_Transmit(hspi, &RegParam->addr, 1, 10);
			if (ret) {
				return -1;
			}
			ret = HAL_SPI_Transmit(hspi, (uint8_t *)&RegParam->data,
					       1, 10);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
			if (ret) {
				return -1;
			}
			HAL_Delay(2);
		}
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
	ret = HAL_SPI_Transmit(hspi, &TXbuf, 1, 10);
	if (ret) {
		return -1;
	}
	RXbuf = 0;
	ret = HAL_SPI_Receive(hspi, &RXbuf, 1, 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
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
		(0b01000000 << LIS3DH_DATA); // en Temperature sensor

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
		if (Lis3dhSpiRequest(Interface, Param, RxBuf))
			return -1;
		break;
	case LIS3DH_I2C_TYPE:
		break;
	default:
		return -1;
	}
	return 0;
}

static inline int Lis3dhSpiRequest(InterfaceTypes_t *Interface,
				   __attribute__((unused)) uint32_t *Param,
				   __attribute__((unused)) uint32_t *RxBuf)
{
	int ret = 0;
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)Interface->Handle;
	if (hspi == NULL) {
		return -1;
	}

	uint8_t TmpBuf[ACTUAT_MECH_TYPE_PARAM_SIZE] = { 0 };
	for (uint8_t i = 0; i < ACTUAT_MECH_TYPE_PARAM_SIZE; i++) {
		Lsi3dhParamType_t *RegParam = (Lsi3dhParamType_t *)&Param[i];
		if (RegParam->type == LIS3DH_REQUEST_PARAM) {
			RegParam->addr |= READ;
			uint8_t addr = RegParam->addr;
			HAL_Delay(5);
			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 0);
			ret = HAL_SPI_Transmit(hspi, (uint8_t *)&addr, 1, 10);
			if (ret) {
				return -1;
			}

			ret = HAL_SPI_Receive(hspi, (uint8_t *)&TmpBuf[i], 1,
					      10);
			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, 1);
			if (ret) {
				return -1;
			}
		}
	}

	return 0;
}