#include "common.h"
#include "diff_branch_pack.h"

int main(int argc, char **argv)
{
    int ret = EXIT_SUCCESS;

    if (argc < 3)
        ERR_MAIN_EXIT("few arguments\n");

    if (diff_branch_pack(argv[1], argv[2]) < 0)
        ERR_MAIN_EXIT("diff_branch_pack");

end:
    return ret;
}
