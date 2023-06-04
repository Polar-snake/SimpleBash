#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} Flags;

Flags parse_flags(int argc, char *argv[], char *pattern, char *f_file_flag);
void grep_print(Flags flags, char *pattern, char *filename, int file_count);
void f_flag(char *pattern, char *f_file_flag);
void output(Flags flags, int line_count, char *buffer, int start, int end,
            char *filename, int file_count);

int main(int argc, char *argv[]) {
  if (argc < 3) {  //учитываем имя программы в качестве первого аргумента.
    printf("Usage: grep [options] template [file_name]\n");
    exit(1);
  }
  char pattern[2048];
  char f_file_flag[2048];
  int file_count = 0;
  Flags flags = parse_flags(argc, argv, pattern, f_file_flag);
  if (argc == 3 || (flags.e != 1 && flags.f != 1)) optind++;
  file_count = argc - optind;
  // printf("optind now:%d\n", optind);
  // printf("elem_optind:%s\n", argv[optind + 1] );
  if (flags.f == 1) f_flag(pattern, f_file_flag);
  for (int i = optind; i < argc; i++) {
    FILE *fp;
    if ((fp = fopen(argv[i], "r")) != NULL)
      grep_print(flags, pattern, argv[i], file_count);
    else if (flags.s != 1 && fp == NULL) {
      fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
    }
  }
  return 0;
}

// Функция для парсинга опций
Flags parse_flags(int argc, char *argv[], char *pattern, char *f_file_flag) {
  Flags flags = {0};  // жоско инициалзируем все флаги нулями
  int opt, option_index;
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

//жоско зачитаем файлы и красиво выведем на экран
void grep_print(Flags flags, char *pattern, char *filename, int file_count) {
  int len = strlen(pattern);
  if (pattern[len - 1] == '|') pattern[len - 1] = '\0';
  char buffer[2048];
  FILE *fp;
  fp = fopen(filename, "r");
  regex_t regex;  //тут хранятся скомпилир рег выражения
  regmatch_t matches;  //[1];  //массив хранит инфу о совпадениях
  int result = 0;  // приравн к результату regexec
  int match_found = 0;
  int cflags = REG_EXTENDED;
  int line_count = 0;
  if (flags.i == 1) cflags = REG_ICASE;
  int comp = regcomp(&regex, pattern, cflags);  //комп рег выраж в regex
  if (comp == 0) {
    while ((fgets(buffer, 2048, fp)) !=
           NULL) {   // fgets счит постр, дошел до \n count++
      line_count++;  //счетчик строк
      result = regexec(&regex, buffer, 1, &matches, 0); // rege прогн файл ч/з буфф
      if (result == 0) match_found++;  //если совп найдены то match_found обновл
      if ((result == 0 && flags.v != 1) || (flags.v == 1 && result == 1))
        output(flags, line_count, buffer, matches.rm_so, matches.rm_eo,
               filename, file_count);
    }
  }
  if (file_count == 1) {
    if (flags.c == 1 && flags.l != 1)
      printf("%d\n", match_found);  //печатает match_found/совпадение здесь
    if (flags.l == 1 && flags.c != 1 && match_found > 0)
      printf("%s\n", filename);
    if (flags.l == 1 && flags.c == 1 && match_found > 0)
      printf("1\n%s\n", filename);
    if (flags.l == 1 && flags.c == 1 && match_found == 0) printf("0\n");
  } else if (file_count > 1 && (flags.c == 1 || flags.l == 1)) {
    if (flags.h != 1 && (flags.c == 1 || match_found != 0))
      printf("%s", filename);
    if (flags.c == 1 && flags.l != 1 && match_found >= 1)
      printf(":%d\n", match_found);  // c
    if (flags.c == 1 && flags.l != 1 && match_found == 0)
      printf(":%d\n", match_found);
    if (flags.l == 1 && flags.c != 1 && match_found > 0) printf("\n");  // l
    if (flags.l == 1 && flags.c == 1 && match_found > 0)
      printf(":1\n%s\n", filename);  // cl
    if (flags.l == 1 && flags.c == 1 && match_found == 0) printf(":0\n");
  }
  fclose(fp);
  regfree(&regex);
}

void f_flag(char *pattern, char *f_file_flag) {
  FILE *fp;
  int ch = 0;
  int i = 0;
  fp = fopen(f_file_flag, "r");
  if ((fp = fopen(f_file_flag, "r")) != NULL) {
    while ((ch = fgetc(fp)) != EOF) {
      if (ch == 10 || ch == 13)  // символ /n в ascii
        ch = '|';
      pattern[i] = ch;
      i++;
    }
  } else
    fprintf(stderr, "grep: %s: No such file or directory\n", f_file_flag);
  fclose(fp);
}

void output(Flags flags, int line_count, char *buffer, int start, int end,
            char *filename, int file_count) {
  if (flags.c != 1 && flags.l != 1) {
    if (file_count > 1 && flags.h != 1) printf("%s:", filename);
    if (flags.n == 1) printf("%d:", line_count);
    if (flags.o == 1 && flags.v != 1) {
      for (int i = start; i < end; i++) {
        printf("%c", buffer[i]);
      }
      printf("\n");
    }
    // printf("%.*s\n", end - start, &buffer[start]);

    else if (flags.c != 1 && flags.l != 1)
      printf("%s", buffer);
    // printf("\n");
  }
  int len = strlen(buffer);
  if (buffer[len - 1] != '\n') printf("\n");
}