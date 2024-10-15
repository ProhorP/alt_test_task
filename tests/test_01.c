#include "common.h"
#include "diff_branch_pack.h"

int main(void)
{
    int ret = EXIT_SUCCESS;

    if (diff_branch_pack("branch1", "branch2") < 0)
        ERR_MAIN_EXIT("diff_branch_pack");

end:
    return ret;
}