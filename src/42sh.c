#include <stdio.h>

#include "config/config.h"
#include "iobackend/iobackend.h"

int main(int argc, char **argv)
{
    if (set_conf(argc, argv) != 0)
        return 2;
    io_setup();

    io_close();
    return 0;
}
