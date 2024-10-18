#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "uthash.h"
//Коды ошибок всегда трактуются одинаково:
//0  - успешно
//-1 - ошибка

#define NUM_THREADS 2      // Количество потоков

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

#define OS_FREE(x)    \
	do                \
	{                 \
		if (x)        \
		{             \
			free(x);  \
			x = NULL; \
		}             \
	} while (0)

#define OS_JSON_FREE(x)      \
	do {                     \
		if (x)               \
		{                    \
			cJSON_Delete(x); \
			x = NULL;        \
		}                    \
	} while (0)

struct uthash_entry_t
{
    int version[4];
    const char *release;
    cJSON *obj;
    UT_hash_handle hh; /* makes this structure hashable */
    char name[];       /* key (name+arch) */
};

struct uthash_data_t
{
    struct uthash_entry_t *users;
    char *buf;
    size_t buf_size;
    struct uthash_data_t *other;
    cJSON *array;
    cJSON *arr_greater_version;
};

struct json_data_t
{
    char *json_str;
    cJSON *json;
    size_t size_str;
	size_t capacity;
};

struct thread_data_t
{
    struct uthash_data_t uthash_data;
    struct json_data_t json_data;
    pthread_t tid;
    int index;
	char* url;
};

/**
 * @brief Инициализация ресурсов для потоков
 * 
 * @param td указатель на массив из NUM_THREADS элементов
 * @param branch1 имя первой ветки
 * @param branch2 имя второй ветки
 * @return Код ошибки.
 */
int thread_data_init(struct thread_data_t td[NUM_THREADS], const char* branch1, const char* branch2);

/**
 * @brief Освобождение ресурсов, выделенных для потоков
 * 
 * @param td указатель на массив из NUM_THREADS элементов
 */
void thread_data_destroy(struct thread_data_t td[NUM_THREADS]);

/**
 * @brief Выводит в stdout json, который содержит 3 массива:
 * - все пакеты, которые есть в 1-й но нет во 2-й
 * - все пакеты, которые есть в 2-й но их нет в 1-й
 * - все пакеты, version-release которых больше в 1-й чем во 2-й
 * 
 * @param uthash_data1 структура содержащая данные из 1-й ветки
 * @param uthash_data2 структура содержащая данные из 2-й ветки
 * @return Код ошибки 
 */
int print_json(struct uthash_data_t *uthash_data1, struct uthash_data_t *uthash_data2);

/**
 * @brief Заполняет массив json объектами, если в текущей ветке версия пакета больше
 * 
 * @param uthash_data структура с данными текущей ветки(и указателем на другую)
 * @return Код ошибки 
 */
int fill_pack_greater(struct uthash_data_t *uthash_data);

/**
 * @brief Заполняет массив json объектами, если пакет есть только в текущей ветке
 * 
 * @param uthash_data структура с данными текущей ветки(и указателем на другую)
 * @return Код ошибки 
 */
int fill_pack_only_cur(struct uthash_data_t *uthash_data);

/**
 * @brief Функция сравнения 2-х версий, которые хранятся в массивах
 * 
 * @param lhs 
 * @param rhs 
 * @return true Если в левой структуре версия больше
 * @return false В остальных случаях
 */
bool greater_version(struct uthash_entry_t *lhs, struct uthash_entry_t *rhs);

/**
 * @brief Очищает json в виде строки и в виде дерева
 * 
 * @param json_data 
 */
void json_data_destroe(struct json_data_t *json_data);

/**
 * @brief Очищает хэш-таблицу, буфер для элементов в хэш таблице, json массивы, которые наполнялись при сравнении
 * 
 * @param uthash_data 
 */
void uthash_destroy(struct uthash_data_t *uthash_data);

/**
 * @brief Заполняет хэш-таблицу из json дерева
 * 
 * @param data структура с хэш-таблицей
 * @param json дерево json
 * @return Код ошибки 
 */
int fill_hash_table(struct uthash_data_t *data, cJSON *json);

/**
 * @brief Скачивает через curl json-строку
 * 
 * @return Код ошибки 
 */
int get_json_list(struct json_data_t *json_data, const char *url);