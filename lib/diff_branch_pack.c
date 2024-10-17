#include "common.h"
#include "diff_branch_pack.h"

#include <stdbool.h>
#include <stdio.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "uthash.h"

const char str1[] = "{\n"
                    "\"request_args\": {\"arch\": null}, \"length\": 187619, \"packages\":\n"
                    "[\n"
                    "{\"name\": \"a1\", \"epoch\": 1, \"version\": \"0.0.1\", \"release\": \"alt0_1_alpha.p10\", \"arch\": \"aarch64\", \"disttag\": \"p10+307479.400.5.1\", \"buildtime\": 1665497454, \"source\": \"0ad\"}\n"
                    ",\n"
                    "{\"name\": \"a1\", \"epoch\": 1, \"version\": \"0.0.2\", \"release\": \"alt0_1_alpha.p10\", \"arch\": \"i586\", \"disttag\": \"p10+307479.400.5.1\", \"buildtime\": 1665497454, \"source\": \"0ad\"}\n"
                    ",\n"
                    "{\"name\": \"a2\", \"epoch\": 1, \"version\": \"0.0.1\", \"release\": \"alt0_1_alpha.p10\", \"arch\": \"i586\", \"disttag\": \"p10+307479.400.5.1\", \"buildtime\": 1665497454, \"source\": \"0ad\"}\n"
                    "]\n"
                    "}";

const char str2[] = "{\n"
                    "\"request_args\": {\"arch\": null}, \"length\": 187619, \"packages\":\n"
                    "[\n"
                    "{\"name\": \"a1\", \"epoch\": 1, \"version\": \"0.0.1\", \"release\": \"alt0_1_alpha.p10\", \"arch\": \"aarch64\", \"disttag\": \"p10+307479.400.5.1\", \"buildtime\": 1665497454, \"source\": \"0ad\"}\n"
                    ",\n"
                    "{\"name\": \"a1\", \"epoch\": 1, \"version\": \"0.0.1\", \"release\": \"alt0_1_alpha.p10\", \"arch\": \"i586\", \"disttag\": \"p10+307479.400.5.1\", \"buildtime\": 1665497454, \"source\": \"0ad\"}\n"
                    ",\n"
                    "{\"name\": \"a3\", \"epoch\": 1, \"version\": \"0.0.1\", \"release\": \"alt0_1_alpha.p10\", \"arch\": \"i586\", \"disttag\": \"p10+307479.400.5.1\", \"buildtime\": 1665497454, \"source\": \"0ad\"}\n"
                    "]\n"
                    "}";

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
};

struct json_data_t
{
    char *json_str;
    cJSON *json;
    size_t size_str;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct json_data_t *mem = (struct json_data_t *)userp;

    // Расширяем буфер для нового ответа
    char *ptr = realloc(mem->json_str, mem->size_str + realsize + 1);
    if (ptr == NULL)
    {
        // Не хватает памяти
        printf("Not enough memory to allocate buffer\n");
        return 0;
    }

    mem->json_str = ptr;
    // Копируем данные в наш буфер
    memcpy(&(mem->json_str[mem->size_str]), contents, realsize);
    mem->size_str += realsize;
    mem->json_str[mem->size_str] = '\0'; // Завершаем строку нулевым символом

    return realsize;
}

int get_json_list(struct json_data_t *json_data, const char *url)
{
    int ret = 0;

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl)
        ERR_EXIT("curl_easy_init");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Устанавливаем функцию обратного вызова для обработки данных
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    // Передаем наш буфер в функцию обратного вызова
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, json_data);

    // Выполняем запрос
    res = curl_easy_perform(curl);

    // Проверяем ошибки
    if (res != CURLE_OK)
        ERR_EXIT("curl_easy_perform");

end:
    if (curl)
    {
        // Освобождаем ресурсы curl
        curl_easy_cleanup(curl);
        curl = NULL;
    }

    return ret;
}

