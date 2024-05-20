#include "options.h"
#include "protocol.h"




void encapsulate_run_cmd_admin(char *buffer, char command, uint32_t ip, uint32_t mask, uint16_t port, char output, uint64_t data_size) {
    buffer[0] = command;
    memcpy(&buffer[1], &ip, sizeof(uint32_t));
    memcpy(&buffer[5], &mask, sizeof(uint32_t));
    memcpy(&buffer[9], &port, sizeof(uint16_t));
    buffer[11] = output;
    memcpy(&buffer[12], &data_size, sizeof(uint64_t));
    // memcpy(&buffer[17], data, BUFFER_SIZE-16); // remaining space
}

void decapsulate_run_cmd_admin(char *buffer, uint32_t *ip, uint32_t *mask, uint16_t *port, char *output, uint64_t *data_size) {
    // *command = buffer[0];
    memcpy(ip, &buffer[1], sizeof(uint32_t));
    memcpy(mask, &buffer[5], sizeof(uint32_t));
    memcpy(port, &buffer[9], sizeof(uint16_t));
    *output = buffer[11];
    memcpy(data_size, &buffer[12], sizeof(uint64_t));
    // memcpy(data, &buffer[17], BUFFER_SIZE-16);
}

void encapsulate_run_cmd_client(char *buffer, char command, uint32_t data_stream_id, char output, uint64_t data_size) {
    buffer[0] = command;
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t));
    buffer[5] = output;
    memcpy(&buffer[6], &data_size, sizeof(uint64_t));
}

void decapsulate_run_cmd_client(char *buffer, uint32_t *data_stream_id, char *output, uint64_t *data_size) {
    // *command = buffer[0];
    memcpy(data_stream_id, &buffer[1], sizeof(uint32_t));
    *output = buffer[5];
    memcpy(data_size, &buffer[6], sizeof(uint64_t));
}


void encapsulate_transfer_cmd(char *buffer, char command, uint32_t data_stream_id) {
    buffer[0] = command; // 1 byte
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t)); // 4 bytes
}

// void decapsulate_transfer_cmd(char *buffer, uint32_t data_stream_id) {
//     // command = buffer[0]; // 1 byte
//     memcpy(&data_stream_id, &buffer[1], sizeof(uint32_t)); // 4 bytes
// }
uint32_t get_data_stream_id(char *buffer) {
    uint32_t data_stream_id;
    memcpy(&data_stream_id, &buffer[1], sizeof(uint32_t));
    return data_stream_id;
}



void encapsulate_transfer_data_cmd(char *buffer, char command, uint32_t data_stream_id, char *data) {
    buffer[0] = command; // 1 byte
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t)); // 4 bytes
    memcpy(&buffer[5], data, BUFFER_SIZE-5); // chunk of data
}

void decapsulate_transfer_data_cmd(char *buffer, uint32_t *data_stream_id, char *data, int offset) {
    // command = buffer[0]; // 1 byte
    memcpy(data_stream_id, &buffer[1], sizeof(uint32_t)); // 4 bytes
    memcpy(&data[offset], &buffer[5], BUFFER_SIZE-5); // chunk of data
}

void encapsulate_transfer_data_tid_cmd(char *buffer, char command, uint32_t data_stream_id, uint16_t thread_id, char *data) {
    buffer[0] = command; // 1 byte
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t)); // 4 bytes
    memcpy(&buffer[5], &data_stream_id, sizeof(uint16_t)); // 2 bytes
    memcpy(&buffer[7], data, BUFFER_SIZE-7); // chunk of data
}
void decapsulate_transfer_data_tid_cmd(char *buffer, uint32_t *data_stream_id, uint16_t *thread_id, char *data) {
    // command = buffer[0]; // 1 byte
    memcpy(data_stream_id, &buffer[1], sizeof(uint32_t)); // 4 bytes
    memcpy(data_stream_id, &buffer[5], sizeof(uint16_t)); // 2 bytes
    memcpy(data, &buffer[7], BUFFER_SIZE-7); // chunk of data
}

