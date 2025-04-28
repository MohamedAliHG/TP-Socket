#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 128
#define MAX_MSG 60

DWORD WINAPI clientHandler(LPVOID clientSocketPtr) {
    SOCKET clientSocket = *(SOCKET *)clientSocketPtr;
    free(clientSocketPtr);

    char buffer[BUFFER_SIZE];
    int bytes;

    bytes = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) {
        closesocket(clientSocket);
        return 1;
    }

    buffer[bytes] = '\0';
    printf("[Thread %d] Reçu : %s\n", GetCurrentThreadId(), buffer);

    if (strcmp(buffer, "Bonjour") == 0) {
        for (int i = 0; i < MAX_MSG; i++) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            snprintf(buffer, BUFFER_SIZE, "Il est %02d:%02d:%02d !\n", t->tm_hour, t->tm_min, t->tm_sec);
            send(clientSocket, buffer, strlen(buffer), 0);
            Sleep(1000);
        }

        bytes = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            if (strcmp(buffer, "Au revoir") == 0) {
                printf("[Thread %d] Fin de communication avec le client.\n", GetCurrentThreadId());
            }
        }
    }

    closesocket(clientSocket);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int clientLen = sizeof(client);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Erreur socket : %d\n", WSAGetLastError());
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Erreur bind : %d\n", WSAGetLastError());
        return 1;
    }

    listen(serverSocket, SOMAXCONN);
    printf("Serveur en attente de connexions sur le port %d...\n", PORT);

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&client, &clientLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Erreur accept : %d\n", WSAGetLastError());
            continue;
        }

        printf("Nouveau client connecté.\n");
        SOCKET *pClientSocket = malloc(sizeof(SOCKET));
        *pClientSocket = clientSocket;

        CreateThread(NULL, 0, clientHandler, pClientSocket, 0, NULL);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

