#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"
#include "emoji-translate.h"


void emoji_init(emoji_t *emoji) {
}

void emoji_add_translation(emoji_t *emoji, const unsigned char *source, const unsigned char *translation) {
}

// Translates the emojis contained in the file `fileName`.
const unsigned char *emoji_translate_file_alloc(emoji_t *emoji, const char *fileName) {
  return NULL;
}

void emoji_destroy(emoji_t *emoji) {
}
