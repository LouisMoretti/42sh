#ifndef IOBACKEND_H
#define IOBACKEND_H

int peek_chr(void);
void pop_chr(void);
int get_chr(void);
int io_setup(void);
void io_close(void);

#endif // ! IOBACKEND_H
