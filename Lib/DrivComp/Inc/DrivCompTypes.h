#ifndef __DrivCompTypes_h__
#define __DrivCompTypes_h__

#include "main.h"
#include "ActuatMechTypes.h"

typedef int(InitFunc_t)(void *, uint32_t *);
typedef int(DeInitFunc_t)(void *);
typedef int(SetDefault_t)(char *, uint32_t *);

typedef struct InterfaceTypes {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	void *Handle;
	InitFunc_t *Init;
	DeInitFunc_t *DeInit;
	SetDefault_t *SetDefault;
} InterfaceTypes_t;

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

#endif //__DrivCompTypes_h__