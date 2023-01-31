#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Return your favorite emoji.  Do not allocate new memory.
// (This should **really** be your favorite emoji, we plan to use this later in the semester. :))
char *emoji_favorite() {
  return "\U0001F5FF";
}


// Count the number of emoji in the UTF-8 string `utf8str`, returning the count.  You should
// consider everything in the ranges starting from (and including) U+1F000 up to (and including) U+1FAFF.
int emoji_count(const unsigned char *utf8str) {
  int count = 0;
  for(int i=0; i<strlen(utf8str); i++) {
    int byte = (unsigned int)(utf8str[i]);
    if(byte >= 240) {
      unsigned int val = 0;
      for(int j=i; j<i+4; j++) {
        val = (val << 8) | ((unsigned int)(utf8str[j]));
      }
      if(val >= 4036984960 /* U+1F000 */ && val <= 4036996031 /* U+1FAFF */ ) {
        count++;
        i+=3;
      }
    }
  }
  return count;
}


// Return a random emoji stored in new heap memory you have allocated.  Make sure what
// you return is a valid C-string that contains only one random emoji.
char *emoji_random_alloc() {
  // srand (time(NULL));
  int r;
  char *emoji = (char *)malloc(5*sizeof(char));

  r = rand() % 2816;
  int bruh[4] = {8,0,8,0};
  bruh[3] = r%16;
  r/=16;
  int x = r%16;
  bruh[2] += x%2;
  x/=2;
  bruh[2] += x%2*2;
  x/=2;
  bruh[1] += x%2;
  x/=2;
  bruh[1] += x%2*2;
  r/=16;
  x = r%16;
  bruh[1] += x%2*4;
  x/=2;
  bruh[1] += x%2*8;
  x/=2;
  bruh[0] += x%2;
  x/=2;
  bruh[0] += x%2*2;
  
  // char* emoji = (char *)malloc(100);
  // // 0xF0 0x9F 0x80 0x80 to 0xF0 0x9F 0xAB 0xBF
  // // 32896 to 43967
  emoji[0] = 240;
  emoji[1] = 159;
  emoji[2] = bruh[0]*16+bruh[1];
  emoji[3] = bruh[2]*16+bruh[3];

  // emoji[2] = 171;
  // emoji[3] = 191;
  // // return "\U0001F000";
  // printf("%s\n", emoji);
  // int len = strlen(emoji);
  emoji[4] = '\0';
  return &(*emoji);
}


// Modify the UTF-8 string `utf8str` to invert the FIRST character (which may be up to 4 bytes)
// in the string if it the first character is an emoji.  At a minimum:
// - Invert "😊" U+1F60A ("\xF0\x9F\x98\x8A") into ANY non-smiling face.
// - Choose at least five more emoji to invert.
void emoji_invertChar(unsigned char *utf8str) {
  unsigned int val = 0;
  if(strlen(utf8str) < 4) return;
  for(int j=0; j<4; j++) {
    val = (val << 8) | ((unsigned int)(utf8str[j]) & 0xFF);
  }
  if(val == 4036991114) {
    int rep[4] = {0xF0, 0x9F, 0x99, 0x81};
    replace(utf8str, rep);
  } else if(val == 4036994486) {
    int rep[4] = {0xF0, 0x9F, 0xA5, 0xB5};
    replace(utf8str, rep);
  } else if(val == 4036994481) {
    int rep[4] = {0xF0, 0x9F, 0x98, 0xA0};
    replace(utf8str, rep);
  } else if(val == 4036989373) {
    int rep[4] = {0xF0, 0x9F, 0x91, 0xBB};
    replace(utf8str, rep);
  } else if(val == 4036994209) {
    int rep[4] = {0xF0, 0x9F, 0x92, 0xA9};
    replace(utf8str, rep);
  } else if(val == 4036991165) {
    int rep[4] = {0xF0, 0x9F, 0x98, 0xBF};
    replace(utf8str, rep);
  }
}

void replace(char *utf8str, int rep[]) {
  for(int i=0; i<4; i++) {
    utf8str[i] = rep[i];
  }
}



// Modify the UTF-8 string `utf8str` to invert ALL of the character by calling your
// `emoji_invertChar` function on each character.
void emoji_invertAll(unsigned char *utf8str) {
  for(int i=0; i<strlen(utf8str); i++) {
    emoji_invertChar(&utf8str[i]);
  }
}


// Reads the full contents of the file `fileName, inverts all emojis, and
// returns a newly allocated string with the inverted file's content.
unsigned char *emoji_invertFile_alloc(const char *fileName) {
  FILE* ptr = fopen(fileName, "r");
  if (NULL == ptr) {
    return;
  }

  char *str = (char *)malloc(100);
  fgets(str, 100, ptr);

  emoji_invertAll(str);
  fclose(ptr);
  return str;
}
