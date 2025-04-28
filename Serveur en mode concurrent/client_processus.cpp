#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 12346
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2,2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server, sizeof(server));

    int bytes = recv(sock, buffer, BUFFER_SIZE, 0);
    buffer[bytes] = '\0';
    printf("Réponse : %s\n", buffer);

    closesocket(sock);
    WSACleanup();
    return 0;
}
