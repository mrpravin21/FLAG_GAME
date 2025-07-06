#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8080
#define BUF_SIZE 4096

// Function to determine content type based on file extension
const char* get_content_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return "text/plain";

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    if (strcmp(ext, ".wav") == 0) return "audio/wav";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";

    return "application/octet-stream"; // Default binary type
}

// Function to send file contents over socket
void send_file(int client_sock, const char *filename, const char *content_type) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        // File not found, send 404
        char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found\n";
        write(client_sock, not_found, strlen(not_found));
        return;
    }

    char buffer[BUF_SIZE];
    int bytes_read;

    // Get file size
    off_t filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    // Send HTTP headers
    char header[256];
    snprintf(header, sizeof(header),
         "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %lld\r\n\r\n",
         content_type, (long long)filesize);

    write(client_sock, header, strlen(header));

    // Send file content
    while ((bytes_read = read(fd, buffer, BUF_SIZE)) > 0) {
        write(client_sock, buffer, bytes_read);
    }

    close(fd);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int addr_len = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len)) < 0) {
            perror("accept");
            continue;
        }

        char buffer[BUF_SIZE] = {0};
        read(client_sock, buffer, BUF_SIZE - 1);

        // Parse first line of request: GET /path HTTP/1.1
        char method[8], path[256];
        sscanf(buffer, "%s %s", method, path);

        printf("Request: %s %s\n", method, path);

        if (strcmp(method, "GET") == 0) {
            // Default to index.html for root
            if (strcmp(path, "/") == 0) {
                strcpy(path, "/index.html");
            }

            // Remove leading '/'
            char filename[256];
            snprintf(filename, sizeof(filename), ".%s", path);

            const char *content_type = get_content_type(filename);
            send_file(client_sock, filename, content_type);
        }

        close(client_sock);
    }

    close(server_fd);
    return 0;
}


