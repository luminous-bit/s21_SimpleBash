CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11

SRCDIR = ./

BUILDDIR = ./

TARGET1 = s21_grep

all: s21_grep 

s21_grep:
	
	$(CC) $(CFLAGS) grep.c -o $(BUILDDIR)/$(TARGET1)	
	@find $(BUILDDIR) -name '*.o' -delete

test:
	bash tests.sh

clean:
	
	rm -f $(BUILDDIR)/*	
	find $(BUILDDIR) -name '*.o' -exec rm -f {} +

rebuild: clean all

.PHONY: all clean rebuild
