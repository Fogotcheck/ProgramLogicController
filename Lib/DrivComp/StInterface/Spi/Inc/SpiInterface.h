#ifndef __SpiInterface_h__
#define __SpiInterface_h__

#include <string.h>
#include <stdlib.h>
#include "DrivCompTypes.h"
#include "main.h"

#define SPI_INTERFACE_NAME "spi"

#define SPI_INTERFACE_DEF_PARAM_0 "spi0"
#define SPI_INTERFACE_DEF_PARAM_1 "spi1"

enum SPI_PARAM_DEF {
	INSTANCE,
	MODE,
	DIRECTION,
	DATASIZE,
	CLKPOLARITY,
	CLKPHASE,
	NSS,
	BAUDRATEPRESCALER,
	FIRSTBIT,
	TIMODE,
	CRCCALCULATION,
	CRCPOLYNOMIAL,
};

int SpiInterfaceGetSupport(InterfaceTypes_t *AllInterface,
			   uint16_t MaxInterfaceSize);

#endif //__SpiInterface_h__