#include "socket.h"
#include "key.h"
// void encrypt_decrypt(char *message, char *key, int key_len) {
void encrypt_decrypt(char *message){
    // pr_info("\nkey: %s\n", key);
    // const int message_len = strlen(message);
    const int message_len = sizeof(message);
    int j = 0;
    int i;
    for (i =0 ;i <= message_len-1; i++){
        if (i >= (key_len + (key_len * j))){
            j += 1;
        }
        message[i] = message[i] ^ key[i - (key_len * j)];
        // pr_info("i: %i, j: %i \t if: %i \n", i, j, ((key_len * j))); // debug
    }
    message[i] = '\0';
}

