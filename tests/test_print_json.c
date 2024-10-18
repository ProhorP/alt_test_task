#include "common.h"
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int ret = EXIT_SUCCESS;

    // Открываем /dev/null для перенаправления
    int dev_null = open("/dev/null", O_WRONLY);

    if (dev_null == -1)
        ERR_MAIN_EXIT("print_json");

    // Перенаправляем stdout на /dev/null
    dup2(dev_null, fileno(stdout));
    close(dev_null);

    struct thread_data_t td[NUM_THREADS];

    if (thread_data_init(td, NULL, NULL) < 0)
        ERR_MAIN_EXIT("thread_data_init");

    if (print_json(&td[0].uthash_data, &td[1].uthash_data) < 0)
        ERR_MAIN_EXIT("print_json");

end:
    thread_data_destroy(td);
    return ret;
}