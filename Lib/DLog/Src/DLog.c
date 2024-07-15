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
	int len = CustVsnprintf(DLogBuf, sizeof(DLogBuf), tag, args);
	va_end(args);
	DSendMSG(len);
	if (osSemaphoreRelease(DLogMutex) != osOK) {
		return 0;
	}
	return len >= CUSTOM_PRINT_MAX_LEN_MSG ? CUSTOM_PRINT_MAX_LEN_MSG : len;
}
#endif

int CustVsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	int written = 0;
	char *p = str;

	while (*format != '\0' && size > 0) {
		if (*format == '%') {
			format++;
			switch (*format) {
			case 's': {
				char *s = va_arg(ap, char *);
				size_t len = 0;
				while (s[len] != '\0') {
					len++;
				}
				if (len < size) {
					for (size_t i = 0; i < len; i++) {
						*p++ = s[i];
					}
					size -= len;
				} else {
					for (size_t i = 0; i < size - 1; i++) {
						*p++ = s[i];
					}
					size = 0;
				}
				written += len;
				break;
			}
			case 'd': {
				int d = va_arg(ap, int);
				char buf[32];
				int len = 0;
				if (d < 0) {
					buf[len++] = '-';
					d = -d;
				}
				do {
					buf[len++] = '0' + (d % 10);
					d /= 10;
				} while (d > 0);
				if (len < (int)size) {
					for (int i = len - 1; i >= 0; i--) {
						*p++ = buf[i];
					}
					size -= len;
				} else {
					for (int i = len - 1;
					     i >= len - (int)size + 1; i--) {
						*p++ = buf[i];
					}
					size = 0;
				}
				written += len;
				break;
			}
			case 'l': {
				format++;
				switch (*format) {
				case 'd': {
					long ld = va_arg(ap, long);
					char buf[32];
					int len = 0;
					if (ld < 0) {
						buf[len++] = '-';
						ld = -ld;
					}
					do {
						buf[len++] = '0' + (ld % 10);
						ld /= 10;
					} while (ld > 0);
					if (len < (int)size) {
						for (int i = len - 1; i >= 0;
						     i--) {
							*p++ = buf[i];
						}
						size -= len;
					} else {
						for (int i = len - 1;
						     i >= len - (int)size + 1;
						     i--) {
							*p++ = buf[i];
						}
						size = 0;
					}
					written += len;
					break;
				}
				case 'x': {
					unsigned long lx =
						va_arg(ap, unsigned long);
					char buf[32];
					int len = 0;
					do {
						int digit = lx & 0xF;
						buf[len++] =
							(digit < 10) ?
								'0' + digit :
								'a' + digit -
									10;
						lx >>= 4;
					} while (lx > 0);
					if (len < (int)size) {
						for (int i = len - 1; i >= 0;
						     i--) {
							*p++ = buf[i];
						}
						size -= len;
					} else {
						for (int i = len - 1;
						     i >= len - (int)size + 1;
						     i--) {
							*p++ = buf[i];
						}
						size = 0;
					}
					written += len;
					break;
				}
				default:
					break;
				}
				break;
			}
			case 'h': {
				format++;
				if (*format == 'h') {
					format++;
					unsigned char hx =
						(unsigned char)va_arg(ap, int);
					char buf[8];
					int len = 0;
					do {
						int digit = hx & 0xF;
						buf[len++] =
							(digit < 10) ?
								'0' + digit :
								'a' + digit -
									10;
						hx >>= 4;
					} while (hx > 0);
					if (len < (int)size) {
						for (int i = len - 1; i >= 0;
						     i--) {
							*p++ = buf[i];
						}
						size -= len;
					} else {
						for (int i = len - 1;
						     i >= len - (int)size + 1;
						     i--) {
							*p++ = buf[i];
						}
						size = 0;
					}
					written += len;
				}
				break;
			}
			case 'x': {
				unsigned int x = va_arg(ap, unsigned int);
				char buf[16];
				int len = 0;
				do {
					int digit = x & 0xF;
					buf[len++] = (digit < 10) ?
							     '0' + digit :
							     'a' + digit - 10;
					x >>= 4;
				} while (x > 0);
				if (len < (int)size) {
					for (int i = len - 1; i >= 0; i--) {
						*p++ = buf[i];
					}
					size -= len;
				} else {
					for (int i = len - 1;
					     i >= len - (int)size + 1; i--) {
						*p++ = buf[i];
					}
					size = 0;
				}
				written += len;
				break;
			}
			default:
				break;
			}
		} else {
			if (size > 1) {
				*p++ = *format;
				size--;
			}
			written++;
		}
		format++;
	}

	if (size > 0) {
		*p = '\0';
	}

	return written;
}

void DSendMSG(int len)
{
	if ((DLog_CustPRINTF_EN & LogLevelMask)) {
		HAL_UART_Transmit(DLogUart, (uint8_t *)DLogBuf, len, 5);
	}
}