#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include <psapi.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT_HEURE 12345
#define PORT_PROC 12346
#define PORT_FILE 12347
#define BUFFER_SIZE 1024
#define MAX_MSG 60


typedef struct {
    int port;
    LPTHREAD_START_ROUTINE handler;
} ServiceInfo;

DWORD WINAPI handleHeure(LPVOID clientSocketPtr) {
    SOCKET clientSocket = *(SOCKET *)clientSocketPtr;
    free(clientSocketPtr);

    char buffer[BUFFER_SIZE];
    recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

    if (strcmp(buffer, "Bonjour") == 0) {
        for (int i = 0; i < MAX_MSG; i++) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            snprintf(buffer, BUFFER_SIZE, "Il est %02d:%02d:%02d !\n", t->tm_hour, t->tm_min, t->tm_sec);
            send(clientSocket, buffer, strlen(buffer), 0);
            Sleep(1000);
        }
        recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (strcmp(buffer, "Au revoir") == 0) {
            printf("[Heure] Client a terminé.\n");
        }
    }

    closesocket(clientSocket);
    return 0;
}

DWORD WINAPI handleProcess(LPVOID clientSocketPtr) {
    SOCKET clientSocket = *(SOCKET *)clientSocketPtr;
    free(clientSocketPtr);

    char buffer[BUFFER_SIZE];
    DWORD processCount = 0;
    DWORD aProcesses[1024], cbNeeded;

    if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        processCount = cbNeeded / sizeof(DWORD);
        snprintf(buffer, BUFFER_SIZE, "Nombre de processus actifs : %lu\n", processCount);
        send(clientSocket, buffer, strlen(buffer), 0);
    }

    closesocket(clientSocket);
    return 0;
}

DWORD WINAPI handleFichier(LPVOID clientSocketPtr) {
    SOCKET clientSocket = *(SOCKET *)clientSocketPtr;
    free(clientSocketPtr);

FILE *file = fopen("client_fichier.txt", "r");


    if (file == NULL) {
        char msg[] = "Fichier introuvable.\n";
        send(clientSocket, msg, strlen(msg), 0);
        closesocket(clientSocket);
        return 1;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        send(clientSocket, line, strlen(line), 0);
        Sleep(100); // pour éviter l'envoi en bloc
    }

    fclose(file);
    closesocket(clientSocket);
    return 0;
}

DWORD WINAPI lancerService(LPVOID arg) {
    ServiceInfo *info = (ServiceInfo *)arg;

    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int clientLen = sizeof(client);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(info->port);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&server, sizeof(server));
    listen(serverSocket, SOMAXCONN);

    printf("[Service %d] En attente de clients...\n", info->port);

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientLen);
        SOCKET *pClient = malloc(sizeof(SOCKET));
        *pClient = clientSocket;
        CreateThread(NULL, 0, info->handler, pClient, 0, NULL);
    }

    return 0;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    ServiceInfo *svc1 = malloc(sizeof(ServiceInfo));
    svc1->port = PORT_HEURE;
    svc1->handler = handleHeure;

    ServiceInfo *svc2 = malloc(sizeof(ServiceInfo));
    svc2->port = PORT_PROC;
    svc2->handler = handleProcess;

    ServiceInfo *svc3 = malloc(sizeof(ServiceInfo));
    svc3->port = PORT_FILE;
    svc3->handler = handleFichier;

    CreateThread(NULL, 0, lancerService, svc1, 0, NULL);
    CreateThread(NULL, 0, lancerService, svc2, 0, NULL);
    CreateThread(NULL, 0, lancerService, svc3, 0, NULL);

    while (1) {
        Sleep(1000);
    }

    WSACleanup();
    return 0;
}