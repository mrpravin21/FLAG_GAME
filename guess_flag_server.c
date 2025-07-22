#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8080
#define BUF_SIZE 4096

typedef struct Player {
    char roll[50];
    int score;
    struct Player *next;
} Player;

Player *head = NULL;

void update_leaderboard(const char *roll, int score) {
    printf("head pointer at update_leaderboard: %p\n", (void*)head);
    Player *curr = head;

    while (curr) {
        if (strcmp(curr->roll, roll) == 0) {
            if (score > curr->score)
                curr->score = score;
            printf("Updated existing player: %s, new score: %d\n", roll, curr->score);
            return;
        }
        curr = curr->next;
    }

    Player *new_player = (Player *)malloc(sizeof(Player));
    if (!new_player) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    strcpy(new_player->roll, roll);
    new_player->score = score;
    new_player->next = head;
    head = new_player;

    printf("Added new player: %s, score: %d\n", roll, score);

    printf("Current leaderboard:\n");
    Player *temp = head;
    while (temp) {
        printf(" - %s: %d\n", temp->roll, temp->score);
        temp = temp->next;
    }
}

char* generate_leaderboard_html() {
    static char leaderboard[2048];
    strcpy(leaderboard, "<html><body><h1>Leaderboard</h1><ul>");
    Player *curr = head;

    if (curr == NULL) {
        printf("No players in leaderboard\n");
    }

    while (curr) {
        char line[100];
        printf("Adding to HTML: %s - %d\n", curr->roll, curr->score);
        sprintf(line, "<li>%s - %d</li>", curr->roll, curr->score);
        strcat(leaderboard, line);
        curr = curr->next;
    }

    strcat(leaderboard, "</ul></body></html>");
    return leaderboard;
}

char* generate_leaderboard_json() {
    printf("head pointer at generate_leaderboard_json: %p\n", (void*)head);
    static char json[4096];
    strcpy(json, "[");
    Player *curr = head;
    while (curr) {
        char line[128];
        snprintf(line, sizeof(line), "{\"roll\":\"%s\",\"score\":%d}", curr->roll, curr->score);
        strcat(json, line);
        if (curr->next) strcat(json, ",");
        curr = curr->next;
    }
    strcat(json, "]");
    printf("Generated leaderboard JSON: %s\n", json);
    return json;
}

const char* get_content_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return "text/plain";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    return "application/octet-stream";
}

void send_response(int client_sock, const char *body, const char *content_type) {
    char header[512];
    int body_len = strlen(body);
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "\r\n",
             content_type, body_len);
    write(client_sock, header, strlen(header));
    write(client_sock, body, body_len);
}

void handle_client(int client_sock) {
    char buffer[BUF_SIZE] = {0};
    ssize_t bytes_read = read(client_sock, buffer, BUF_SIZE - 1);
    if (bytes_read <= 0) {
        close(client_sock);
        return;
    }
    buffer[bytes_read] = '\0';

    char method[8], path[256];
    sscanf(buffer, "%7s %255s", method, path);
    printf("Request: %s %s\n", method, path);
    char *query_start = strchr(path, '?');
    if (query_start) {
        *query_start = '\0';
    }

    if (strncmp(path, "/submit_score", 13) == 0) {
        char roll[50] = {0};
        int score = 0;

        char *query = strchr(buffer, '?');
        if (query) {
            sscanf(query, "?roll=%49[^&]&score=%d", roll, &score);
            if (roll[0] != '\0') {
                update_leaderboard(roll, score);
                send_response(client_sock, "Score submitted!", "text/plain");
            } else {
                send_response(client_sock, "400 Bad Request", "text/plain");
            }
        } else {
            send_response(client_sock, "400 Bad Request", "text/plain");
        }

    } else if (strcmp(path, "/leaderboard") == 0) {
        char *json = generate_leaderboard_json();
        printf("Sending response:\nHTTP/1.1 200 OK\nContent-Type: application/json\nContent-Length: %lu\n\n%s\n",
        strlen(json), json);
        send_response(client_sock, json, "application/json");

    } else {
        if (strcmp(path, "/") == 0) strcpy(path, "/index.html");
        char filename[256];
        snprintf(filename, sizeof(filename), ".%s", path);

        int fd = open(filename, O_RDONLY);
        if (fd < 0) {
            send_response(client_sock, "404 Not Found", "text/plain");
        } else {
            off_t filesize = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            char header[512];
            snprintf(header, sizeof(header),
                "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %lld\r\n\r\n",
                get_content_type(filename), (long long)filesize);
            write(client_sock, header, strlen(header));

            char file_buffer[BUF_SIZE];
            ssize_t read_bytes;
            while ((read_bytes = read(fd, file_buffer, sizeof(file_buffer))) > 0) {
                write(client_sock, file_buffer, read_bytes);
            }
            close(fd);
        }
    }

    close(client_sock);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int addr_len = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);
    while (1) {
        client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len);
        if (client_sock >= 0) {
            handle_client(client_sock);
            close(client_sock);
        }
    }
    return 0;
}


