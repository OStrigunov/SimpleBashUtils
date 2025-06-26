#ifndef S21_CAT_H
#define S21_CAT_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct Options {
  int b, e, n, s, t, v;
} flags;

void usage(void);
int iscntrl(int current_symbol);
int change_symbol(int current_symbol);
int v_flag(int current_symbol);
void parsing(flags* opts, int argc, char** argv);
void reading(flags opts, int argc, char** argv);

#endif