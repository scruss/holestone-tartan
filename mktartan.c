
/*
# Builds XPM or BMP files from a TDF tartan definition file.
# Copyright (c) 1998 Robert Bradford. All Rights Reserved.
# 
# This software may be freely used for any purpose whatsoever, 
# provided the copyright message is left intact. 
# 
# Have fun.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct Colour
  {
  int value;
  int letter;
  } Colour;
  
int scale;
int cross;
int pixelsize;
Colour colour[26];
int coloursize;
int threadsize[2];
char threads[2][1024];

unsigned long atox(char *p)
  {
  unsigned long result;

  sscanf(p, "%lx", &result);
  return result;
  }

void help(void)
  {
  char **p, *help[] =
    {
    "mktartan [-s] [-c] [-h] <tdf file>",
    "",
    "Mktartan converts a .tdf (tartan definition file) into the equivalent pixmap (.xpm) file,",
    "or bitmap file (.bmp) for Unix or Windows systems respectively. The command line options are:",
    "",
    "  -b    Make a .bmp file output only.",
    "  -s<n> Multiply the scale of the tartan by <n>.",
    "  -c    Make the tartan at a 45 degree angle. (cross)",
    "  -x    Make a .xpm file output only.",
    "  -h   Print this help message and exit.",
    "",
    "The tartan is simply defined by the colours and numbers of threads that will appear in the",
    "weave. Normally, you will only need to define half of the warp (the horizontal part of the",
    "weave). This may be doubled by reflection, and the same pattern used for the weft or woof,",
    "which is the vertical part of the weave. The following commands may be used in the .tdf file.",
    "",
    "#<6 digit hexadecimal number> <letter>",
    "",
    "Defines the colour known by <letter> to have a certain colour value, using the normal rrggbb",
    "colour convention. The colour number must be exactly 6 digits long. Colours must be defined",
    "before they are used. Example: '#ffc800 y' represents a yellow colour.",
    "",
    "r6(g10b10)k1",
    "",
    "states that the weave will contain 6 red threads followed by 10 green, 10 blue and 1 black",
    "thread, provided that the appropriate colour have been defined. After these colours, the",
    "section in brackets will be reversed and appended to the weave. There may only be one pair",
    "of brackets in a pattern line.",
    "",
    "If a file contains one pattern line, the pattern will be used for both the warp and the weft.",
    "Two pattern lines will generate separate warp and weft patterns. More lines is an error. All",
    "tartans only require a single pattern line since they are the same in both directions. However",
    "many tweeds do require two lines, because they are not symmetrical in this way.",
    "",
    "Example Tartan: 'Mar District'",
    "",
    "#ffc800 y",
    "#000000 k",
    "#c80000 r",
    "#004c00 g",
    "",
    "(y1k4g32k4r1)",
    "",
    "Example Tweed: 'Kincardine Estate Tweed'",
    "",
    "#0000c8 b",
    "#00c800 g",
    "#c80000 r",
    "#b9860b n",
    "#8b4513 o",
    "",
    "b1(b1n15r1n14)n1",
    "g1(g1o15r1o14)o1",
    "",
    "When you view the resulting graphic file using your favourite viewer, you may consider that the",
    "tartan can be improved. You can do this either by modifying the .tdf file and running mktartan",
    "again, or by modifying the final graphic image. For example, I often use xpaint to add 5% noise",
    "to the pixmap file, which makes the tartan have a rougher woven appearance.",
    "",
    NULL
    };
    
    for (p = help; *p; p++)
      fprintf(stderr, "%s\n", *p);
    exit(0);
    }

void errmsg(char *format, ...)
  {
  va_list ap;
  
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
  fprintf(stderr,"\n");
  exit(1);
  }
  
void read(char *filename) 
  {
  FILE *ifp;
  char *p, *q, buffer[1024];
  int i, j, index, reflectstart, reflectend, weave, letter, count;
    
  pixelsize = cross ? 1 : 2;
  weave = 0;
  reflectstart = reflectend = -1;
  index = 0;
    
  ifp = fopen(filename, "r");
  if (ifp == NULL)
    errmsg("Cannot open '%s'", filename);
  fgets(buffer, BUFSIZ, ifp);
  while (!feof(ifp))
    {
    p = strchr(buffer, '\n');
    if (p) *p = 0;
    p = buffer;
    if (*p == '#') 
      {
      colour[coloursize].value = atox(p + 1);
      q = strsep(&p, " \t\n");
      i = *p;
      if ('A' <= i && i <= 'Z') p += ' ';
      if (i < 'a' || i > 'z') errmsg("Colour indicator must be a letter");
      colour[coloursize].letter = i;
      coloursize++;
      }
    else if (*p > ' ')
      {  
      if (weave >= 2) errmsg("Too many pattern lines found");
      while (*p)
        {
        if (*p == '(') 
          {
          if (reflectstart != -1) errmsg("Two open brackets found in pattern line");
          p++;
          reflectstart = index;
          }
        else if (*p == ')') 
          {
          p++;
          if (reflectstart < 0)
            errmsg("Close bracket found with no open bracket");
          if (reflectend != -1) errmsg("Two close brackets found in pattern line");
          reflectend = index;
          }
        else
          {     
          letter = *p++;
          if ('A' <= letter && letter <= 'Z') letter += ' ';
          for (i = 0; i < coloursize; i++)
            if (colour[i].letter == letter) break;
          if (i == coloursize) errmsg("Invalid colour '%c' found", letter);
          count = atoi(p);
          if (count < 1) errmsg("Invalid thread count found for colour '%c'", letter);
          while ('0' <= *p && *p <= '9') p++;
          for (j = 0; j < count * scale; j++)
            threads[weave][index++] = i;
          }
        }  
      if (reflectstart >= 0)
        {
        if (reflectend < 0) errmsg("Close bracket missing");
        if (reflectend < reflectstart)
          errmsg("Close bracket before open braket");
        for (i = reflectend - 1; i >= reflectstart; i--) 
          threads[weave][index++] = threads[weave][i];
        }  
      threadsize[weave++] = index;
      reflectstart = reflectend = -1;
      index = 0;
      }  
    fgets(buffer, BUFSIZ, ifp);
    }
  fclose(ifp);
  if (weave == 0) errmsg("No pattern line found");
  if (weave == 1)
    {
    for (i = 0; i < threadsize[0]; i++)
      threads[1][i] = threads[0][i];
    threadsize[1] = threadsize[0];
    }  
  }
    
void write_xpm(char *filename)
  {  
  char *p, buffer[512];
  FILE *ofp;
  int i, j, k;
  
  strcpy(buffer, filename);
  p = strrchr(buffer, '.');
  if (p == NULL)
    p = buffer + strlen(buffer);
  strcpy(p, ".xpm");
  
  ofp = fopen(buffer, "w");
  if (ofp == NULL)
    errmsg("Cannot open file '%s'", buffer);
    
  fprintf(ofp, "/* XPM */\n");
  fprintf(ofp, "static char *test_xpm[] =\n");
  fprintf(ofp, "  {\n");
  fprintf(ofp, "  \"%d %d %d %d\",\n\n", threadsize[0] * pixelsize, 
    threadsize[1] * pixelsize, coloursize, 1);
  for (i = 0; i < coloursize; i++)
    fprintf(ofp, "  \"%c c #%06x\",\n", colour[i].letter, colour[i].value);
  fprintf(ofp, "\n");
  
  for (i = 0; i < threadsize[1] * pixelsize; i++)
    {
    fprintf(ofp, "  \"");
    for (j = 0; j < threadsize[0] * pixelsize; j++)
      {
      if (cross)
        {
        if ((j / pixelsize) & 1)
          k = threads[1][(threadsize[1] + (i + j) / pixelsize) % threadsize[1]];
        else
          k = threads[0][(threadsize[0] + (j - i) / pixelsize) % threadsize[0]];
        } 
      else 
        {
        if (((i + j) / pixelsize) & 1)
          k = threads[0][j / pixelsize];
        else
          k = threads[1][i / pixelsize];
        }
      fprintf(ofp, "%c", colour[k].letter);
      }
    fprintf(ofp, "\"");
    if (i < threadsize[1] * pixelsize - 1) fprintf(ofp, ",");
    fprintf(ofp, "\n");
    }
  fprintf(ofp, "  };");
  fclose(ofp);
  }

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

