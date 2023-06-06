#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int E;
  int T;
  int v;
} Flags;

void read_print(char *filename, Flags flags);
Flags parse_flags(int argc, char *argv[]);