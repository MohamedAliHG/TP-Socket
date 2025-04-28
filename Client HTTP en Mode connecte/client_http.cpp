#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#define MAX 1024


void http_request(const char *hostname, int port, const char *request) {
    WSADATA wsaData;
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Erreur Winsock");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur de création de socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    server_addr.sin_addr.s_addr = inet_addr(hostname);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        perror("Erreur d'adresse IP");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connexion échouée");
        exit(1);
    }

    char full_request[1024];
    snprintf(full_request, sizeof(full_request),
        "%s\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "User-Agent: MyHttpClient/1.0\r\n\r\n",
        request, hostname);


    send(sockfd, full_request, strlen(full_request), 0);

    int n;
    while ((n = recv(sockfd, buffer, MAX - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    if (n < 0) {
        perror("Erreur de réception");
    }

    closesocket(sockfd);
    WSACleanup();
}

int main() {
    char hostname[100];
    char request[1024];

    printf("Entrez l'adresse du serveur: ");
    fgets(hostname, sizeof(hostname), stdin);
    hostname[strcspn(hostname, "\n")] = 0;

    printf("Entrez la requête HTTP (par exemple GET / HTTP/1.1): ");
    fgets(request, sizeof(request), stdin);
    request[strcspn(request, "\n")] = 0;

    http_request(hostname, 80, request);

    return 0;
}
