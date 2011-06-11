#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sac.h"

typedef char* string;

#define array_nt  (array, T_OLD((AKD, (NHD, (NUQ, )))))
#define ret_nt    (ret,   T_OLD((AKD, (NHD, (NUQ, )))))

#define MAXLINE 80

/*
 * Read a pgm (ascii or binary) image.
 */
void SAC_PGM_pgm2array( SAC_ND_PARAM_out( array_nt, int),
                        FILE *fp)
{
  char line[MAXLINE];
  int w;
  int h;
  int max;
  int *data;
  unsigned int c;
  bool binary;
  bool comment;

  /*
   * Check for a valid header
   */
  if (fgets(line, MAXLINE, fp) == NULL) {
    SAC_RuntimeError( "Unexpected error when reading file");
  }
  if ( (line[0] != 'P' && line[1] != '2') || (line[0] != 'P' && line[1] != '5') ) {
    SAC_RuntimeError( "Not reading a pgm file!");
  }

  /*
   * Check if we are reading an ascii or binary file
   */
  if (line[1] == '2') {
    binary = false;
  } else {
    binary = true;
  }

  /*
   * Check for comment (and ignore it)
   */
  while(true) {
    if (fgets(line, MAXLINE, fp) == NULL) {
      SAC_RuntimeError( "Unexpected error when reading file");
    }

    /*
     * Check for a comment line
     */
    if (line[0] == '#') {
      comment = true;

      while(comment == true) {
        for(int i = 0; i < MAXLINE; i++) {
          if (line[i] == '\n') {
            comment = false;
            break;
          }
        }

        if (comment == true) {
          fgets(line, MAXLINE, fp);
        }
      }
    } else {
      break;
    }
  }

  /*
   * Get the dimensions of the image
   * and allocate an array of sufficient size
   */
  if (sscanf(line, "%d %d", &w, &h) != 2) {
    SAC_RuntimeError( "Unexpected error when reading file");
  }
  data = malloc(sizeof(int) * w * h);
  if (data == NULL) {
    SAC_RuntimeError( "Could not allocate data for image array");
  }

  /*
   * Read out the maximum gray value
   */
  if (fgets(line, MAXLINE, fp) == NULL) {
    SAC_RuntimeError( "Unexpected error when reading file");
  }
  if (sscanf(line, "%d", &max) != 1) {
    SAC_RuntimeError( "Unexpected error when reading file");
  }

  /*
   * Readout the image
   */
  if (binary == true) {
    for (int i = 0; i < h*w; i++) {
      if(fread(&c, 1, 1, fp) != 1) {
        SAC_RuntimeError( "Unexpected end of file" );
      }
      data[i] = c * (255/max);
    }
  } else {
    for (int i = 0; i < h*w; i++) {
      if( fscanf(fp, "%u", &c) != 1) {
        SAC_RuntimeError( "Unexpected end of file");
      }
      data[i] = c * (255/max);
    }
  }

  /*
   * Create the SAC resturn array & descriptor
   */
  SAC_ND_DECL__DATA( ret_nt, int, )
  SAC_ND_DECL__DESC( ret_nt, )
  int SAC_ND_A_MIRROR_DIM( ret_nt) = 2;
  SAC_ND_ALLOC__DESC( ret_nt, dims)
  SAC_ND_SET__RC( ret_nt, 1)
  SAC_ND_A_DESC_SHAPE( ret_nt, 0) = h;
  SAC_ND_A_DESC_SHAPE( ret_nt, 1) = w;
  SAC_ND_A_FIELD( ret_nt) = data;
  SAC_ND_RET_out( array_nt, ret_nt)
}
         
