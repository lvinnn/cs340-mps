#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "lib/wallet.h"

// Your server must use only one global wallet:
wallet_t wallet;

void *client_communication_thread(void *vptr_fd) {
  int fd = *((int *)vptr_fd);
  char buffer[4096];

  while (1) {
    // recv message:
    ssize_t len = recv(fd, buffer, 4096, 0);
    if (len == -1) {
      printf("[%d]: socket closed\n", fd);
      break;
    } else if (len == 0) { 
      continue;
    }
    buffer[len] = '\0';
    
    int pos = 0;
    while(buffer[pos] != ' ' && buffer[pos] != '\0') {
      pos++;
    }
    char* mes = malloc(pos+1);
    strncpy(mes, buffer, pos);
    mes[pos] = '\0';
    pos++;
    if(strcmp(mes, "GET") == 0) {
      char * name = malloc(len-pos);
      strncpy(name, &buffer[pos], len-pos-1);
      name[len-pos] = '\0';
      int n = wallet_get(&wallet, name);
      sprintf(buffer, "%d\n", n);
      send(fd, buffer, strlen(buffer), 0);
    } else if(strcmp(mes, "MOD") == 0) {
      int resource_start= pos;
      while(buffer[pos] != ' ' && buffer[pos] != '\0') {
        pos++;
      }
      char * name = malloc(pos - resource_start+1);
      strncpy(name, &buffer[resource_start], pos-resource_start);
      name[pos-resource_start] = '\0';
      pos++;
      char * delta = malloc(len-pos);
      strncpy(delta, &buffer[pos], len-pos-1);
      delta[len-pos] = '\0';
      int d = atoi(delta);
      int n = wallet_change_resource(&wallet, name, d);
      sprintf(buffer, "%d\n", n);
      send(fd, buffer, strlen(buffer), 0);
    } else if(strcmp(mes, "EXIT\n")) {
      break;
    }
  }

  return NULL;
}

void create_wallet_server(int port) {
  // socket:
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd <= 0) { perror("socket"); return; }
  printf("socket: returned fd=%d\n", sockfd);

  // bind:
  struct sockaddr_in server_addr;
  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);  
  if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
    perror("bind");
    return;
  }
  printf("bind: binding fd=%d to port %d (:%d)\n", sockfd, port, port);

  // listen:
  if (listen(sockfd, 10) != 0) {
    perror("listen");
    return;
  }
  printf("listen: fd=%d is now listening for incoming connections\n", sockfd);

  // continue to accept new connections forever:
  while (1) {
    struct sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    // accept:
    int *fd = malloc(sizeof(int));
    *fd = accept(sockfd, (struct sockaddr *)&client_address, &client_addr_len);
    
    char *ip = inet_ntoa(client_address.sin_addr);
    printf("accept: new client connected from %s, communication on fd=%d\n", ip, *fd);

    pthread_t tid;
    pthread_create(&tid, NULL, client_communication_thread, fd);
  }
}

int main(int argc, char* argv[]) {
  int c;
  int local_port = 34000;

  // Reads the (optional) command line argument:
  while((c = getopt(argc, argv, "p:")) != -1) {
    switch(c) {
      case 'p':
        if(optarg != NULL) {
          local_port = atoi(optarg);
        }
        break;
    }
  }

  // Calls `wallet_init`:
  wallet_init(&wallet);

  // Calls the `create_wallet_server` with the user-supplied port:
  printf("Launching wallet server on :%d\n", local_port);
  create_wallet_server(local_port);
}
