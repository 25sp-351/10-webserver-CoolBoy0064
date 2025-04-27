#ifndef HANDLE_CALC_H
#define HANDLE_CALC_H

void send_response(int client_socket, const char *status, const char *content_type, const char *body);
void handle_calc(int client_socket, const char *path);

#endif // HANDLE_CALC_H