#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Макрос выхода из функции с печатью ошибки
 *
 */
#define ERR_EXIT(format, ...)          \
	do                                 \
	{                                  \
		printf(format, ##__VA_ARGS__); \
		ret = -1;                      \
		goto end;                      \
	} while (0)

/**
 * @brief Макрос выхода из main с печатью ошибки
 *
 */
#define ERR_MAIN_EXIT(format, ...)     \
	do                                 \
	{                                  \
		printf(format, ##__VA_ARGS__); \
		ret = EXIT_FAILURE;            \
		goto end;                      \
	} while (0)
