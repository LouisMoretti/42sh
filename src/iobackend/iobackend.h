#ifndef IOBACKEND_H
#define IOBACKEND_H

int peek_chr(void);
void pop_chr(void);
int get_chr(void);
int io_setup(void);
void io_close(void);
int io_setup_string(char *string); // Exported For Testing

#endif // ! IOBACKEND_H
