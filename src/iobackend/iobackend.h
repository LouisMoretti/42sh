#ifndef IOBACKEND_H
#define IOBACKEND_H

#include "config/config.h"

int peek_chr(void);
void pop_chr(void);
int get_chr(void);
int io_setup(struct config *my_conf);
void io_close(void);

#endif // ! IOBACKEND_H
