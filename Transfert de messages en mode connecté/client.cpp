
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    strcpy(buffer, "Bonjour");
    send(sock, buffer, strlen(buffer) + 1, 0);

    for (int i = 0; i < 60; i++) {
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("Reçu : %s\n", buffer);
    }

    strcpy(buffer, "Au revoir");
    send(sock, buffer, strlen(buffer) + 1, 0);

    closesocket(sock);
    WSACleanup();
    return 0;
}