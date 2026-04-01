#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024

static struct option options[] = {
    {"number-nonblank", 0, 0, 'b'},
    {"number", 0, 0, 'n'},
    {"squeeze-blank", 0, 0, 's'},
    {0, 0, 0, 0},
};

typedef struct flag {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} flgf;

int process_flag(int argc, char **argv, flgf *flag) {
  int f;
  int err = 0;
  int index = 0;
  while ((f = getopt_long(argc, argv, "benstvTE", options, &index)) != -1) {
    switch (f) {
      case 'b':
        flag->b = 1;
        break;
      case 'E':
        flag->e = 1;
        break;
      case 'n':
        flag->n = 1;
        break;
      case 's':
        flag->s = 1;
        break;
      case 'T': {
        flag->t = 1;
      } break;
      case 'v':
        flag->v = 1;
        break;
      case 'e':
        flag->e = 1;
        flag->v = 1;
        break;
      case 't':
        flag->t = 1;
        flag->v = 1;
        break;
      case '?':
        err = -1;
        break;
    }
  }
  return err;
}

void cat_file(const char *filename, flgf *flag) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Cannot open file: %s\n", filename);
    return;
  }

  char line[MAX_LINE_LENGTH];
  int line_number = 1;
  int prev_blank = 0;

  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    if (flag->s && line[0] == '\n') {
      if (prev_blank) continue;
      prev_blank = 1;
    } else {
      prev_blank = 0;
    }

    int should_print_number = 0;
    if (flag->n || flag->b) {
      if (flag->n || line[0] != '\n') {
        should_print_number = 1;
      }
    }

    if (should_print_number) {
      printf("     %d  ", line_number++);
    }

    for (int i = 0; line[i] != '\0'; i++) {
      if (line[i] == '\t' && flag->t) {
        putchar('^');
        putchar('I');
      } else if (line[i] == '\n' && flag->e) {
        putchar('$');
        putchar('\n');
      } else {
        putchar(line[i]);
      }
    }
  }

  fclose(file);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s [-b] [-e] [-n] [-s] [-t] file1 [file2 ...]\n", argv[0]);
    return 1;
  } else {
    int err_flag = 0;
    flgf flag = {0};
    err_flag = process_flag(argc, argv, &flag);
    if (err_flag == -1) {
      printf("Invalid flag\n");
    } else {
      cat_file(argv[argc - 1], &flag);
    }
  }
  return 0;
}
