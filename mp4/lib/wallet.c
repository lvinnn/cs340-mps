#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include "wallet.h"

/**
 * Initializes an empty wallet.
 */
void wallet_init(wallet_t *wallet) {
  wallet->head = malloc(sizeof(res));

  wallet->head->count = 0;
  // wallet->head->name = malloc(1);
  wallet->head->name = NULL;
  wallet->head->next = NULL;

  pthread_mutex_init(&wallet->lock, NULL);
  pthread_cond_init(&wallet->cond, NULL);
}

/**
 * Returns the amount of a given `resource` in the given `wallet`.
 */
int wallet_get(wallet_t *wallet, const char *resource) {
  // Implement `wallet_get`
  pthread_mutex_lock(&wallet->lock);

  res *curr = wallet->head->next;
  while(curr != NULL && strcmp(curr->name, resource) != 0) curr = curr->next;
  if(curr == NULL) {
    pthread_mutex_unlock(&wallet->lock);
    return 0;
  }

  int n = curr->count;
  pthread_mutex_unlock(&wallet->lock);
  return n;
}

/**
 * Modifies the amount of a given `resource` in a given `wallet by `delta`.
 * - If `delta` is negative, this function MUST NOT RETURN until the resource can be satisfied.
 *   (Ths function MUST BLOCK until the wallet has enough resources to satisfy the request.)
 * - Returns the amount of resources in the wallet AFTER the change has been applied.
 */
int wallet_change_resource(wallet_t *wallet, const char *resource, const int delta) {
  // Implement `wallet_change_resource`
  pthread_mutex_lock(&wallet->lock);

  res *curr = wallet->head->next;
  res *last = wallet->head;
  while(curr != NULL && strcmp(curr->name, resource) != 0) {
    last = curr;
    curr = curr->next;
  }
  if(curr == NULL) {
    res *r = malloc(sizeof(res));
    r->count = 0;
    r->name = malloc(strlen(resource)+1);
    strcpy(r->name, resource);
    r->next = NULL;
    last->next = r;
    curr = r;
  }

  while(curr->count + delta < 0)
    pthread_cond_wait(&wallet->cond, &wallet->lock);

  curr->count += delta;
  int n = curr->count;
  pthread_cond_broadcast(&wallet->cond);
  pthread_mutex_unlock(&wallet->lock);
  return n;
}

/**
 * Destroys a wallet, freeing all associated memory.
 */
void wallet_destroy(wallet_t *wallet) {
  // Implement `wallet_destroy
  res *curr = wallet->head;
  while(curr->next != NULL) {
    free(curr->name);
    res *next = curr->next;
    free(curr);
    curr = next;
  }
  free(curr->name);
  free(curr);
}