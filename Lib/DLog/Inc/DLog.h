#ifndef __DLog_h__
#define __DLog_h__

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define CUSTOM_PRINT_MAX_LEN_MSG 128

#ifndef DLOG_SET_DEFAULT_LEVEL
#define DLOG_SET_DEFAULT_LEVEL 0x10
#endif

enum DLogLevelMask {
	DLog_PRINTF_EN = (uint8_t)(1 << 0),
	DLog_CustPRINTF_EN = (uint8_t)(1 << 1),
	DLog_Info_EN = (uint8_t)(1 << 2),
	DLog_Warning_EN = (uint8_t)(1 << 3),
	DLog_Errors_EN = (uint8_t)(1 << 4),
	Dlog_Default = (uint8_t)(DLog_PRINTF_EN) | (DLog_CustPRINTF_EN) |
		       (DLOG_SET_DEFAULT_LEVEL)
};

enum DlogLevelState { DLog_OFF, DLog_ON };

#define InfoMessage(fmt, ...)                                                \
	DMSGHandler(DLog_Info_EN, "info\t%s::%d\t" fmt "\r\n", __FUNCTION__, \
		    __LINE__, ##__VA_ARGS__)
#define WarningMessage(fmt, ...)                                     \
	DMSGHandler(DLog_Warning_EN, "Warning\t%s::%d\t" fmt "\r\n", \
		    __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ErrMessage(fmt, ...)                                       \
	DMSGHandler(DLog_Errors_EN, "Errors\t%s::%d\t" fmt "\r\n", \
		    __FUNCTION__, __LINE__, ##__VA_ARGS__)

int DLogInit(void);
void DLogSwitch(uint8_t LogLevel, uint8_t state);

#ifdef __GNUC__
__attribute__((format(gnu_printf, 2, 3))) uint16_t DMSGHandler(uint8_t LogLevel,
							       const char *tag,
							       ...);
#endif

#endif //__DLog_h__