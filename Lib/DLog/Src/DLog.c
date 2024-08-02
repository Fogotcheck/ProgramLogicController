#include "DLog.h"

static uint8_t LogLevelMask = Dlog_Default;
extern UART_HandleTypeDef huart3;
UART_HandleTypeDef *DLogUart = &huart3;
SemaphoreHandle_t DLogMutex = NULL;
char DLogBuf[CUSTOM_PRINT_MAX_LEN_MSG] = { 0 };

int PrintfUartProvider(uint8_t data);
int CustVsnprintf(char *str, size_t size, const char *format, va_list ap);
void DSendMSG(int len);

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{ /* поведение функции не отслеживается в дальнейшем */
	int ret = PrintfUartProvider(ch);
	return ret == 0 ? ch : EOF;
}

int DLogInit(void)
{
	DLogMutex = xSemaphoreCreateMutex();
	if (DLogMutex == NULL) {
		return -1;
	}

	InfoMessage("DLogLevel::0x%x", LogLevelMask);
	return 0;
}

void DLogSwitch(uint8_t LogLevel, uint8_t state)
{
	LogLevelMask &= ~LogLevel;
	if (state == DLog_ON) {
		LogLevelMask |= LogLevel;
	}
}

int PrintfUartProvider(uint8_t data)
{
	if ((LogLevelMask & DLog_PRINTF_EN) == 0) {
		return 0;
	}

	HAL_StatusTypeDef ret;
	ret = HAL_UART_Transmit(DLogUart, &data, 1, 5);
	if (ret != HAL_OK) {
		return 1;
	}
	return 0;
}

#ifdef __GNUC__
__attribute__((format(gnu_printf, 2, 3))) uint16_t DMSGHandler(uint8_t LogLevel,
							       const char *tag,
							       ...)
{
	LogLevel &= ~((DLog_PRINTF_EN) | (DLog_CustPRINTF_EN));
	if ((LogLevel & LogLevelMask) == 0) {
		return 0;
	}

	if (osSemaphoreAcquire(DLogMutex, portMAX_DELAY) != osOK) {
		return 0;
	}
	memset(DLogBuf, 0, sizeof(DLogBuf));
	va_list args;
	va_start(args, tag);
	int len = vsiprintf(DLogBuf, tag, args);

	va_end(args);
	DSendMSG(len);
	if (osSemaphoreRelease(DLogMutex) != osOK) {
		return 0;
	}
	return len >= CUSTOM_PRINT_MAX_LEN_MSG ? CUSTOM_PRINT_MAX_LEN_MSG : len;
}
#endif



void DSendMSG(int len)
{
	if ((DLog_CustPRINTF_EN & LogLevelMask)) {
		HAL_UART_Transmit(DLogUart, (uint8_t *)DLogBuf, len, 10);
	}
}