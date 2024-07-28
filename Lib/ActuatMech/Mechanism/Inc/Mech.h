#ifndef __Mech_h__
#define __Mech_h__

#include "ActuatMechTypes.h"
#include "ActuatMechEvent.h"
#include "main.h"
#include "DLog.h"

#define MECH_MODULE_NAME "MechTasks"
#define MECH_THR_STACK 512
#define MECH_THR_PRIORITIES (configMAX_PRIORITIES - 11)

int MechInit(void);
int MechConfigInitStart(uint32_t Ch, ConfigCh_t *Config);

#endif //__Mech_h__