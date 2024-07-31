#include "DrivComp.h"

InterfaceTypes_t SupportInterface[SUPPORT_INTERFACE_COUNT];

int DriversInit(void)
{
	if (SpiInterfaceGetSupport(SupportInterface,
				   sizeof(SupportInterface) /
					   sizeof(SupportInterface[0]))) {
		return -1;
	}

	return 0;
}
