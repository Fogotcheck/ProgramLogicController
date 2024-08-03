#include "Lsi3dh.h"

int Lis3dhCheckFree(DriverTypes_t *Driver);

static int Lis3dhInit(InterfaceTypes_t *Interface, uint32_t *Param);
static void Lis3dhSetInterfaceType(InterfaceTypes_t *Interface,
				   uint8_t *InterfaceType);
static inline int Lis3dhSpiInit(InterfaceTypes_t *Interface, uint32_t *Param);

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

	HAL_StatusTypeDef ret = HAL_SPI_Transmit(hspi, &TXbuf, 1, 10);
	if (ret) {
		return -1;
	}

	ret = HAL_SPI_Receive(hspi, &RXbuf, 1, 10);
	if (ret) {
		return -1;
	}
	if (RXbuf != LIS3DH_VAL_WHO_AM_I) {
		return -1;
	}

	return 0;
}