#include "tools.h"

void read_file(char *buf, char *file_name, size_t size, loff_t offset) {
    struct file *pipe_file;

    pipe_file = filp_open(file_name, O_RDONLY, 0);

    pipe_file->f_pos = offset;

    // read bytes
    kernel_read(pipe_file, buf, size, &pipe_file->f_pos);
    filp_close(pipe_file, NULL);
}

