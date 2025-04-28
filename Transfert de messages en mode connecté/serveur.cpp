#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2, 2), &wsa);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 1);

    printf("Serveur en attente...\n");

    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("Client dit : %s\n", buffer);

    if (strcmp(buffer, "Bonjour") == 0) {
        for (int i = 0; i < 60; i++) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            snprintf(buffer, BUFFER_SIZE, "Il est %02d:%02d:%02d !", t->tm_hour, t->tm_min, t->tm_sec);
            send(client_socket, buffer, strlen(buffer) + 1, 0);
            //Sleep(500);
        }


        recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, "Au revoir") == 0) {
            printf("Client dit : %s\n", buffer);

        }
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

