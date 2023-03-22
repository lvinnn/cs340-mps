#include "http.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>

void *client_thread(void *vptr) {
  printf("bruh1\n\n\n\n\n");
  int fd = *((int *)vptr);

  HTTPRequest * req = malloc(sizeof(HTTPRequest));
  httprequest_read(req, fd);
  close(fd);
  printf("bruh2\n\n\n\n\n");

  char * response = calloc(10000,1);
  sprintf(response, "%s 200 OK\r\n", req->version);
  dic * curr = req->head;
  while(curr) {
    strcat(response, curr->key);
    strcat(response, ": ");
    strcat(response, curr->value);
    strcat(response, "\r\n");
  }
  strcat(response, "\r\n");
  int i = 0;
  while(response[i] != 0) i++;
  memcpy(response+i, req->payload, atoi(httprequest_get_header(req, "Content-Length")));

  printf("bruh\n\n\n\n\n");
  send(fd, response, strlen(response), 0);

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);
  printf("Binding to port %d. Visit http://localhost:%d/ to interact with your server!\n", port, port);

  // socket:
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // bind:
  struct sockaddr_in server_addr, client_address;
  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);  
  bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));

  // listen:
  listen(sockfd, 10);

  // accept:
  socklen_t client_addr_len;
  while (1) {
    int *fd = malloc(sizeof(int));
    client_addr_len = sizeof(struct sockaddr_in);
    *fd = accept(sockfd, (struct sockaddr *)&client_address, &client_addr_len);
    printf("Client connected (fd=%d)\n", *fd);

    pthread_t tid;
    pthread_create(&tid, NULL, client_thread, fd);
    pthread_detach(tid);
  }

  return 0;
}