int fill_hash_table(struct uthash_data_t *data, cJSON *json)
{
    int ret = 0;
    cJSON *tmp = NULL;
    char *buf_version = NULL;
    size_t max_version_size = 0;

    cJSON *packages = cJSON_GetObjectItemCaseSensitive(json, "packages");
    if (!(packages && cJSON_IsArray(packages)))
        ERR_EXIT("not found json.packages(array)");

    size_t size = cJSON_GetArraySize(packages);
    if (size == 0)
        goto end;

    /* Пройдем массив, посмотрим сколько нужно памяти чтобы сделать 1 malloc
     *  Получим размер максимальной длины ключа*/
    for (size_t i = 0; i < size; i++)
    {
        cJSON *package = cJSON_GetArrayItem(packages, i);

        // json.packages[i].name
        if (!(tmp = cJSON_GetObjectItemCaseSensitive(package, "name")))
            ERR_EXIT("not found json.packages[i].name");
        const char *name = cJSON_GetStringValue(tmp);
        if (!name)
            ERR_EXIT("is NULL json.packages[i].name");

        // json.packages[i].version
        if (!(tmp = cJSON_GetObjectItemCaseSensitive(package, "version")))
            ERR_EXIT("not found json.packages[i].version");
        char *version = cJSON_GetStringValue(tmp);
        if (!version)
            ERR_EXIT("is NULL json.packages[i].version");

        // json.packages[i].arch
        if (!(tmp = cJSON_GetObjectItemCaseSensitive(package, "arch")))
            ERR_EXIT("not found json.packages[i].arch");
        const char *arch = cJSON_GetStringValue(tmp);
        if (!arch)
            ERR_EXIT("is NULL json.packages[i].arch");

        size_t cur_key_size = strlen(name) + strlen(arch) + 1;
        size_t cur_version_size = strlen(version) + 1;
        max_version_size = (cur_version_size > max_version_size) ? cur_version_size : max_version_size;
        data->buf_size += sizeof(struct uthash_entry_t) + cur_key_size;
    }

    data->buf = malloc(data->buf_size);
    if (!data->buf)
        ERR_EXIT("malloc");

    buf_version = malloc(max_version_size);
    if (!buf_version)
        ERR_EXIT("malloc");

    char *ptr = data->buf;

    for (size_t i = 0; i < size; i++)
    {
        cJSON *package = cJSON_GetArrayItem(packages, i);

        // json.packages[i].name
        if (!(tmp = cJSON_GetObjectItemCaseSensitive(package, "name")))
            ERR_EXIT("not found json.packages[i].name");
        const char *name = cJSON_GetStringValue(tmp);
        if (!name)
            ERR_EXIT("is NULL json.packages[i].name");

        // json.packages[i].version
        if (!(tmp = cJSON_GetObjectItemCaseSensitive(package, "version")))
            ERR_EXIT("not found json.packages[i].version");
        char *version = cJSON_GetStringValue(tmp);
        if (!version)
            ERR_EXIT("is NULL json.packages[i].version");

        // json.packages[i].release
        if (!(tmp = cJSON_GetObjectItemCaseSensitive(package, "release")))
            ERR_EXIT("not found json.packages[i].release");
        const char *release = cJSON_GetStringValue(tmp);
        if (!release)
            ERR_EXIT("is NULL json.packages[i].arch");

        // json.packages[i].arch
        if (!(tmp = cJSON_GetObjectItemCaseSensitive(package, "arch")))
            ERR_EXIT("not found json.packages[i].arch");
        const char *arch = cJSON_GetStringValue(tmp);
        if (!arch)
            ERR_EXIT("is NULL json.packages[i].arch");

        size_t cur_key_size = strlen(name) + strlen(arch) + 1;
        struct uthash_entry_t *s = (struct uthash_entry_t *)ptr;
        memset(s, 0, sizeof(*s));
        strcpy(s->name, name);
        strcat(s->name, arch);
        s->obj = package;
        s->release = release;

        // версию поместим в массив для быстрого сравнения
        strcpy(buf_version, version);
        char *token = strtok(buf_version, ".");

        for (size_t i = 0; token != NULL; i++)
        {
            if (i > sizeof(s->version) / sizeof(s->version[0]))
                ERR_EXIT("version very long");
            char *endptr = NULL;
            s->version[i] = strtol(token, &endptr, 10);
            if (*endptr != '\0')
                ERR_EXIT("strtol");
            token = strtok(NULL, ".");
        }

        HASH_ADD_STR(data->users, name, s);
        ptr += sizeof(*s) + cur_key_size;
    }

end:
    OS_FREE(buf_version);

    return ret;
}

void uthash_destroy(struct uthash_data_t *uthash_data)
{
    struct uthash_entry_t *s, *tmp = NULL;
    /* free the hash table contents */
    HASH_ITER(hh, uthash_data->users, s, tmp)
    {
        HASH_DEL(uthash_data->users, s);
    }
    OS_FREE(uthash_data->buf);
}

