#include "png-extractGIF.h"

int png_extractGIF(const char *png_filename, const char *gif_filename) {
  PNG *png = PNG_open(png_filename, "r");
  if (!png) { return ERROR_INVALID_FILE; }
  int out = 255;

  // Read chunks until reaching "IEND" or an invalid chunk:
  while (1) {
    // Read chunk and ensure we get a valid result (exit on error):
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      return ERROR_INVALID_CHUNK_DATA;
    }

    // Check for the "IEND" chunk to exit:
    if ( strcmp(chunk.type, "IEND") == 0 ) {
      PNG_free_chunk(&chunk);
      break;  
    } else if ( strcmp(chunk.type, "uiuc") == 0 ) {
      out = 0;
      FILE * gif = fopen(gif_filename, "w");
      if(!gif) { return ERROR_INVALID_FILE; }
      for(unsigned int i=0; i<chunk.len; i++) {
        fputc(*(chunk.data+i), gif);
      }

      fclose(gif);
    }

    // Free the memory associated with the chunk we just read:
    PNG_free_chunk(&chunk);
  }

  PNG_close(png);
  return out;
}