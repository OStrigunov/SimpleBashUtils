CC = gcc
FLAGS = -Wall -Wextra -Werror

all: s21_grep

s21_grep:
	$(CC) $(FLAGS) s21_grep.c -o s21_grep

clean:
	rm -rf *.o s21_grep

rebuild: clean s21_grep

clang:
	clang-format -n *.c *.h

.PHONY: s21_grep clean rebuild clang
