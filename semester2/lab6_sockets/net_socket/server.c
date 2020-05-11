#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <bits/fcntl-linux.h>
#include "common.h"

#define LISTEN_ERR -101
#define ACCEPT_ERR -102

#define MAX_CLIENTS 100

extern int errno;

void close_socket(int s) {
    static int sock;
    if (s) {
        sock = s;
        return;
    }
    close(sock);
}

void signal_handler(int sig) {
    printf("closing");
    close_socket(0);
}

int init_server(int *sock, struct sockaddr_in *sock_addr) {
    *sock = socket(PF_INET, SOCK_STREAM, 0);
    if ((*sock) == -1) {
        perror("socket error");
        return SOCKET_ERR;
    }
//    fcntl(*sock, F_SETFL, O_NONBLOCK);
    // for initialising static sock variable
    close_socket(*sock);

    sock_addr->sin_family = PF_INET;
    sock_addr->sin_addr.s_addr = INADDR_ANY;
    sock_addr->sin_port = htons(PORT);

    if (bind(*sock, (struct sockaddr*) sock_addr, sizeof(*sock_addr)) < 0) {
        perror("bind error");
        return BIND_ERR;
    }

    signal(SIGINT, signal_handler);

    if (listen(*sock, 4) < 0) {
        perror("listen error");
        close_socket(0);
        return LISTEN_ERR;
    }

    return 0;
}

int handle_new_client(int sock, int * const client_sockets) {
    struct sockaddr_in sock_addr;
    size_t addr_len = sizeof(sock_addr);
    int new_socket = accept(sock, (struct sockaddr*) &sock_addr, (socklen_t *) &addr_len);
    if (new_socket < 0) {
        perror("accept error");
        return ACCEPT_ERR;
    }

    printf("New connection: fd = %d; %s:%d\n", new_socket,
           inet_ntoa(sock_addr.sin_addr), ntohs(sock_addr.sin_port));

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == 0) {
            client_sockets[i] = new_socket;
            break;
        }
    }

    return 0;
}

void process_all_clients(fd_set *read_fd_set, int * const client_sockets) {
    static char msg_buffer[MSG_LEN];

    for (int i = 0; i < MAX_CLIENTS; i++) {
        int sd = client_sockets[i];
        if (FD_ISSET(sd, read_fd_set)) {
            msg_buffer[0] = 0;
            ssize_t read_len = read(sd, msg_buffer, MSG_LEN);
            if (read_len == 0) {
                // smb disconnected
                close(sd);
                client_sockets[i] = 0;
            } else if (read_len == -1) {
                perror("read error");
            } else {
                msg_buffer[read_len] = 0;
                printf("New message %s", msg_buffer);
            }
        }
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in sock_addr = {};
    int err = 0;
    if ((err = init_server(&sock, &sock_addr))) {
        return err;
    }
    printf("Listener on port %d \n", PORT);

    int client_sockets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    while (1) {
        fd_set read_fd_set;
        FD_SET(sock, &read_fd_set);
        int max_sd = sock;

        // adding clients sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (sd > 0) {
                FD_SET(sd, &read_fd_set);
            }
            max_sd = sd > max_sd ? sd : max_sd;
        }

        struct timeval interval = {1, 500000};
        int activity = select(max_sd + 1, &read_fd_set, NULL, NULL, &interval);
        if (activity < 0 && errno != EINTR) {
            perror("select error");
        }

        if (FD_ISSET(sock, &read_fd_set)) {
            err = handle_new_client(sock, client_sockets);
            if (err) {
                return err;
            }
        }

        process_all_clients(&read_fd_set, client_sockets);
    }
}
