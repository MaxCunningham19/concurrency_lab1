#include "sort-harness.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <x86intrin.h>


void print_arr(float a[],int size){
  printf("[");
  for (int i=0;i<size-1;i++){
    printf("%f, ",a[i]);
  }
   printf("%f]\n",a[size-1]);
}

__m128 pick_pivots(float a[],int size){
  int size_delta = (size/5);
  float b[4] = {a[size_delta],a[size_delta*2],a[size_delta*3],a[size_delta*4]};
  david_sort(b,4);
  __m128 pivots = _mm_loadu_ps(&b[0]);
  return pivots;
}

int switch_it(int mask){
 if (mask==0){
  return 4;
 }
 if (mask==8){
  return 3;
 }
 if (mask==12){
  return 2;
 }
 if (mask==14){
  return 1;
 }
 if (mask==15){
  return 0;
 }
  return -1;
}

void sample_sort(float a[], int size){
  if (size<=1){
    return;
  }
  if (size<5){
    david_sort(a,size);
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


  for (int i=0;i<5;i++){
    free(bucket[i]);
  }
  free(bucket);
}

// void is_sorted(float a[],int size){
//   for (int i=1;i<size;i++){
//     if(a[i]<a[i-1]){
//       printf("error:not sorted");
//     }
//   }
//   printf("wediditbois");
// }

//----interesting sort----//

__m128 pick_pivots_isort(float a[],int size){
  int size_delta = (size/5);
  float b[5] = {a[0],a[size_delta],a[size_delta*2],a[size_delta*3],a[size_delta*4]};
  david_sort(b,5);
  float c[4] = {b[1],b[3],b[1],b[3]};
  __m128 pivots = _mm_loadu_ps(&c[0]);
  return pivots;
}

void switch_it_isort(int mask,int bucketSel[]){
  switch (mask)
  {
  case 0:
    bucketSel[0] = 2;
    bucketSel[1] = 2;
    return;
  case 8:
    bucketSel[0] = 2;
    bucketSel[1] = 1;
    return;
  case 12:
    bucketSel[0] = 2;
    bucketSel[1] = 0;
    return;
  case 2:
    bucketSel[0] = 1;
    bucketSel[1] = 2;
    return;
  case 10:
    bucketSel[0] = 1;
    bucketSel[1] = 1;
    return;
  case 14:
    bucketSel[0] = 1;
    bucketSel[1] = 0;
    return;
  case 3:
    bucketSel[0] = 0;
    bucketSel[1] = 2;
    return;
  case 11:
    bucketSel[0] = 0;
    bucketSel[1] = 1;
    return;
  case 15:
    bucketSel[0] = 0;
    bucketSel[1] = 0;
    return;
  default:
    printf("Oh No!!");
    return;
  }
}

void interesting_sort(float a[], int size){
   if (size<=1){
    return;
  }
  if (size<5){
    david_sort(a,size);
    return;
  }
  
  __m128 pivots = pick_pivots_isort(a, size);
  float** bucket;
  bucket = (float**)malloc(3*sizeof(float*));
  for (int i=0;i<3;i++){
    bucket[i] = (float*)malloc(size*sizeof(float));
  }

  int tmpB[2] = {0,0};
  int bucketIndex[3] = {0,0,0}; 
  for (int i=0;i<size;i=i+2){
    float b[4] = {a[i],a[i],a[i+1],a[i+1]};
    __m128 a2 = _mm_loadu_ps(&b[0]);
    
    switch_it_isort(_mm_movemask_ps(_mm_cmple_ps(a2,pivots)), tmpB);

    bucket[tmpB[0]][bucketIndex[tmpB[0]]] = a[i];
    bucketIndex[tmpB[0]] = bucketIndex[tmpB[0]] + 1;
    bucket[tmpB[1]][bucketIndex[tmpB[1]]] = a[i+1];
    bucketIndex[tmpB[1]] = bucketIndex[tmpB[1]] + 1;
  }


  for (int i=0;i<3;i++){
    sample_sort(bucket[i],bucketIndex[i]);
  }

  int curI = 0;
  for (int b = 0;b<3;b++){
    for (int tmp=0;tmp<bucketIndex[b];tmp++){
      a[curI] = bucket[b][tmp];
      curI = curI + 1;
    }
  }


  for (int i=0;i<3;i++){
    free(bucket[i]);
  }
  free(bucket);
}

void student_sort(float array[], int array_size) {
  sample_sort(array,array_size);
}
