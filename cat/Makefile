CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11

SRCDIR = ./

BUILDDIR = ./

TARGET1 = s21_cat

all: s21_cat 

s21_cat:
	
	$(CC) $(CFLAGS) cat.c -o $(BUILDDIR)/$(TARGET1)
	@find $(BUILDDIR) -name '*.o' -delete

test:
	bash tests.sh

clean:
	
	rm -f $(BUILDDIR)/*	
	find $(BUILDDIR) -name '*.o' -exec rm -f {} +

rebuild: clean all

.PHONY: all clean rebuild
