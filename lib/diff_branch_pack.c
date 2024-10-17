#include "common.h"
#include "diff_branch_pack.h"

#include <stdio.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "uthash.h"

struct uthash_entry
{
    int id;
    cJSON *obj;
    UT_hash_handle hh; /* makes this structure hashable */
    char name[];       /* key (string is WITHIN the structure) */
};

int test_uthash(void)
{
    const char *names[] = {"joe", "bob", "betty", NULL};
    struct uthash_entry *s, *tmp, *users = NULL;

    for (int i = 0; names[i]; ++i)
    {
        s = (struct uthash_entry *)malloc(sizeof *s + strlen(names[i]) + 1);
        strcpy(s->name, names[i]);
        s->id = i;
        HASH_ADD_STR(users, name, s);
    }

    HASH_FIND_STR(users, "betty", s);
    if (s)
        printf("betty's id is %d\n", s->id);

    /* free the hash table contents */
    HASH_ITER(hh, users, s, tmp)
    {
        HASH_DEL(users, s);
        free(s);
    }
    return 0;
}

int test_cjson(void)
{
    int ret = 0;
    cJSON *json = cJSON_CreateObject();

    cJSON_Delete(json);

    return ret;
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

int diff_branch_pack(const char *branch1, const char *branch2)
{
    int ret = 0;

    struct memory json_data = {0};

    if (get_json_list(&json_data, "https://example.com") < 0)
        ERR_EXIT("get_json_list");

    printf("%s", json_data.json_str);
    test_cjson();
    test_uthash();

    printf("run: diff_branch_pack(%s,%s)\n", branch1, branch2);

end:
    OS_FREE(json_data.json_str);

    return ret;
}