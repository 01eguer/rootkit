#pragma once
#include <linux/string.h>
#include "../net/socket.h"
#include "options.h"
#include "protocol.h"

int receive_data(char *buffer, uint32_t data_stream_id, uint64_t data_size, char *data);
int send_output_from_file(char *buffer, char *file_buffer, int file_size, char *file_name, uint32_t data_stream_id, char *data);