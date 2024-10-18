#include "common.h"

int main(void)
{
    int ret = EXIT_SUCCESS;

    struct thread_data_t td[NUM_THREADS];

    if (thread_data_init(td, NULL, NULL) < 0)
        ERR_MAIN_EXIT("thread_data_init");

    if (get_json_list(&td[0].json_data, "https://example.com") < 0)
        ERR_MAIN_EXIT("get_json_list");

    if (td[0].json_data.size_str != 1256)
        ERR_MAIN_EXIT("check fail");

end:
    thread_data_destroy(td);
    return ret;
}