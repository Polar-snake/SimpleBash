#include "s21_cat.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: cat [-benstv] [file ...]\n");
  }
  Flags flags = parse_flags(argc, argv);
  for (int i = optind; i < argc; i++) {
    read_print(argv[i], flags);
  }
  return 0;
}

void read_print(char *filename, Flags flags) {
  FILE *fp;
  int ch = 0;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory\n", filename);
  } else {
    int d = 0;
    char lastC = '\n';
    int count_empty = 0;
    while ((ch = fgetc(fp)) != EOF) {
      if (flags.b == 1 && flags.n == 1) {
        flags.b = 1;
        flags.n = 0;
      }
      if (flags.b == 1 && lastC == '\n' && ch != '\n') {
        d++;
        printf("%6d\t", d);
      }
      if (flags.s == 1) {
        if (ch == '\n' && lastC == '\n')
          count_empty++;
        else
          count_empty = 0;
        if (count_empty > 1) continue;
      }
      if (flags.n == 1 && lastC == '\n') {
        d++;
        printf("%6d\t", d);
      }
      if (flags.e == 1 && ch == '\n') printf("$");
      if (flags.t == 1 && ch == '\t') {
        ch = 'I';
        printf("^");
      }
      if (flags.v == 1) {
        if (ch == 127) {
          printf("^");
          ch = '?';
        } else if ((ch >= 0 && ch < 9) || (ch > 10 && ch < 32)) {
          putchar('^');
          ch += 64;
        }
      }
      putchar(ch);
      lastC = ch;
    }
    fclose(fp);
  }
}

Flags parse_flags(int argc, char *argv[]) {
  Flags flags = {0, 0, 0, 0, 0, 0, 0, 0};
  int opt = 0;
  int option_index = 0;
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'}};
  while ((opt = getopt_long(argc, argv, "+benstETv", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'b':
        flags.b = 1;
        break;
      case 'e':
        flags.e = 1;
        flags.v = 1;
        break;
      case 'n':
        flags.n = 1;
        break;
      case 's':
        flags.s = 1;
        break;
      case 't':
        flags.t = 1;
        flags.v = 1;
        break;
      case 'E':
        flags.e = 1;
        break;
      case 'T':
        flags.t = 1;
        break;
      case 'v':
        flags.v = 1;
        break;
      default:
        printf("Usage: cat [-benstv] [file ...]\n");
    }
  }
  return flags;
}
