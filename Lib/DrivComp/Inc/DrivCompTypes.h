#ifndef __DrivCompTypes_h__
#define __DrivCompTypes_h__

#include "main.h"
#include "ActuatMechTypes.h"
#include "FreeRTOS.h"
#include "event_groups.h"

typedef int(InitFaceFunc_t)(void *, uint32_t *);
typedef int(DeInitFaceFunc_t)(void *);
typedef int(SetFaceDefault_t)(char *, uint32_t *);
typedef void(InterfaceCallback_t)(EventGroupHandle_t);

typedef struct DrivGpioType {
	GPIO_TypeDef *Port;
	uint16_t Pin;
} DrivGpioType_t;

typedef struct CallbackType {
	EventGroupHandle_t xEventGroup;
	InterfaceCallback_t *_cb;
} CallbackType_t;

typedef struct InterfaceTypes {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	void *Handle;
	InitFaceFunc_t *Init;
	DeInitFaceFunc_t *DeInit;
	SetFaceDefault_t *SetDefault;
	DrivGpioType_t Gpio;
	CallbackType_t Callback;
} InterfaceTypes_t;

typedef int(InitDriverFunc_t)(InterfaceTypes_t *, uint32_t *);
typedef int(RequestDriverFunc_t)(InterfaceTypes_t *, uint32_t *, uint32_t *);
typedef int(SetDriverDefault_t)(InterfaceTypes_t *, uint32_t *);
typedef void(DriverRaWDataInterpreter_t)(uint32_t *, uint32_t *, uint32_t *);
typedef int(DriverCpltToCharInterpreter_t)(uint16_t, uint32_t *, char *,
					   char *);

typedef struct DriverTypes {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	char InterfaceSupport[ACTUAT_MECH_TYPE_NAME_SIZE];
	InitDriverFunc_t *Init;
	SetDriverDefault_t *SetDefault;
	RequestDriverFunc_t *Request;
	DriverRaWDataInterpreter_t *RaWDataInterpreter;
	DriverCpltToCharInterpreter_t *CpltToCharInterpreter;
} DriverTypes_t;

#endif //__DrivCompTypes_h__