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

// Функция для парсинга опций
Flags parse_flags(int argc, char *argv[], char *pattern/*, char *f_file_flag*/) {
  Flags flags = {0};  // жоско инициалзируем все флаги нулями
  int opt;
  int option_index;
  const struct option long_options[] = {{NULL, 0, NULL, 0}};
  // {"regexp", required_argument, NULL, 'e'},
  // {"ignore-case", no_argument, NULL, 'i'},
  // {"invert-match", no_argument, NULL, 'v'},
  // {"count", no_argument, NULL, 'c'},
  // {"files-with-matches", no_argument, NULL, 'l'},
  // {"line-number", no_argument, NULL, 'n'},
  // {NULL, 0, NULL, 0}
  // };
  while ((opt = getopt_long(argc, argv, "e:ivclnsf:", long_options, //optind прниимает индекс элем команд строки
                            &option_index)) != -1) { //гетоптлонг возвр -1 если не можем обработать
    switch (opt) {
      case 'e':
        flags.e = 1;
        strcat(pattern, optarg);
        char divider[2] = "|";
        strcat(pattern, divider);
        printf("switch_e: %s\n", pattern);
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
      case 's':
        flags.s = 1;
        break;
        case 'f':
        flags.f = 1;
        //strcat(f_file_flag, optarg);
        break;
      default:
        printf("Usage: %s  \n", argv[0]);
        exit(1);
    }
  }
  if (flags.e != 1 && flags.f != 1)
    // if (flags.e != 1)
    //   if (flags.f != 1)
    strcat(pattern, argv[optind]);
    char divider[2] = "|";
    strcat(pattern, divider);
    printf("pattern: %s\n", pattern);
  return flags;
}

//жоско зачитаем файлы и красиво выведем sна экран
void grep_print(Flags flags, char *pattern, char *filename/*, char *f_file_flag*/) {
  printf("palku_ne–udalil1 %s\n", pattern);
  int len = strlen(pattern);
  printf("lenusik %d\n", len);
  if (pattern[len - 1] == '|')
      pattern[len - 1] = '\0';
  char buffer[2048];
  FILE *fp;
  // int ch = 0;
  fp = fopen(filename, "r");
  if (fp != NULL) {
  regex_t regex;  //тут хранятся скомпилир рег выражения
  regmatch_t matches[1];  //массив хранит инфу о совпадениях
  int template = 0;  // приравн к результату regexec
  int match_found = 0;
  int cflags = REG_EXTENDED;
  int line_count = 0;
  if (flags.i == 1) cflags = REG_ICASE;
  int comp = regcomp(&regex, pattern, cflags); //компилирует рег выражения в regex
  //printf("regcomp: %d\n", comp); 
  if(comp == 0) {
    while ((fgets(buffer, 228, fp)) != NULL) {  // fgets считывает построчно, дошкл до \n count++
      line_count++;                             //счетчик строк
      template = regexec(&regex, buffer, 1, matches, 0); // regex прогоняет файл через буффер 
      if (template == 0) {match_found++; // если совп найдены то match_found обновляется
        } 
      if (flags.n == 1 && template == 0) printf("%d:", line_count);
      if (template == 0 && flags.c != 1 && flags.l != 1)  //
        printf("%s", buffer); //
    }
      //printf("templ%d\n", template);                            
  }

  if (flags.c == 1 && flags.l != 1) printf("%d", match_found); //печатает match_found/совпадение здесь
  if (flags.l == 1 && flags.c != 1 && match_found > 0) printf("%s\n", filename);
  if (flags.l == 1 && flags.c == 1 && match_found > 0) printf("1\n%s\n", filename);
  if (flags.l == 1 && flags.c == 1 && match_found == 0) printf("0nhh\n");
  fclose(fp);
  regfree(&regex);
  }
//     else
//     exit(1);
    }

int main(int argc, char *argv[]) {
  if(argc != 3) 
    optind++;
  if (argc < 3) {  //учитываем имя программы в качестве первого аргумента.
    printf("Usage: %s  \n", argv[0]);
    exit(1);
  }
  char pattern[2048];
  Flags flags = parse_flags(argc, argv, pattern);
  //printf("optind now:%d\n", optind);
  //printf("elem_optind:%s\n", argv[optind + 1] );
  for(int i = optind; i < argc; i++) {
    FILE *fp;
    if((fp = fopen(argv[i], "r")) != NULL)
      grep_print(flags, pattern, argv[i]);  //optind + 1 это наш файл, optind это pattern
    else if (flags.s != 1 && fp == NULL) {
      printf("net_faila\n");
      fprintf(stderr, "grep: %s: No such file or directory\n", argv[i]);
  }
  }
  printf("palku_udalil %s\n", pattern);
  
  return 0;
}