typedef struct
  {
  WORD    bfType;
  DWORD   bfSize;
  WORD    bfReserved1;
  WORD    bfReserved2;
  DWORD   bfOffBits;
  } BITMAPFILEHEADER;

typedef struct 
  {
  DWORD   biSize;
  DWORD   biWidth;
  DWORD   biHeight;
  WORD    biPlanes;
  WORD    biBitCount;
  DWORD   biCompression;
  DWORD   biSizeImage;
  DWORD   biXPelsPerMeter;
  DWORD   biYPelsPerMeter;
  DWORD   biClrUsed;
  DWORD   biClrImportant;
  } BITMAPINFOHEADER;

typedef struct {
  BYTE    rgbBlue;
  BYTE    rgbGreen;
  BYTE    rgbRed;
  BYTE    rgbReserved;
  } RGBQUAD;

void putword(FILE *fp, WORD word)
  {
  fwrite(&word, sizeof(BYTE), 2, fp);
  }
  
void putdword(FILE *fp, DWORD dword)
  {
  fwrite(&dword, sizeof(BYTE), 4, fp);
  }
  
void write_bmp(char *filename)
  {  
  char *p, buffer[512];
  FILE *ofp;
  int i, j, k;
  
  strcpy(buffer, filename);
  p = strrchr(buffer, '.');
  if (p == NULL)
    p = buffer + strlen(buffer);
  strcpy(p, ".bmp");
  
  ofp = fopen(buffer, "w");
  if (ofp == NULL)
    errmsg("Cannot open file '%s'", buffer);
    
  putword(ofp, 19778); // bfType
  i = 14 + 40 + coloursize * sizeof(RGBQUAD);
  j = i + (threadsize[0] * pixelsize * threadsize[1] * pixelsize * sizeof(BYTE));
  putdword(ofp, j);
  putword(ofp, 0); 
  putword(ofp, 0);
  putdword(ofp, i);
  
  putword(ofp, sizeof(BITMAPINFOHEADER));
  putword(ofp, 0); // fudge
  putdword(ofp, threadsize[0] * pixelsize);
  putdword(ofp, threadsize[1] * pixelsize);
  putword(ofp, 1);
  putword(ofp, 8);
  putdword(ofp, 0); // BI_RGB
  putdword(ofp, j - i);
  putdword(ofp, 0);
  putdword(ofp, 0);
  putdword(ofp, coloursize);
  putdword(ofp, 0);
  
  for (i = 0; i < coloursize; i++)
    putdword(ofp, colour[i].value);

  for (i = 0; i < threadsize[1] * pixelsize; i++)
    {
    for (j = 0; j < threadsize[0] * pixelsize; j++)
      {
      if (cross)
        {
        if ((j / pixelsize) & 1)
          k = threads[1][(threadsize[1] + (i + j) / pixelsize) % threadsize[1]];
        else
          k = threads[0][(threadsize[0] + (j - i) / pixelsize) % threadsize[0]];
        } 
      else 
        {
        if (((i + j) / pixelsize) & 1)
          k = threads[0][j / pixelsize];
        else
          k = threads[1][i / pixelsize];
        }
      fwrite( &k, sizeof(BYTE), 1, ofp);
      }
    }
  fclose(ofp);
  }

void main(int argc, char **argv)
  {
  char *p;
  int bmp, xpm;
  
  cross = 0;
  scale = 0;
  bmp = xpm = 1;
  while (argc > 1 && *argv[1] == '-')
    {
    p = argv[1];
    argc--;
    ++argv;
    switch(p[1])
      {
    case 'b':
      xpm = 0;
      bmp = 1;
      break;
    case 'c':
      cross = 1;
      scale = 2;
      break;
    case 's':
      scale = atoi(p + 2);
      break;
    case 'x':
      xpm = 1;
      bmp = 0;
      break;
    case 'h':
      help();
      break;
      }
    }
  if (argc == 1) help();
  if (scale == 0) scale = cross ? 2 : 1;
  while (--argc)
    {
    read(*++argv);
    if (xpm) write_xpm(*argv);
    if (bmp) write_bmp(*argv);
    }
  }


