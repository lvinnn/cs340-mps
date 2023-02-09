#include "png-hideGIF.h" 

#include "lib/crc32.h"
#include <arpa/inet.h>

int png_hideGIF(const char *png_filename_source, const char *gif_filename, const char *png_filename_out) {
    // Open the file specified in argv[1] for reading and argv[2] for writing:
  PNG *png = PNG_open(png_filename_source, "r");
  if (!png) { return ERROR_INVALID_FILE; }

  PNG *out = PNG_open(png_filename_out, "w");

  // Read chunks until reaching "IEND" or in invalid chunk:
  while (1) {
    // Read chunk and ensure we get a valid result (exit on error):
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      PNG_close(out);
      return ERROR_INVALID_CHUNK_DATA;
    }

    // Check for the "IEND" chunk to exit:
    if ( strcmp(chunk.type, "IEND") == 0 ) {
      FILE* gif = fopen(gif_filename, "r");
      fseek(gif, 0, SEEK_END);
      int len = ftell(gif);
      fclose(gif);
      gif = fopen(gif_filename, "r");
      char* gifData = malloc(len+4);
      for(int i=0; i<len; i++) {
        char c = fgetc(gif);
        gifData[i+4] = c;
      }
      int aiya = htonl(len);
      fwrite(&aiya, 4, 1, out->fp); //length
      fwrite("uiuc", 1, 4, out->fp); //type
      for(int i=4; i<len+4; i++) { //data
        fputc(gifData[i], out->fp);
      }
      // fwrite(gifData, 1, len, out->fp); //data
      unsigned int crc = 0;
      gifData[0] = 'u';
      gifData[1] = 'i';
      gifData[2] = 'u';
      gifData[3] = 'c';
      // strncat(gifData, "uiuc", 5);
      crc32(gifData, len+4, &crc);
      crc = htonl(crc);
      fwrite(&crc, 4, 1, out->fp);
      free(gifData);

      PNG_write(out, &chunk);
      PNG_free_chunk(&chunk);
      fclose(gif);
      break;  
    } else {
      PNG_write(out, &chunk);
      PNG_free_chunk(&chunk);
    }
  }

  PNG_close(out);
  PNG_close(png);
  return 0;
}
