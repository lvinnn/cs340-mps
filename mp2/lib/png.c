#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "crc32.h"
#include "png.h"

const int ERROR_INVALID_PARAMS = 1;
const int ERROR_INVALID_FILE = 2;
const int ERROR_INVALID_CHUNK_DATA = 3;
const int ERROR_NO_UIUC_CHUNK = 4;


/**
 * Opens a PNG file for reading (mode == "r" or mode == "r+") or writing (mode == "w").
 * 
 * (Note: The function follows the same function prototype as `fopen`.)
 * 
 * When the file is opened for reading this function must verify the PNG signature.  When opened for
 * writing, the file should write the PNG signature.
 * 
 * This function must return NULL on any errors; otherwise, return a new PNG struct for use
 * with further fuctions in this library.
 */
PNG * PNG_open(const char *filename, const char *mode) {
  PNG * image = malloc(sizeof(PNG));
  image->fp = fopen(filename, mode);
  if (image->fp == NULL) {
    free(image);
    return NULL;
  }

  char signature[8] = {137,80,78,71,13,10,26,10};
  if(mode[0] != 'w') {
    for(int i=0; i<8; i++) {
      char c = fgetc(image->fp);
      if(c != signature[i]) {
        fclose(image->fp);
        free(image);
        return NULL;
      }
    }
  } else {
    for(int i=0; i<8; i++) {
      fputc(signature[i], image->fp);
    }
  }

  return image;
}


/**
 * Reads the next PNG chunk from `png`.
 * 
 * If a chunk exists, a the data in the chunk is populated in `chunk` and the
 * number of bytes read (the length of the chunk in the file) is returned.
 * Otherwise, a zero value is returned.
 * 
 * Any memory allocated within `chunk` must be freed in `PNG_free_chunk`.
 * Users of the library must call `PNG_free_chunk` on all returned chunks.
 */
size_t PNG_read(PNG *png, PNG_Chunk *chunk) {
  // if(feof(png->fp))

  //length
  unsigned int len = 0;
  if(fread(&len, 4, 1, png->fp) == 0) return 0;
  len = ntohl(len);

  //type
  char chonkType[5];
  fread(&chonkType, 1, 4, png->fp);
  chonkType[4] = '\0';

  //data
  char * chonkData;
  if(len > 0) {
    chonkData = malloc(len+1);
    // fread(chonkData, 1, len, png->fp);
    
    for(unsigned int i=0; i<len; i++) {
      char c = fgetc(png->fp);
      chonkData[i] = c;
      // chonkData[i] = fgetc(png->fp);
    }
    // chonkData[len] = '\0';
  } else
    chonkData = NULL;

  //crc
  unsigned int crc = 0;
  fread(&crc, 4, 1, png->fp);
  crc = htonl(crc);

  chunk->len = len;
  strcpy(chunk->type, chonkType);
  chunk->type[4] = '\0';
  if(len > 0) {
    chunk->data = malloc(len+2);
    for(unsigned int i=0; i<len; i++) {
      chunk->data[i] = chonkData[i];
    }
    // strncpy(chunk->data, chonkData, len);
  } else chunk->data = NULL;
  chunk->crc = crc;

  if(len > 0)
    free(chonkData);

  return len + 12;
}


/**
 * Writes a PNG chunk to `png`.
 * 
 * Returns the number of bytes written. 
 */
size_t PNG_write(PNG *png, PNG_Chunk *chunk) {
  unsigned int len = htonl(chunk->len);
  fwrite(&len, 4, 1, png->fp);
  fwrite(&chunk->type, 1, 4, png->fp);
  chunk->type[4] = '\0';
  if(chunk->len > 0)
    fwrite(chunk->data, 1, chunk->len, png->fp);

  char * data = malloc(chunk->len + 5);
  strncpy(data, chunk->type, 4);
  // printf("%s\n", data);
  // if(chunk->len > 0) strcat(data, chunk->data);
  for(unsigned int i=4; i<chunk->len+4; i++) {
    data[i] = chunk->data[i-4];
  }

  chunk->crc = 0;
  // for(unsigned int i=0; i<chunk->len + 5; i++) {
  //   if(!data[i]) data[i] = '\0';
  // }
  crc32(data, 4+chunk->len, &chunk->crc);
  chunk->crc = htonl(chunk->crc);
  free(data);

  fwrite(&chunk->crc, 4, 1, png->fp);

  return 12 + chunk->len;
}


/**
 * Frees all memory allocated by this library related to `chunk`.
 */
void PNG_free_chunk(PNG_Chunk *chunk) {
  if(chunk != NULL && chunk->data != NULL) free(chunk->data);
}


/**
 * Closes the PNG file and frees all memory related to `png`.
 */
void PNG_close(PNG *png) {
  if(png != NULL && png->fp != NULL)
    fclose(png->fp);
  if(png != NULL)
    free(png);
}