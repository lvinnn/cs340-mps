#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"
#include "emoji-translate.h"


void emoji_init(emoji_t *emoji) {
  emoji->source = malloc(10*sizeof(char *));
  emoji->translation = malloc(10*sizeof(char *));
  for(int i=0; i<10; i++) {
    emoji->source[i] = malloc(sizeof(char)*50);
    emoji->source[i][0] = 0;
    emoji->translation[i] = malloc(sizeof(char)*50);
    emoji->translation[i][0] = 0;
  }
}

void emoji_add_translation(emoji_t *emoji, const unsigned char *source, const unsigned char *translation) {
  int ind = 0;
  while(emoji->source[ind][0] != 0 && ind != 9) {
    ind++;
  }
  if(strlen(source) >= strlen(emoji->source[ind])) {
    emoji->source[ind] = realloc(emoji->source[ind], 1+strlen(source));
  }
  if(strlen(translation) >= strlen(emoji->translation[ind])) {
    emoji->translation[ind] = realloc(emoji->translation[ind], 1+strlen(translation));
  }
  strcpy(emoji->source[ind], source);
  strcpy(emoji->translation[ind], translation);
}

// Translates the emojis contained in the file `fileName`.
const unsigned char *emoji_translate_file_alloc(emoji_t *emoji, const char *fileName) {
  FILE* ptr = fopen(fileName, "r");
  if (NULL == ptr) {
    return;
  }

  char *str = malloc(sizeof(char *)*100);
  char *translated = malloc(sizeof(char *)*101);
  char *output = malloc(sizeof(char *)*102);
  strcpy(output, "");
  while(fgets(str, 100, ptr) != NULL) {
    strcpy(translated, str);
    int l = strlen(str);
    for(int i=0; i<strlen(str); i++) {
      int longest = 0;
      int ind = -1;
      for(int j=0; j<10; j++) {
        int len = 0;
        int slen = strlen(emoji->source[j]);
        for(int k=0; k<strlen(emoji->source[j]); k++) {
          char c = *(*(emoji->source+j)+k);
          char c2 = *(str+i+k);
          if(i+k < strlen(str) && c == c2) {
            len++;
          } else {
            len = 0;
            break;
          }
        }
        if(len > longest) {
          longest = len;
          ind = j;
        }
      }
      if(ind != -1) {
        strncpy(translated, str, i);
        translated[i] = '\0';
        strcat(translated, emoji->translation[ind]);
        strcat(translated, str + i + longest);
        strcpy(str, translated);
        i = i-longest/4+strlen(emoji->translation[ind]);
      }
    }
    strcat(output, translated);
  }

  free(translated);
  free(str);
  fclose(ptr);
  return output;
  // }
}

void emoji_destroy(emoji_t *emoji) {
  for(int i=0; i<10; i++) {
    free(emoji->source[i]);
    free(emoji->translation[i]);
  }
  free(emoji->source);
  free(emoji->translation);
}
