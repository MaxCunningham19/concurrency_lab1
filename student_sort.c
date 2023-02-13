#include "sort-harness.h"
#include <xmmintrin.h>

void insertionSort(float arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
 
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

__m128 pick_pivots(float a[],int size){
  int size_delta = (size/5);
  float b[4] = {a[size_delta],a[size_delta*2],a[size_delta*3],a[size_delta*4]};
  insertionSort(b,4);
  __m128 pivots = _mm_loadu_ps(&b);
  return pivots;
}

int switch_it(int mask){
  if ((mask & 8) != 0){ //1000 <p1
    return 0;
  } else if ((mask & 4) != 0){ //0100 p0<x<p1
    return 1;
  } else if ((mask & 2) != 0){ //0010 p1<x<p2
    return 2;
  } else if ((mask & 1) != 0){ //0001 p2<x<p3
    return 3;
  } else if (mask == 0){ //0000 >p3
    return 4;
  }
  return -1;
}


void sample_sort(float a[], int size){
  if (size<=1){
    return;
  }
  if (size<5){
    insertionSort(a,size);
    return;
  }
  
  __m128 pivots = pick_pivots(a, size);

  float** bucket;
  bucket = (float**)malloc(5*sizeof(float*));
  for (int i=0;i<5;i++){
    bucket[i] = (float*)malloc(size*sizeof(float));
  }

  int bucketIndex[5] = {0,0,0,0,0}; 
  for (int i=0;i<size;i++){
    __m128 ai = _mm_load1_ps(&(a[i]));
    int tmpB = switch_it(_mm_movemask_ps(_mm_cmple_ps(ai,pivots)));
    if (tmpB==-1){
      exit(1);
    }
    bucket[tmpB][bucketIndex[tmpB]] = a[i];
    bucketIndex[tmpB] = bucketIndex[tmpB] + 1;
  }

  for (int i=0;i<5;i++){
    sample_sort(bucket[i],bucketIndex[i]);
  }

  int curI = 0;
  for (int b = 0;b<5;b++){
    for (int tmp=0;tmp<bucketIndex[b];tmp++){
      a[curI] = bucket[b][tmp];
      curI = curI + 1;
    }
  }
  
  free(bucketIndex);
  for (int i=0;i<5;i++){
    free(bucket[i]);
  }
  free(bucket);
}


void student_sort(float a[], int size) {
  //david_sort(a, size);
  sample_sort(a,size);
}

//----interesting sort----//
// assuming s4 is in ascending order i.e [0|1|2|3]
int insert(float a,__m128 s4){
  __m128 a4 = _mm_load1_ps(&(a));
  int mask = _mm_movemask_ps(_mm_cmple_ps(a4,s4));
  switch (mask)
  {
  case 0: // 0000 greater than all values
    // pop left most push all down 1 insiert in rightmost
    break;
  default:
    break;
  }
}