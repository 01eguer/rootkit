#pragma once
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <net/sock.h>
#include "../server/options.h"


extern struct socket *conn_socket;

int connect_to_server(void);

int send(char *buffer, int size);
int receive(char *buffer, int size);
// int send_message(char *message);
// int receive_message(char *buffer);


void encrypt_decrypt(char *message);
