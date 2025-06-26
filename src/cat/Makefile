CC = gcc
FLAGS =-Wall -Wextra -Werror

all: s21_cat 

s21_cat: clean
	$(CC) $(FLAGS) s21_cat.c -o s21_cat

clean:
	rm -rf *.o s21_cat

rebuild: clean s21_cat

clang:
	clang-format -n *.c *.h

.PHONY: s21_cat clean rebuild clang 
