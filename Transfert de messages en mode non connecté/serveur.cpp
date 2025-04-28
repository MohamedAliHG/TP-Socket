#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")
#define PORT 12345
#define MAX_MSG 60

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);
    char buffer[128];
    int i;

    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Erreur socket : %d\n", WSAGetLastError());
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Erreur de bind : %d\n", WSAGetLastError());
        return 1;
    }

    printf("Serveur UDP en attente...\n");

    int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client, &client_len);
    if (bytes <= 0) {
        printf("Erreur lors de la réception.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    buffer[bytes] = '\0';
    printf("Reçu du client : %s\n", buffer);

    if (strcmp(buffer, "Bonjour") == 0) {
        for (i = 0; i < MAX_MSG; i++) {
            time_t now = time(NULL);
            struct tm *tm_info = localtime(&now);
            strftime(buffer, sizeof(buffer), "Il est %H:%M:%S !", tm_info);
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client, client_len);
            Sleep(1000);
        }

        bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client, &client_len);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            if (strcmp(buffer, "Au revoir") == 0) {
                printf("Client dit : %s\n", buffer);
            }
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
