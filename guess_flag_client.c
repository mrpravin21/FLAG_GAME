
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_RETRIES 10
#define RETRY_INTERVAL_SECONDS 1

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    int retries = 0;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }

    printf("Waiting for server to be available at localhost:%d...\n", PORT);

    while (retries < MAX_RETRIES) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Socket creation error");
            return 1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
            close(sock);
            printf("Server is available!\n");
            printf("Launching browser...\n");
            system("open http://localhost:8080/");  // MacOS
            // system("xdg-open http://localhost:8080/");  // Linux
            return 0;
        } else {
            close(sock);
            retries++;
            printf("Server not available yet, retry %d/%d...\n", retries, MAX_RETRIES);
            sleep(RETRY_INTERVAL_SECONDS);
        }
    }

    printf("Failed to connect to server after %d retries.\n", MAX_RETRIES);
    return 1;
}
*/

/* ==================== CLIENT PROGRAM ==================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char roll[50];

    printf("Enter your roll number: ");
    scanf("%49s", roll);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
        close(sock);
        char url[200];
        snprintf(url, sizeof(url), "open http://localhost:8080/?roll=%s", roll);
        system(url); // MacOS, use xdg-open for Linux
    } else {
        printf("Server is not available.\n");
    }

    return 0;
}
