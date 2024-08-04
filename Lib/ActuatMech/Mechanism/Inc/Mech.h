#ifndef __Mech_h__
#define __Mech_h__

#include "ActuatMechTypes.h"
#include "ActuatMechEvent.h"
#include "main.h"
#include "DLog.h"
#include "DrivComp.h"

#define MECH_MODULE_NAME "MechTasks"
#define MECH_THR_STACK 728
#define MECH_THR_PRIORITIES (configMAX_PRIORITIES - 11)

#define MECH_RAW_BUF_SIZE_IN_WORD 32
#define MECH_CPLT_BUF_SIZE_IN_WORD 32
#define MECH_FILTER_CONF_BUF_SIZE_IN_WORD 32

typedef struct MechPrivateBuf {
	uint32_t Raw[MECH_RAW_BUF_SIZE_IN_WORD];
	uint32_t Prev[MECH_RAW_BUF_SIZE_IN_WORD];
	uint32_t Cplt[MECH_CPLT_BUF_SIZE_IN_WORD];
	uint32_t FiltrConf[MECH_FILTER_CONF_BUF_SIZE_IN_WORD];
} MechPrivateBuf_t;

typedef struct MechType {
	uint32_t ThrNum;
	InterfaceTypes_t *Interface;
	DriverTypes_t *Driver;
	MechPrivateBuf_t *Buf;
} MechPrivateHandleType_t;

int MechInit(void);
int MechConfigInitStart(uint32_t Ch, ConfigCh_t *Config);

#endif //__Mech_h__