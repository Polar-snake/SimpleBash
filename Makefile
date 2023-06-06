cc=gcc -Wall -Werror -Wextra -std=c11
all: check s21_cat

s21_cat: clean
	$(cc) s21_cat.c -o s21_cat

check:
	clang-format -n *.c *.h

clean:
	rm -rf *.o