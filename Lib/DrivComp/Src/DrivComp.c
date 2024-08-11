#include "DrivComp.h"

InterfaceTypes_t SupportInterface[SUPPORT_INTERFACE_COUNT];
DriverTypes_t SupportDrivers[SUPPORT_DRIVERS_COUNT];

int DrivCompInit(void)
{
	/* Get All interface */
	if (SpiInterfaceGetSupport(SupportInterface,
				   sizeof(SupportInterface) /
					   sizeof(SupportInterface[0]))) {
		return -1;
	}

	/* Get All Drivers */

	if (Lis3dhDriverGetSupport(SupportDrivers,
				   sizeof(SupportDrivers) /
					   sizeof(SupportDrivers[0]))) {
		return -1;
	}

	return 0;
}

void DrivCompGetInterface(InterfaceTypes_t **Interface, char *type)
{
	*Interface = NULL;
	for (size_t i = 0;
	     i < sizeof(SupportInterface) / sizeof(SupportInterface[0]); i++) {
		if (strcmp(SupportInterface[i].type, type) == 0) {
			*Interface = &SupportInterface[i];
			return;
		}
	}
}

void DrivCompGetDriver(DriverTypes_t **Drivers, char *type)
{
	*Drivers = NULL;
	for (size_t i = 0;
	     i < sizeof(SupportDrivers) / sizeof(SupportDrivers[0]); i++) {
		if (strcmp(SupportDrivers[i].type, type) == 0) {
			*Drivers = &SupportDrivers[i];
			return;
		}
	}
}

