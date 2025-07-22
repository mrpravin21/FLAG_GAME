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
        system(url); // This is for MacOS, use xdg-open for Linux
    } else {
        printf("Server is not available.\n");
    }

    return 0;
}
