#ifndef __DrivCompTypes_h__
#define __DrivCompTypes_h__

#include "main.h"
#include "ActuatMechTypes.h"
#include "FreeRTOS.h"
#include "event_groups.h"

typedef int(InitFaceFunc_t)(void *, uint32_t *);
typedef int(DeInitFaceFunc_t)(void *);
typedef int(SetFaceDefault_t)(char *, uint32_t *);

typedef struct InterfaceTypes {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	void *Handle;
	InitFaceFunc_t *Init;
	DeInitFaceFunc_t *DeInit;
	SetFaceDefault_t *SetDefault;
} InterfaceTypes_t;

typedef int(InitDriverFunc_t)(InterfaceTypes_t *, uint32_t *);
typedef int(RequestDriverFunc_t)(InterfaceTypes_t *, uint32_t *, uint32_t *);
typedef void(CallBackFunc_t)(EventGroupHandle_t, EventBits_t);
typedef int(SetDriverDefault_t)(InterfaceTypes_t *, uint32_t *);

typedef struct DriverTypes {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	char InterfaceSupport[ACTUAT_MECH_TYPE_NAME_SIZE];
	InitDriverFunc_t *Init;
	SetDriverDefault_t *SetDefault;
	RequestDriverFunc_t *Request;
} DriverTypes_t;

#endif //__DrivCompTypes_h__