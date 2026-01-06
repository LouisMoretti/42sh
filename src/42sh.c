#include <stdio.h>

#include "config/config.h"

int main(int argc, char **argv)
{
    if (set_conf(argc, argv) != 0)
        return 2;
    // call io_setup

    return 0;
}
