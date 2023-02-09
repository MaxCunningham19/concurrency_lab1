/* Test and timing harness program for developing a sorting
   routine for the CS3014 module */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <x86intrin.h>
#include "student_sort.h"

/* the following two definitions of DEBUGGING control whether or not
   debugging information is written out. To put the program into
   debugging mode, uncomment the following line: */
/*#define DEBUGGING(_x) _x */
/* to stop the printing of debugging information, use the following line: */
#define DEBUGGING(_x)

/* this is a comparison function for floats that is being added
   purely to allow the standard qsort function to be used */
int float_compare(const void * ptr2num1, const void * ptr2num2)
{
  float * fpnum1 = (float*) ptr2num1;
  float * fpnum2 = (float*) ptr2num2;
  float num1 = *fpnum1;
  float num2 = *fpnum2;

  if ( num1 > num2 )
    return 1;
  else if ( num1 < num2 )
    return -1;
  else
    return 0;
}

/* sort an array into non-decreasing order */
void david_sort(float a[], int size)
{
  qsort(a, size, sizeof(float), float_compare);
}

/* write out an array of integers up to 'size' */
void write_out(float a[], int size)
{
  int i;

  for ( i = 0; i < size; i++ ) {
    printf("%f\n", a[i]);
  }
}

/* read a stream of float numbers from a file.
   the first number in the file is the number of numbers*/
float * read_in(char filename[], int * ptr2size)
{
  const int max_line = 1024;
  char line[max_line];
  int i;
  FILE * file;
  char * eof;
  float * a;
  int size;

  /* open the file */
  file = fopen(filename, "r");
  if ( file == NULL ) {
    fprintf(stderr, "File not found: %s\n", filename);
    exit(1);
  }

  /* read in the size of the array to allocate */
  eof = fgets(line, max_line, file);
  if ( eof == NULL ) {
    fprintf(stderr, "Empty file: %s\n", filename);
    exit(1);
  }
  sscanf(line, "%d", &size);
  a = malloc(sizeof(long long) * size);

  /* read in the floats - one per line */
  i = 0;
  eof = fgets(line, max_line, file);
  while ( eof != NULL && i < size ) {     /* eof == NULL => end of file */
    sscanf(line, "%f", &(a[i]));
    i++;
    eof = fgets(line, max_line, file);
  }

  fclose(file);
  *ptr2size = size;

  return a;
}

float * copy_array(float * array, int size)
{
  float * result = malloc(sizeof(float) * size);
  int i;

  for ( i = 0; i < size; i++ ) {
    result[i] = array[i];
  }
  return result;
}

/* create an array of length size and fill it with random numbers */
float * gen_random(int size)
{
  float * result = malloc(sizeof(float) * size);
  int i;
  int seed;

  /* use the lower 22 bits of the time stamp counter as a random seed */
  seed = _rdtsc() & ((1LL << 23) - 1);
  srandom(seed);

  /* fill the array with random numbers */
  for ( i = 0; i < size; i++ ) {
    long long upper = random();
    long long lower = random();
    result[i] = (upper << 32) | lower;
  }

  return result;
}

int main(int argc, char ** argv)
{
  float * array;
  float * copy;
  long long sort_time;
  int array_size;
  struct timeval start_time;
  struct timeval stop_time;
  long long start_cycle, stop_cycle;
  int i;

  if ( argc == 3 && !strcmp(argv[1], "-f") ) { /* read data from file */
    char * filename = argv[2];
    array = read_in(filename, &array_size);
  }
  else if ( argc == 3 && !strcmp(argv[1], "-r") ) { /* generate random data */
    array_size = atoi(argv[2]);
    array = gen_random(array_size);
  }
  else {
    fprintf(stderr, "Usage: sort-harness -f <filename> OR sort-harness -r <size>\n");
    exit(1);
  }
  DEBUGGING(write_out(array, array_size));

  /* make a copy of the array, so we can check sorting later */
  copy = copy_array(array, array_size);

  /* record starting time from the processor's time stamp counter */
  gettimeofday(&start_time, NULL);
  start_cycle = _rdtsc();

  /* sort the array somehow */
  /* you should write the code for the student sort */
  student_sort(array, array_size);

  /* record finishing time */
  stop_cycle = _rdtsc();
  gettimeofday(&stop_time, NULL);
  sort_time = (stop_time.tv_sec - start_time.tv_sec) * 1000000L +
    (stop_time.tv_usec - start_time.tv_usec);
  printf("Sorting time: %lld microseconds, %lld cycles\n", sort_time, stop_cycle - start_cycle);

  DEBUGGING(write_out(array, array_size));

  /* sort the copy. use built-in sorting function to check your function */
  david_sort(copy, array_size);

  /* now check that the two are identical */
  for ( i = 0; i < array_size; i++ ) {
    if ( array[i] != copy[i] ) {
      fprintf(stderr, "Error in sorting at position %d\n", i);
    }
  }

  free(array);
  free(copy);
  
  return 0;
}
