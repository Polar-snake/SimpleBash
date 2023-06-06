#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: grep [options] template [file_name]\n");
  }
  char pattern[2048] = {0};
  char f_file_flag[2048] = {0};
  int file_count = 0;
  Flags flags = parse_flags(argc, argv, pattern, f_file_flag);
  if (argc == 3 || (flags.e != 1 && flags.f != 1)) optind++;
  file_count = argc - optind;
  if (flags.f == 1) f_flag(pattern, f_file_flag);
  for (int i = optind; i < argc; i++) {
    FILE *fp;
    if ((fp = fopen(argv[i], "r")) != NULL) {
      grep_print(flags, pattern, argv[i], file_count);
      fclose(fp);
    } else if (flags.s != 1 && fp == NULL) {
      fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
    }
  }
  return 0;
}

Flags parse_flags(int argc, char *argv[], char *pattern, char *f_file_flag) {
  Flags flags = {0};
  int opt = 0;
  int option_index = 0;
  const struct option long_options[] = {{NULL, 0, NULL, 0}};
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'e':
        flags.e = 1;
        strcat(pattern, optarg);
        strcat(pattern, "|");
        break;
      case 'i':
        flags.i = 1;
        break;
      case 'v':
        flags.v = 1;
        break;
      case 'c':
        flags.c = 1;
        break;
      case 'l':
        flags.l = 1;
        break;
      case 'n':
        flags.n = 1;
        break;
      case 'h':
        flags.h = 1;
        break;
      case 's':
        flags.s = 1;
        break;
      case 'f':
        flags.f = 1;
        strcat(f_file_flag, optarg);
        break;
      case 'o':
        flags.o = 1;
        break;
      default:
        printf("Usage: grep [options] template [file_name]\n");
    }
  }
  if (flags.e != 1 && flags.f != 1) {
    strcat(pattern, argv[optind]);
    strcat(pattern, "|");
  }
  return flags;
}

void grep_print(Flags flags, char *pattern, char *filename, int file_count) {
  int len = strlen(pattern);
  if (pattern[len - 1] == '|') pattern[len - 1] = '\0';
  char buffer[2048] = {0};
  FILE *fp;
  fp = fopen(filename, "r");
  regex_t regex;
  regmatch_t matches;
  int result = 0;
  int match_found = 0;
  int cflags = REG_EXTENDED;
  int line_count = 0;
  if (flags.i == 1) cflags = REG_EXTENDED | REG_ICASE;
  int comp = regcomp(&regex, pattern, cflags);
  if (comp == 0) {
    while ((fgets(buffer, 2048, fp)) != NULL) {
      line_count++;
      result = regexec(&regex, buffer, 1, &matches, 0);
      if (result == 0) match_found++;
      if ((result == 0 && flags.v != 1) || (flags.v == 1 && result == 1))
        output(flags, line_count, buffer, matches.rm_so, matches.rm_eo,
               filename, file_count, result, regex, matches);
    }
  }
  if (file_count == 1) {
    if (flags.c == 1 && flags.l != 1) printf("%d\n", match_found);
    if (flags.l == 1 && flags.c != 1 && match_found > 0)
      printf("%s\n", filename);
    if (flags.l == 1 && flags.c == 1 && match_found > 0)
      printf("1\n%s\n", filename);
    if (flags.l == 1 && flags.c == 1 && match_found == 0) printf("0\n");
  } else if (file_count > 1 && (flags.c == 1 || flags.l == 1)) {
    if (flags.h != 1 && (flags.c == 1 || match_found != 0))
      printf("%s", filename);
    if (flags.c == 1 && flags.l != 1 && match_found >= 1)
      printf(":%d\n", match_found);
    if (flags.c == 1 && flags.l != 1 && match_found == 0)
      printf(":%d\n", match_found);
    if (flags.l == 1 && flags.c != 1 && match_found > 0) printf("\n");
    if (flags.l == 1 && flags.c == 1 && match_found > 0)
      printf(":1\n%s\n", filename);
    if (flags.l == 1 && flags.c == 1 && match_found == 0) printf(":0\n");
  }
  fclose(fp);
  regfree(&regex);
}

void f_flag(char *pattern, char *f_file_flag) {
  FILE *fp;
  int ch = 0;
  int i = 0;
  if ((fp = fopen(f_file_flag, "r")) != NULL) {
    while ((ch = fgetc(fp)) != EOF) {
      if (ch == 10 || ch == 13) ch = '|';
      pattern[i] = ch;
      i++;
    }
    fclose(fp);
  } else
    fprintf(stderr, "grep: %s: No such file or directory\n", f_file_flag);
}

void output(Flags flags, int line_count, char *buffer, int start, int end,
            char *filename, int file_count, int result, regex_t regex,
            regmatch_t matches) {
  if (flags.c != 1 && flags.l != 1) {
    result = regexec(&regex, buffer, 1, &matches, 0);
    if (flags.v == 1) result = !result;
    if (file_count > 1 && flags.h != 1) printf("%s:", filename);
    if (flags.n == 1) printf("%d:", line_count);
    if (flags.o == 1 /*&& flags.v != 1*/) {
      while (end != start && result == 0) {
        printf("%.*s\n", end - start, &buffer[start]);
        buffer += end;
        result = regexec(&regex, buffer, 1, &matches, 0);
        start = matches.rm_so;
        end = matches.rm_eo;
      }

    } else if (flags.c != 1 && flags.l != 1)
      printf("%s", buffer);
  }
  int len = strlen(buffer);
  if (buffer[len - 1] != '\n' && flags.l != 1 && flags.c != 1) printf("\n");
}