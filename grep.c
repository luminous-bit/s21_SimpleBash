#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int line_count;
  int match_count;
} Counter;

size_t getline_sh(char **line, size_t *n, FILE *stream) {
  if (!line || !n || !stream) {
    return -1;
  }

  if (*line == NULL) {
    *n = 128;
    *line = (char *)malloc(*n);
    if (*line == NULL) {
      return -1;
    }
  }

  size_t pos = 0;
  int c;
  while ((c = fgetc(stream)) != EOF && c != '\n') {
    (*line)[pos++] = c;
    if (pos >= *n) {
      *n *= 2;
      char *temp = (char *)realloc(*line, *n);
      if (temp == NULL) {
        return -1;
      }
      *line = temp;
    }
  }

  if (c == EOF && pos == 0) {
    return -1;
  }

  (*line)[pos] = '\0';
  return pos;
}

int process_line(char *line, regex_t *regex, bool invert_match,
                 bool print_count, bool print_filename, bool print_line_number,
                 bool print_match_only, bool flag_h, char *pattern,
                 char *filename, int count_files, int check, Counter *counter) {
  int reti;
  reti = (regexec(regex, line, 0, NULL, 0) == 0) ^ (invert_match);
  if (reti) {
    counter->match_count++;
    if (count_files > 1 && !print_line_number && !flag_h && !print_filename &&
        !print_count) {
      printf("%s:", filename);
    }

    if (count_files > 1 && print_line_number && !flag_h) {
      printf("%s:%d:", filename, counter->line_count);
    } else if ((count_files == 1 && print_line_number) ||
               (count_files > 1 && print_line_number && flag_h)) {
      printf("%d:", counter->line_count);
    }

    if (!print_count && !print_match_only && !print_filename)
      printf("%s\n", line);
    if (print_match_only) {
      if (strstr(line, pattern) != NULL) {
        printf("%s\n", pattern);
      }
    }
    if (print_filename) check = 1;
  }
  return check;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf(
        "Usage: %s [-e] [-i] [-v] [-c] [-l] [-n] pattern file1 [file2 ...]\n",
        argv[0]);
    return 1;
  }

  bool ignore_case = false;
  bool invert_match = false;
  bool print_count = false;
  bool print_filename = false;
  bool print_line_number = false;
  bool print_match_only = false;
  bool flag_h = false;
  int check = 0;
  int count = 0;
  char *pattern = NULL;

  if (strcmp(argv[1], "-e") == 0) {
    if (2 < argc) {
      pattern = argv[2];
      count = 2;
    } else {
      printf("No argument for the -e flag\n");
      return 1;
    }
  } else if (strcmp(argv[1], "-i") == 0) {
    ignore_case = true;
    count = 2;
  } else if (strcmp(argv[1], "-v") == 0) {
    invert_match = true;
    count = 2;
  } else if (strcmp(argv[1], "-c") == 0) {
    print_count = true;
    count = 2;
  } else if (strcmp(argv[1], "-l") == 0) {
    print_filename = true;
    count = 2;
  } else if (strcmp(argv[1], "-n") == 0) {
    print_line_number = true;
    count = 2;
  } else if (strcmp(argv[1], "-o") == 0) {
    print_match_only = true;
    count = 2;
  } else if (strcmp(argv[1], "-h") == 0) {
    flag_h = true;
    count = 2;
  } else {
    pattern = argv[1];
    count = 1;
  }

  if (pattern == NULL) {
    pattern = argv[2];
  }

  int count_files = argc - count - 1;

  regex_t regex;
  int regex_flags = REG_EXTENDED;
  if (ignore_case) regex_flags |= REG_ICASE;
  if (regcomp(&regex, pattern, regex_flags) != 0) {
    printf("Error compiling regular expression\n");
    return 1;
  }

  for (int i = count + 1; i < argc; i++) {
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      printf("Error opening file: %s\n", argv[i]);
    }

    char *line = NULL;
    size_t len = 0;
    size_t read;

    Counter counter = {1, 0};

    while ((int)(read = getline_sh(&line, &len, file)) != -1) {
      check =
          process_line(line, &regex, invert_match, print_count, print_filename,
                       print_line_number, print_match_only, flag_h, pattern,
                       argv[i], count_files, check, &counter);
      counter.line_count++;
      if (check == 1) {
        printf("%s\n", argv[i]);
        check = 0;
        break;
      }
    }

    if ((print_count && count_files == 1) || (print_count && flag_h)) {
      printf("%d\n", counter.match_count);
    } else if (print_count && count_files > 1) {
      printf("%s:%d\n", argv[i], counter.match_count);
    }

    free(line);
    fclose(file);
  }

  regfree(&regex);

  return 0;
}
