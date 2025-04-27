#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "webserver.h"
#include "handle_static.h"
#include "handle_calc.h"

#define DEFAULT_PORT 8080

bool running = true;

void handle_client(int client_socket);

void *client_thread(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr; // dereference and cast to int
    free(client_socket_ptr);
    handle_client(client_socket);
    close(client_socket);
    return NULL;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return;
    }

    buffer[bytes_read] = '\0';

    char method[16];
    char path[256];
    char protocol[16];

    sscanf(buffer, "%15s %255s %15s", method, path, protocol);

    if (strcmp(method, "GET") != 0) {
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Only GET method is supported.");
        return;
    }

    if (strncmp(path, "/static", 7) == 0) {
        handle_static(client_socket, path + 7);
    } 
    else if (strncmp(path, "/calc", 5) == 0) {
        handle_calc(client_socket, path + 6); // Pass the path after "/calc/"
    } 
    else if (strncmp(path, "/quit", 5) == 0){
        running = false;
        send_response(client_socket, "200 OK", "text/plain", "Server is shutting down.");
        return;
    } 
    else if (strcmp(path, "/") == 0) {
        send_response(client_socket, "200 OK", "text/html", "<html><body><h1>Welcome to the Web Server!</h1></body></html>");
    }
    else {
        send_response(client_socket, "404 Not Found", "text/plain", "The requested resource was not found.");
    }
}

int main(int argc, char *argv[]) {
    int server_socket;
    int client_socket; 
    int port;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);

    if (argc > 1){
        port = atoi(argv[1]);
    }
    else {
        port = DEFAULT_PORT;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE); //use exit instead of return to avoid undefined behavior
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    while (running) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        pthread_t thread;
        pthread_create(&thread, NULL, client_thread, client_socket_ptr);
        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}