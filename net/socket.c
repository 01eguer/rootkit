#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <net/sock.h>
#include "socket.h"

struct socket *conn_socket = NULL;

int connect_to_server(void) {
    struct sockaddr_in server_addr;
    int ret;

    // Create a TCP socket
    ret = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &conn_socket);
    if (ret < 0) {
        pr_err("Failed to create socket\n");
        return ret;
    }

    // Initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = in_aton(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Connect to the server
    ret = conn_socket->ops->connect(conn_socket, (struct sockaddr *)&server_addr, sizeof(server_addr), 0);
    if (ret < 0) {
        pr_err("Failed to connect to server\n");
        sock_release(conn_socket);
        return ret;
    }

    pr_info("Connected to server\n");

    return 0;
}

int send(char *buffer, int size) {
    int ret;

    struct msghdr msg = {0};
    struct kvec iov = {.iov_base = (void *)buffer, .iov_len = size};
    // msleep(400);
    ret = kernel_sendmsg(conn_socket, &msg, &iov, 1, size);
    if (ret < 0) {
        pr_err("Failed to send message\n");
        return ret;
    }

    pr_info("Message sent successfully\n");

    return 0;
}

int receive(char *buffer, int size) {
    int ret;

    struct msghdr rcv_msg = {0};
    struct kvec rcv_iov = {.iov_base = (void *)buffer, .iov_len = BUFFER_SIZE};
    ret = kernel_recvmsg(conn_socket, &rcv_msg, &rcv_iov, 1, size, MSG_EOR);
    if (ret < 0) {
        pr_err("Failed to receive message\n");
        return ret;
    }

    // pr_info("Received message from server: %s\n", buffer);

    return 0;
}
