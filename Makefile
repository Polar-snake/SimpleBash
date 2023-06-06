cc=gcc -Wall -Werror -Wextra -std=c11
all: check s21_grep

s21_grep: clean
	$(cc) s21_grep.c -o s21_grep

check:
	clang-format -n *.c *.h

clean:
	rm -rf *.o