#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "webserver.h"
#include "handle_static.h"
#include "handle_calc.h"


void handle_static(int client_socket, const char *path) {
    char file_path[256] = "./static";
    strncat(file_path, path, sizeof(file_path) - strlen(file_path) - 1);

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        send_response(client_socket, "404 Not Found", "text/plain", "File not found.");
        return;
    }

    const char *content_type = "text/plain"; 
    if (strstr(file_path, ".mp4")) {
        content_type = "video/mp4"; // vain attempt to play bad apple in browser :c
    } 
    else if (strstr(file_path, ".html")) {
        content_type = "text/html";
    } 
    else if (strstr(file_path, ".css")) {
        content_type = "text/css";
    } 
    else if (strstr(file_path, ".js")) {
        content_type = "application/javascript";
    }
    else if (strstr(file_path, ".png")) {
        content_type = "image/png";
    } 
    else if (strstr(file_path, ".jpg") || strstr(file_path, ".jpeg")) {
        content_type = "image/jpeg";
    } 
    else if (strstr(file_path, ".gif")) {
        content_type = "image/gif";
    } 
    else if (strstr(file_path, ".svg")) {
        content_type = "image/svg+xml";
    }


    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    snprintf(buffer, sizeof(buffer),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             content_type);
    write(client_socket, buffer, strlen(buffer));

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(client_socket, buffer, bytes_read);
    }

    fclose(file);
}