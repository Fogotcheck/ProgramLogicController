#ifndef __ActuatMechTypes_h__
#define __ActuatMechTypes_h__

#define ACTUAT_MECH_TYPE_NAME_SIZE 10
#define ACTUAT_MECH_TYPE_PARAM_SIZE 10

typedef struct ConfInterface {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	uint32_t param[ACTUAT_MECH_TYPE_PARAM_SIZE];
} ConfInterface_t;

typedef struct ConfDriver {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	uint32_t param[ACTUAT_MECH_TYPE_PARAM_SIZE];
} ConfDriver_t;

typedef struct ConfPostHandle {
	uint8_t filter : 1;
	uint8_t revers : 1;
	uint8_t fourier : 1;
} ConfPostHandle_t;

typedef struct ConfCh {
	ConfInterface_t Interface;
	ConfDriver_t Driver;
	ConfPostHandle_t PostHandle;
} ConfigCh_t;

#endif //__ActuatMechTypes_h__