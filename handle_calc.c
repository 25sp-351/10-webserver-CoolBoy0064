#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "webserver.h"
#include "handle_calc.h"
#include "handle_static.h"

void send_response(int client_socket, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    int content_length = strlen(body);

    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             status, content_type, content_length, body);

    write(client_socket, response, strlen(response));
}


void handle_calc(int client_socket, const char *path) {
    char operation[16];
    int a, b, result;
    char response_body[128];

    printf("Handling calculation for path: %s\n", path);
    // Parse the path components (e.g., /calc/add/1/2)
    if (sscanf(path, "%15[^/]/%d/%d", operation, &a, &b) != 3) {
        send_response(client_socket, "400 Bad Request", "text/plain", "Invalid path format.");
        return;
    }

    // Perform the requested operation
    if (strcmp(operation, "add") == 0) {
        result = a + b;
    } 
    else if (strcmp(operation, "sub") == 0) {
        result = a - b;
    } 
    else if (strcmp(operation, "mul") == 0) {
        result = a * b;
    } 
    else if (strcmp(operation, "div") == 0) {
        if (b == 0) {
            send_response(client_socket, "400 Bad Request", "text/plain", "Dividing by zero.");
            return;
        }
        result = a / b;
    } 
    else {
        send_response(client_socket, "400 Bad Request", "text/plain", "Invalid operation.");
        return;
    }
    snprintf(response_body, sizeof(response_body), "Result: %d", result);
    send_response(client_socket, "200 OK", "text/plain", response_body);
}