void json_data_destroe(struct json_data_t *json_data)
{
    OS_FREE(json_data->json_str);
    OS_JSON_FREE(json_data->json);
}

bool greater_version(struct uthash_entry_t *lhs, struct uthash_entry_t *rhs)
{
    for (size_t i = 0; i < sizeof(lhs->version) / sizeof(lhs->version[0]); i++)
    {
        if (lhs->version[i] > rhs->version[i])
            return true;
        if (lhs->version[i] < rhs->version[i])
            return false;
    }
    return false;
}

int diff_branch_pack(const char *branch1, const char *branch2)
{
    int ret = 0;

    struct json_data_t json_data[2] = {0};
    struct uthash_data_t uthash_data[2] = {0};
    uthash_data[0].other = &uthash_data[1];
    uthash_data[1].other = &uthash_data[0];
    cJSON *arr_list1_greater_version = NULL;
    cJSON *json_out = NULL;
    char *json_out_str = NULL;

    for (size_t i = 0; i < 2; i++)
        if (!(uthash_data[i].array = cJSON_CreateArray()))
            ERR_EXIT("cJSON_CreateArray");
    if (!(arr_list1_greater_version = cJSON_CreateArray()))
        ERR_EXIT("cJSON_CreateArray");

    for (size_t i = 0; i < 2; i++)
    {
        if (get_json_list(&json_data[i], "https://example.com") < 0)
            ERR_EXIT("get_json_list");

        // заглушка для проверки кода
        OS_FREE(json_data[i].json_str);

        if (i == 0)
            json_data[i].json_str = strdup(str1);
        else
            json_data[i].json_str = strdup(str2);

        json_data[i].json = cJSON_Parse(json_data[i].json_str);
        if (!json_data[i].json)
            ERR_EXIT("invalid json: %s", json_data[i].json_str);

        if (fill_hash_table(&uthash_data[i], json_data[i].json) < 0)
            ERR_EXIT("fill_hash_table");
    }

    for (size_t i = 0; i < 2; i++)
    {
        struct uthash_entry_t *s1, *s2, *tmp = NULL;
        /* free the hash table contents */
        HASH_ITER(hh, uthash_data[i].users, s1, tmp)
        {
            HASH_FIND_STR(uthash_data[i].other->users, s1->name, s2);
            if (!s2)
            {
                if (!cJSON_AddItemReferenceToArray(uthash_data[i].array, s1->obj))
                    ERR_EXIT("cJSON_AddItemToArray");
                printf("in json%ld unique %s\n", i, s1->name);
            }
            s2 = NULL;
        }
    }

    {
        struct uthash_entry_t *s1, *s2, *tmp = NULL;
        /* free the hash table contents */
        HASH_ITER(hh, uthash_data[0].users, s1, tmp)
        {
            HASH_FIND_STR(uthash_data[0].other->users, s1->name, s2);
            if (s2)
                if (greater_version(s1, s2))
                    if (!cJSON_AddItemReferenceToArray(arr_list1_greater_version, s1->obj))
                        ERR_EXIT("cJSON_AddItemToArray");
            s2 = NULL;
        }
    }

    if (!(json_out = cJSON_CreateObject()))
        ERR_EXIT("cJSON_CreateObject()");

    if (!cJSON_AddItemToObject(json_out, "only1", uthash_data[0].array))
        ERR_EXIT("cJSON_AddItemToObject");
    uthash_data[0].array = NULL;

    if (!cJSON_AddItemToObject(json_out, "only2", uthash_data[1].array))
        ERR_EXIT("cJSON_AddItemToObject");
    uthash_data[1].array = NULL;

    if (!cJSON_AddItemToObject(json_out, "version1great", arr_list1_greater_version))
        ERR_EXIT("cJSON_AddItemToObject");
    arr_list1_greater_version = NULL;

    if (!(json_out_str = cJSON_Print(json_out)))
        ERR_EXIT("cJSON_Print");

    printf("%s\n", json_out_str);
    printf("run: diff_branch_pack(%s,%s)\n", branch1, branch2);

end:
    for (size_t i = 0; i < 2; i++)
    {
        uthash_destroy(&uthash_data[i]);
        json_data_destroe(&json_data[i]);
    }
    OS_JSON_FREE(json_out);
    OS_FREE(json_out_str);

    return ret;
}