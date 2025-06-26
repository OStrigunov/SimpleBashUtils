#include "s21_cat.h"

int main(int argc, char** argv) {
  flags opts = {0};
  parsing(&opts, argc, argv);
  reading(opts, argc, argv);
  return 0;
}

void usage(void) {
  fprintf(stderr, "Usage: ./s21_cat [-benstvET] [FILENAME]\n");
  exit(1);
}

int iscntrl(int current_symbol) {
  return ((current_symbol >= 0 && current_symbol < 32) ||
          (current_symbol == 127));
}

int change_symbol(int current_symbol) {
  if (current_symbol == 127) {
    current_symbol -= 64;
  } else {
    current_symbol += 64;
  }
  return current_symbol;
}

int v_flag(int current_symbol) {
  if (current_symbol != 9 && current_symbol != 10 && iscntrl(current_symbol)) {
    printf("^");
    current_symbol = change_symbol(current_symbol);
  } else if (current_symbol >= 128) {
    printf("M-");
    current_symbol %= 128;
    if (iscntrl(current_symbol)) {
      printf("^");
      current_symbol = change_symbol(current_symbol);
    }
  }
  return current_symbol;
}

void parsing(flags* opts, int argc, char** argv) {
  int option_index = 0, opt;
  static struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                         {"number", 0, 0, 'n'},
                                         {"squeeze-blank", 0, 0, 's'},
                                         {0, 0, 0, 0}};
  while ((opt = getopt_long(argc, argv, "benstvET", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'b':
        opts->b = 1;
        opts->n = 0;
        break;
      case 'e':
        opts->e = 1;
        opts->v = 1;
        break;
      case 'n':
        opts->n = 1;
        break;
      case 's':
        opts->s = 1;
        break;
      case 't':
        opts->t = 1;
        opts->v = 1;
        break;
      case 'v':
        opts->v = 1;
        break;
      case 'E':
        opts->e = 1;
        break;
      case 'T':
        opts->t = 1;
        break;
      default:
        usage();
        break;
    }
  }
  if (opts->b) opts->n = 0;
}

void reading(flags opts, int argc, char** argv) {
  int last_symbol = '\n';
  int current_symbol;
  int str_count = 1;
  int print_s = 0;
  for (int i = optind; i < argc; i++) {
    FILE* f = fopen(argv[i], "r");
    if (f) {
      while ((current_symbol = fgetc(f)) != EOF) {
        if (last_symbol == '\n' && current_symbol == '\n' && opts.s) {
          print_s++;
          if (print_s > 1) continue;
        } else {
          print_s = 0;
        }
        if (last_symbol == '\n' &&
            ((current_symbol != '\n' && opts.b) || opts.n)) {
          printf("%6d\t", str_count++);
        }
        if (opts.t && (current_symbol == '\t')) {
          printf("^");
          current_symbol += 64;
        }
        if (opts.e && current_symbol == '\n') printf("$");
        if (opts.v) {
          current_symbol = v_flag(current_symbol);
        }
        printf("%c", current_symbol);
        last_symbol = current_symbol;
      }
      fclose(f);
    } else {
      fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[optind]);
    }
  }
}