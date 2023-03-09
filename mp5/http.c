#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "http.h"


char * parse_helper(char *buffer, ssize_t *st, ssize_t *pos, int inc) {
  char * str = malloc(*pos - *st + 1);
  strncpy(str, buffer+*st, *pos - *st);
  str[*pos-*st] = '\0';
  *pos = *pos + inc;
  *st = *pos;
  // printf("str: %s\n", str);
  return str;
}

void findrn(char *buffer, ssize_t buffer_len, ssize_t *st, ssize_t *pos) {
  while(*pos < buffer_len) {
    if(buffer[*pos] == '\r' && buffer[*pos+1] == '\n') {
      break;
    }
    *pos = *pos + 1;
  }
}

/**
 * httprequest_parse_headers
 * 
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */
ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len) {
  ssize_t pos = 0;
  ssize_t st = 0;
  ssize_t total = 0;

  while(pos < buffer_len && buffer[pos] != ' ') pos++;
  total += pos - st;
  req->action = parse_helper(buffer, &st, &pos, 1);
  
  while(pos < buffer_len && buffer[pos] != ' ') pos++;
  total += pos - st;
  req->path = parse_helper(buffer, &st, &pos, 1);

  findrn(buffer, buffer_len, &st, &pos);
  total += pos - st;
  req->version = parse_helper(buffer, &st, &pos, 2);

  req->head = malloc(sizeof(dic));
  dic *curr = req->head;
  curr->next = NULL;
  curr->key = NULL;
  curr->value = NULL;

  int clen = 0;

  while(1) {
    if(buffer[pos] == '\r' && buffer[pos+1] == '\n') {
      pos += 2;
      break;
    }
    while(pos < buffer_len && buffer[pos+1] != ' ') pos++;
    curr->next = malloc(sizeof(dic));
    curr = curr->next;
    curr->key = parse_helper(buffer, &st, &pos, 2);
    findrn(buffer, buffer_len, &st, &pos);
    curr->value = parse_helper(buffer, &st, &pos, 2);
    curr->next = NULL;

    if(strcmp(curr->key, "Content-Length") == 0) clen = atoi(curr->value);
  }

  // total = total + req->content_length;
  if(clen > 0) {
    void * pload = malloc(clen);
    memcpy(pload, buffer+pos, clen);
    req->payload = pload;
  } else {
    req->payload = NULL;
  }

  return total;
}


/**
 * httprequest_read
 * 
 * Populate a `req` from the socket `sockfd`, returning the number of bytes read to populate `req`.
 */
ssize_t httprequest_read(HTTPRequest *req, int sockfd) {
  char buffer[100000];

  while (1) {
    // recv message:
    // ssize_t len = recv(sockfd, buffer, 4096, 0);
    ssize_t len = read(sockfd, buffer, 100000);
    
    if (len == -1) {
      printf("errno: %d", errno);
      printf("[%d]: socket closed\n", sockfd);
      break;
    }
    // else if (len == 0) {
    //   continue;
    // }
    buffer[len] = '\0';

    return httprequest_parse_headers(req, &buffer[0], len);
  }

  return -1;

}


/**
 * httprequest_get_action
 * 
 * Returns the HTTP action verb for a given `req`.
 */
const char *httprequest_get_action(HTTPRequest *req) {
  return req->action;
}


/**
 * httprequest_get_header
 * 
 * Returns the value of the HTTP header `key` for a given `req`.
 */
const char *httprequest_get_header(HTTPRequest *req, const char *key) {
  dic *curr = req->head;
  while(curr->next != NULL) {
    curr = curr->next;
    if(strcmp(key, curr->key) == 0) {
      return curr->value;
    } 
  }
  //  else if(strcmp(key, "content_length") == 0) {
  //   char str[sizeof(ssize_t)];
  //   sprintf(str, "%ld", *req->content_length);
  //   return str;
  // }
  return NULL;
}


/**
 * httprequest_get_path
 * 
 * Returns the requested path for a given `req`.
 */
const char *httprequest_get_path(HTTPRequest *req) {
  return req->path;
}


/**
 * httprequest_destroy
 * 
 * Destroys a `req`, freeing all associated memory.
 */
void httprequest_destroy(HTTPRequest *req) {

}