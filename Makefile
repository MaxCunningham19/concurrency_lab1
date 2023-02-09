all: student_sort.h sort-harness.h
	gcc -o sort -O0 -Wall -g -fsanitize=address sort-harness.c student_sort.c

opt: student_sort.h sort-harness.h
	gcc -o sort -O3 -march=native  sort-harness.c student_sort.c

test: all
	./sort -r 100000
