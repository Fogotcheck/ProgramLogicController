#ifndef __Mech_h__
#define __Mech_h__

#include "ActuatMechTypes.h"
#include "ActuatMechEvent.h"
#include "main.h"
#include "DLog.h"
#include "DrivComp.h"

#define MECH_MODULE_NAME "MechTasks"
#define MECH_THR_STACK 512
#define MECH_THR_PRIORITIES (configMAX_PRIORITIES - 11)

typedef struct MechType {
	uint32_t ThrNum;
	InterfaceTypes_t *Interface;
	DriverTypes_t *Driver;

} MechPrivateHandleType_t;

int MechInit(void);
int MechConfigInitStart(uint32_t Ch, ConfigCh_t *Config);

#endif //__Mech_h__