#ifndef __DrivComp_h__
#define __DrivComp_h__

#include "DrivCompTypes.h"
#include "SpiInterface.h"
#include "Lsi3dh.h"

#define SUPPORT_INTERFACE_COUNT 5
#define SUPPORT_DRIVERS_COUNT 10

int DrivCompInit(void);
void DrivCompGetInterface(InterfaceTypes_t **Interface, char *type);
void DrivCompGetDriver(DriverTypes_t **Drivers, char *type);

#endif //__DrivComp_h__