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
            char *filename, int file_count, int result, regex_t regex,
            regmatch_t matches);