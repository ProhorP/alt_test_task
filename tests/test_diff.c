#include "common.h"

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

int main(void)
{
    int ret = EXIT_SUCCESS;

    struct thread_data_t td[NUM_THREADS];

    if (thread_data_init(td, NULL, NULL) < 0)
        ERR_MAIN_EXIT("thread_data_init");

    td[0].json_data.json_str = strdup(str1);
    td[1].json_data.json_str = strdup(str2);

    for (size_t i = 0; i < NUM_THREADS; i++)
    {
        // парсим его
        td[i].json_data.json = cJSON_Parse(td[i].json_data.json_str);
        if (!td[i].json_data.json)
            ERR_MAIN_EXIT("invalid json: %s", td[i].json_data.json_str);

        if (fill_hash_table(&td[i].uthash_data, td[i].json_data.json) < 0)
            ERR_MAIN_EXIT("fill_hash_table");
    }

    for (size_t i = 0; i < NUM_THREADS; i++)
        if (fill_pack_only_cur(&td[i].uthash_data) < 0)
            ERR_MAIN_EXIT("fill_pack_only_cur");

    if (fill_pack_greater(&td[0].uthash_data) < 0)
        ERR_MAIN_EXIT("fill_pack_greater");

    if (cJSON_GetArraySize(td[0].uthash_data.array) != 1 ||
        cJSON_GetArraySize(td[1].uthash_data.array) != 1 ||
        cJSON_GetArraySize(td[0].uthash_data.arr_greater_version) != 1)
        ERR_MAIN_EXIT("check fail");

end:
    thread_data_destroy(td);
    return ret;
}
