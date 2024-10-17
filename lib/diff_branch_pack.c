#include "common.h"
#include "diff_branch_pack.h"

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

struct uthash_entry_t
{
    const char *version;
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
    size_t max_key_size;
};

int test_uthash(void)
{
    const char *names[] = {"joe", "bob", "betty", NULL};
    struct uthash_entry_t *s, *tmp, *users = NULL;

    for (int i = 0; names[i]; ++i)
    {
        s = (struct uthash_entry_t *)malloc(sizeof *s + strlen(names[i]) + 1);
        strcpy(s->name, names[i]);
        // s->id = i;
        HASH_ADD_STR(users, name, s);
    }

    HASH_FIND_STR(users, "betty", s);
    if (s)
        printf("betty's id is %s\n", s->release);

    /* free the hash table contents */
    HASH_ITER(hh, users, s, tmp)
    {
        HASH_DEL(users, s);
        free(s);
    }
    return 0;
}

struct memory
{
    char *json_str;
    size_t size;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)userp;

    // Расширяем буфер для нового ответа
    char *ptr = realloc(mem->json_str, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        // Не хватает памяти
        printf("Not enough memory to allocate buffer\n");
        return 0;
    }

    mem->json_str = ptr;
    // Копируем данные в наш буфер
    memcpy(&(mem->json_str[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->json_str[mem->size] = '\0'; // Завершаем строку нулевым символом

    return realsize;
}

int get_json_list(struct memory *json_data, const char *url)
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
    struct uthash_entry_t *s, *tmp = NULL;
    cJSON *tmp_json = NULL;

    cJSON *packages = cJSON_GetObjectItemCaseSensitive(json, "packages");
    if (!(packages && cJSON_IsArray(packages)))
        ERR_EXIT("not found json.packages(array)");

    size_t size = cJSON_GetArraySize(packages);

    for (size_t i = 0; i < size; i++)
    {
        cJSON *package = cJSON_GetArrayItem(packages, i);

        // json.packages[i].name
        if (!(tmp_json = cJSON_GetObjectItemCaseSensitive(package, "name")))
            ERR_EXIT("not found json.packages[i].name");
        const char *name = cJSON_GetStringValue(tmp_json);
        if (!name)
            ERR_EXIT("is NULL json.packages[i].name");

        // json.packages[i].arch
        if (!(tmp_json = cJSON_GetObjectItemCaseSensitive(package, "arch")))
            ERR_EXIT("not found json.packages[i].arch");
        const char *arch = cJSON_GetStringValue(tmp_json);
        if (!arch)
            ERR_EXIT("is NULL json.packages[i].arch");

        size_t cur_key_size = strlen(name) + strlen(arch) + 1;
        data->max_key_size = (cur_key_size > data->max_key_size) ? cur_key_size : data->max_key_size;
        data->buf_size += sizeof(struct uthash_entry_t) + cur_key_size;
    }

    data->buf = malloc(data->buf_size);
    if (!data->buf)
        ERR_EXIT("malloc");

    char *ptr = data->buf;

    for (size_t i = 0; i < size; i++)
    {
        cJSON *package = cJSON_GetArrayItem(packages, i);

        // json.packages[i].name
        if (!(tmp_json = cJSON_GetObjectItemCaseSensitive(package, "name")))
            ERR_EXIT("not found json.packages[i].name");
        const char *name = cJSON_GetStringValue(tmp_json);
        if (!name)
            ERR_EXIT("is NULL json.packages[i].name");

        // json.packages[i].version
        if (!(tmp_json = cJSON_GetObjectItemCaseSensitive(package, "version")))
            ERR_EXIT("not found json.packages[i].version");
        const char *version = cJSON_GetStringValue(tmp_json);
        if (!version)
            ERR_EXIT("is NULL json.packages[i].version");

        // json.packages[i].release
        if (!(tmp_json = cJSON_GetObjectItemCaseSensitive(package, "release")))
            ERR_EXIT("not found json.packages[i].release");
        const char *release = cJSON_GetStringValue(tmp_json);
        if (!release)
            ERR_EXIT("is NULL json.packages[i].arch");

        // json.packages[i].arch
        if (!(tmp_json = cJSON_GetObjectItemCaseSensitive(package, "arch")))
            ERR_EXIT("not found json.packages[i].arch");
        const char *arch = cJSON_GetStringValue(tmp_json);
        if (!arch)
            ERR_EXIT("is NULL json.packages[i].arch");

        size_t cur_key_size = strlen(name) + strlen(arch) + 1;
        struct uthash_entry_t *s = (struct uthash_entry_t *)ptr;
        strcpy(s->name, name);
        strcat(s->name, arch);
        s->release = release;
        s->version = version;
        HASH_ADD_STR(data->users, name, s);
        ptr += sizeof(*s) + cur_key_size;
    }

    HASH_FIND_STR(data->users, "a1aarch64", s);
    if (s)
        printf("a1aarch64 release is %s\n", s->release);

    /* free the hash table contents */
    HASH_ITER(hh, data->users, s, tmp)
    {
        HASH_DEL(data->users, s);
    }
end:
    return ret;
}

int diff_branch_pack(const char *branch1, const char *branch2)
{
    int ret = 0;

    struct memory json_data = {0};
    cJSON *json = NULL;
    struct uthash_data_t uthash_data = {0};

    if (get_json_list(&json_data, "https://example.com") < 0)
        ERR_EXIT("get_json_list");

    // заглушка для проверки кода
    OS_FREE(json_data.json_str);
    json_data.json_str = strdup(str1);

    json = cJSON_Parse(json_data.json_str);
    if (!json)
        ERR_EXIT("invalid json: %s", json_data.json_str);

    if (fill_hash_table(&uthash_data, json) < 0)
        ERR_EXIT("fill_hash_table");

    printf("run: diff_branch_pack(%s,%s)\n", branch1, branch2);

end:
    OS_FREE(uthash_data.buf);
    OS_FREE(json_data.json_str);
    OS_JSON_FREE(json);

    return ret;
}