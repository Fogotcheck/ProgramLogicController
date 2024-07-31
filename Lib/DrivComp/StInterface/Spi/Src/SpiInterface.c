#include "SpiInterface.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

void *SpiHandle[2] = { &hspi1, &hspi2 };

int SpiInterfaceCheckFree(InterfaceTypes_t *Interface);
int SpiInterfaceInit(InterfaceTypes_t *Interface, uint16_t count);

static void SpiSetHandle(void **Handle, SPI_HandleTypeDef *hspi);
static int SpiInit(void *Handle, __attribute__((unused)) uint32_t *Param);
static int SpiDeInit(void *Handle);
static int SpiSetDefaultParam(char *type, uint32_t *Param);

int SpiInterfaceGetSupport(InterfaceTypes_t *AllInterface,
			   uint16_t MaxInterfaceSize)
{
	uint16_t count = 0;
	for (uint16_t i = 0; i < MaxInterfaceSize; i++) {
		if (SpiInterfaceCheckFree(&AllInterface[i])) {
			continue;
		}
		do {
			if (count >= sizeof(SpiHandle) / sizeof(SpiHandle[0])) {
				return 0;
			}
			if (SpiInterfaceInit(&AllInterface[i], count)) {
				return -1;
			}
			count++;
		} while (0);
	}

	return -1;
}

int SpiInterfaceCheckFree(InterfaceTypes_t *Interface)
{
	for (uint8_t j = 0;
	     j < sizeof(Interface->type) / sizeof(Interface->type[0]); j++) {
		if (Interface->type[j] != 0) {
			return -1;
		}
	}
	return 0;
}

int SpiInterfaceInit(InterfaceTypes_t *Interface, uint16_t count)
{
	if (count >= sizeof(SpiHandle) / sizeof(SpiHandle[0])) {
		return -1;
	}

	memcpy(Interface->type, SPI_INTERFACE_NAME, sizeof(SPI_INTERFACE_NAME));
	char tmp[sizeof(Interface->type)] = { 0 };
	itoa(count, tmp, 10);
	strcat(Interface->type, tmp);
	SpiSetHandle(&Interface->Handle, SpiHandle[count]);
	Interface->Init = SpiInit;
	Interface->DeInit = SpiDeInit;
	Interface->SetDefault = SpiSetDefaultParam;
	return 0;
}

static void SpiSetHandle(void **Handle, SPI_HandleTypeDef *hspi)
{
	*Handle = hspi;
}

static int SpiInit(void *Handle, uint32_t *Param)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)Handle;

	hspi->Instance = (SPI_TypeDef *)Param[INSTANCE];
	hspi->Init.Mode = Param[MODE];
	hspi->Init.Direction = Param[DIRECTION];
	hspi->Init.DataSize = Param[DATASIZE];
	hspi->Init.CLKPolarity = Param[CLKPOLARITY];
	hspi->Init.CLKPhase = Param[CLKPHASE];
	hspi->Init.NSS = Param[NSS];
	hspi->Init.BaudRatePrescaler = Param[BAUDRATEPRESCALER];
	hspi->Init.FirstBit = Param[FIRSTBIT];
	hspi->Init.TIMode = Param[TIMODE];
	hspi->Init.CRCCalculation = Param[CRCCALCULATION];
	hspi->Init.CRCPolynomial = Param[CRCPOLYNOMIAL];
	if (HAL_SPI_Init(hspi) != HAL_OK) {
		return -1;
	}
	return 0;
}

static int SpiDeInit(void *Handle)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)Handle;
	if (HAL_SPI_DeInit(hspi)) {
		return -1;
	}
	return 0;
}

static int SpiSetDefaultParam(char *type, uint32_t *Param)
{
	int ret = 0;
	if (strcmp(type, SPI_INTERFACE_DEF_PARAM_0) == 0) {
		Param[INSTANCE] = (uint32_t)SPI1;
		ret = 1;
	}
	if (strcmp(type, SPI_INTERFACE_DEF_PARAM_1) == 0) {
		Param[INSTANCE] = (uint32_t)SPI2;
		ret = 1;
	}
	if (ret) {
		Param[MODE] = SPI_MODE_MASTER;
		Param[DIRECTION] = SPI_DIRECTION_2LINES;
		Param[DATASIZE] = SPI_DATASIZE_8BIT;
		Param[CLKPOLARITY] = SPI_POLARITY_LOW;
		Param[CLKPHASE] = SPI_PHASE_1EDGE;
		Param[NSS] = SPI_NSS_HARD_OUTPUT;
		Param[BAUDRATEPRESCALER] = SPI_BAUDRATEPRESCALER_8;
		Param[FIRSTBIT] = SPI_FIRSTBIT_MSB;
		Param[TIMODE] = SPI_TIMODE_DISABLE;
		Param[CRCCALCULATION] = SPI_CRCCALCULATION_DISABLE;
		Param[CRCPOLYNOMIAL] = 10;

		return 0;
	}

	return -1;
}
