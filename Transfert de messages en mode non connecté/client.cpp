#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
#define PORT 12345

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    int server_len = sizeof(server);
    char buffer[1024];
    int i;

    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Erreur socket : %d\n", WSAGetLastError());
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");


    strcpy(buffer, "Bonjour");
    sendto(sock, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&server, server_len);

    for (i = 0; i < 60; i++) {
        int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&server, &server_len);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Reçu : %s\n", buffer);
        }
    }

    strcpy(buffer, "Au revoir");
    sendto(sock, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&server, server_len);

    closesocket(sock);
    WSACleanup();
    return 0;
}
