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
    char name[]; /* key (string is WITHIN the structure) */
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

int test_curl(void)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        /* Perform the request, res gets the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}

int diff_branch_pack(const char *branch1, const char *branch2)
{
    int ret = 0;

    test_curl();
    test_cjson();
    test_uthash();

    printf("run: diff_branch_pack(%s,%s)\n", branch1, branch2);

    return ret;
}