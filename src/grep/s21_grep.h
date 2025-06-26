#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define buffer_size 1024

typedef struct Templates {
  char** templates;
  int count_temp;
} Templates;

typedef struct Flags {
  int e, i, v, c, l, n, h, s, f, o;
} Flags;

void usage(void);
int get_count_files(int argc, int optind);
void add_pattern(Templates* templates, char* pattern);
void free_patterns(Templates* templates);
void flag_o(char* text, Templates templates, int count_files, Flags* opts,
            int flag, char* filename, int nline);
void flag_f(Templates* templates, char* filename);
void flag_v(Flags* opts, int count_files, int nline, char* filename,
            char* text);
void flag_c(Flags* opts, int line_matches, int count_files, char* filename);
void parsing(Flags* opts, Templates* templates, int argc, char** argv);
void search(FILE* file, char* filename, Templates templates, Flags* opts,
            int count_files);

#endif