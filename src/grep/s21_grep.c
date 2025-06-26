#include "s21_grep.h"

int main(int argc, char** argv) {
  Flags opts = {0};
  Templates templates = {NULL, 0};
  parsing(&opts, &templates, argc, argv);
  int count_files = get_count_files(argc, optind);
  while (argv[optind] != NULL) {
    FILE* file = fopen(argv[optind], "r");
    if (file) {
      if (templates.count_temp) {
        search(file, argv[optind], templates, &opts, count_files);
        fclose(file);
      }
    } else if (!opts.s) {
      fprintf(stderr, "./s21_grep: %s: No such file or directory\n",
              argv[optind]);
    }
    optind++;
  }
  free_patterns(&templates);
  return 0;
}

void usage(void) {
  fprintf(
      stderr,
      "Usage: ./s21_grep [-ivclnhso] [-e TEMPLATE] [-f FILENAME] [FILENAME]\n");
  exit(1);
}

int get_count_files(int argc, int optind) {
  int count = 0;
  while (optind + count < argc) {
    count++;
  }
  return count;
}

void add_pattern(Templates* templates, char* pattern) {
  if (pattern != NULL) {
    (templates->count_temp)++;
    if (templates->count_temp == 1) {
      templates->templates = calloc(templates->count_temp, sizeof(char*));
    } else {
      templates->templates =
          realloc(templates->templates, sizeof(char*) * templates->count_temp);
    }
    templates->templates[(templates->count_temp) - 1] =
        calloc(strlen(pattern) + 1, sizeof(char));
    strcpy(templates->templates[templates->count_temp - 1], pattern);
  }
}

void free_patterns(Templates* templates) {
  for (int i = 0; i < templates->count_temp; i++) {
    free(templates->templates[i]);
  }
  free(templates->templates);
}

void flag_o(char* text, Templates templates, int count_files, Flags* opts,
            int flag, char* filename, int nline) {
  int step = 0;
  int min_rm_so = 0;
  int min_rm_eo = 0;
  int count_matches;
  do {
    count_matches = 0;
    for (int index = 0; index < templates.count_temp; index++) {
      regex_t regex;
      regmatch_t pmatch;
      regcomp(&regex, templates.templates[index], flag);
      if (regexec(&regex, text + step, 1, &pmatch, 0) == 0) {
        count_matches++;
        if (count_matches == 1 || (min_rm_so > pmatch.rm_so) ||
            (min_rm_so == pmatch.rm_so && min_rm_eo < pmatch.rm_eo)) {
          min_rm_so = pmatch.rm_so;
          min_rm_eo = pmatch.rm_eo;
        }
      }
      regfree(&regex);
    }
    if (count_matches) {
      if (!opts->h && (count_files > 1)) printf("%s:", filename);
      if (opts->n) printf("%d:", nline);
      printf("%.*s\n", (min_rm_eo - min_rm_so), text + step + min_rm_so);
    }
    step += min_rm_eo;
  } while (count_matches);
}

void flag_f(Templates* templates, char* filename) {
  FILE* file = fopen(filename, "r");
  if (file) {
    char str[buffer_size] = {0};
    while (fgets(str, buffer_size - 1, file)) {
      if (strcmp(str, "\n") != 0 || strcmp(str, "\0") != 0) {
        if (str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0';
        add_pattern(templates, str);
      }
    }
  } else {
    fprintf(stderr, "./s21_grep: %s: No such file or directory\n", filename);
    exit(1);
  }
  fclose(file);
}

void flag_v(Flags* opts, int count_files, int nline, char* filename,
            char* text) {
  if ((count_files > 1) && !opts->h && !opts->c && !opts->l && !opts->o)
    printf("%s:", filename);
  if (opts->n && !opts->l && !opts->c && !opts->o) printf("%d:", nline);
  if (!opts->l && !opts->c && !opts->o) printf("%s", text);
}

void flag_c(Flags* opts, int line_matches, int count_files, char* filename) {
  if (opts->l && line_matches) printf("%s\n", filename);
  if ((count_files > 1) && !opts->h && !opts->l) printf("%s:", filename);
  if (!opts->l) printf("%d\n", line_matches);
}

void parsing(Flags* opts, Templates* templates, int argc, char** argv) {
  int option_index = 0, opt = 0;
  static struct option long_options[] = {0};
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'e':
        opts->e = 1;
        add_pattern(templates, optarg);
        break;
      case 'i':
        opts->i = 1;
        break;
      case 'v':
        opts->v = 1;
        break;
      case 'c':
        opts->c = 1;
        break;
      case 'l':
        opts->l = 1;
        break;
      case 'n':
        opts->n = 1;
        break;
      case 'h':
        opts->h = 1;
        break;
      case 's':
        opts->s = 1;
        break;
      case 'f':
        opts->f = 1;
        flag_f(templates, optarg);
        break;
      case 'o':
        opts->o = 1;
        break;
      default:
        usage();
    }
  }
  if (!opts->f && !opts->e && argv[optind] != NULL) {
    add_pattern(templates, argv[optind]);
    optind++;
  }
}

void search(FILE* file, char* filename, Templates templates, Flags* opts,
            int count_files) {
  int flag = (opts->i) ? REG_ICASE : REG_EXTENDED;
  regex_t regex;
  char text[buffer_size];
  regmatch_t pmatch = {0};
  int line_matches = 0, nline = 1;
  while (fgets(text, buffer_size - 1, file)) {
    int print_v = 0;
    for (int i = 0; i < templates.count_temp; i++) {
      int match = 0;
      regcomp(&regex, templates.templates[i], flag);
      int success = regexec(&regex, text, 1, &pmatch, 0);
      if (strchr(text, '\n') == NULL) strcat(text, "\n");
      if (success == 0 && opts->v) print_v = 1;
      if (success == 0 && !opts->v) match = 1;
      if (match && !opts->c && !opts->l && !opts->v) {
        if (!opts->o) {
          if (!opts->h && (count_files > 1)) printf("%s:", filename);
          if (opts->n) printf("%d:", nline);
          printf("%s", text);
        } else {
          flag_o(text, templates, count_files, opts, flag, filename, nline);
        }
      }
      regfree(&regex);
      if (strcmp(text, "\n") != 0) {
        line_matches += match;
      }
      if (match) break;
    }
    if (!print_v && opts->v) {
      line_matches++;
      flag_v(opts, count_files, nline, filename, text);
    }
    nline++;
  }
  if (opts->c) {
    flag_c(opts, line_matches, count_files, filename);
  }
  if (opts->l && !opts->c && line_matches) printf("%s\n", filename);
}