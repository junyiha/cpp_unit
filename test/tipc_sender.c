#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/tipc.h>

#define SERVER_TYPE 18888
#define SERVER_INST 17

#define MAX_MSG_SIZE 256

void server() {
    int sock;
    struct sockaddr_tipc server_addr;
    char buffer[MAX_MSG_SIZE];
    int ret, len;

    // 创建 TIPC 套接字
    sock = socket(AF_TIPC, SOCK_RDM, 0);
    if (sock == -1) {
        perror("socket");
        return;
    }

    // 绑定服务器地址
    server_addr.family = AF_TIPC;
    server_addr.addrtype = TIPC_ADDR_NAME;
    server_addr.addr.nameseq.type = SERVER_TYPE;
    server_addr.addr.nameseq.lower = 0;
    server_addr.addr.nameseq.upper = SERVER_INST;

    ret = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("bind");
        close(sock);
        return;
    }

    // 接收来自客户端的消息
    len = recv(sock, buffer, MAX_MSG_SIZE, 0);
    if (len == -1) {
        perror("recv");
        close(sock);
        return;
    }

    printf("Received message from client: %s\n", buffer);

    close(sock);
}

void client() {
    int sock;
    struct sockaddr_tipc server_addr;
    char *message = "Hello, server!";
    int ret;

    // 创建 TIPC 套接字
    sock = socket(AF_TIPC, SOCK_RDM, 0);
    if (sock == -1) {
        perror("socket");
        return;
    }

    // 设置服务器地址
    server_addr.family = AF_TIPC;
    server_addr.addrtype = TIPC_ADDR_NAME;
    server_addr.addr.nameseq.type = SERVER_TYPE;
    server_addr.addr.nameseq.lower = 0;
    server_addr.addr.nameseq.upper = SERVER_INST;

    // 发送消息到服务器
    ret = sendto(sock, message, strlen(message), 0,
                 (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("sendto");
        close(sock);
        return;
    }

    printf("Message sent to server: %s\n", message);

    close(sock);
}

int main() {
    // 创建子进程，一个作为服务器，一个作为客户端
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程作为服务器
        server();
    } else if (pid > 0) {
        // 父进程作为客户端
        client();
    } else {
        perror("fork");
        return 1;
    }

    return 0;
}
