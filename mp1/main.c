#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"
#include "emoji-translate.h"

int main() {
  emoji_t emoji;
  emoji_init(&emoji);

  emoji_add_translation(&emoji, (const unsigned char *)"😊", (const unsigned char *)"");

  unsigned char *translation = (unsigned char *) emoji_translate_file_alloc(&emoji, "tests/txt/text-inbetween.txt");
  // REQUIRE(translation != NULL);

  // INFO("translation := " << translation);
  // REQUIRE(strcmp((char *) translation, " CS340 ") == 0);
  free(translation);

  emoji_destroy(&emoji);
}