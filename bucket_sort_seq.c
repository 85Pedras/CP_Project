#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "bucket_sort_seq.h"

typedef struct {
    int n;  // número de elementos de cada bucket - 1
    int bucket[BUCKET_SIZE];
} bucket;
                                                 
void bucket_sort (int v[],int size){      
    bucket bs[N_BUCKETS]; //array de buckets                                  
    int i,j,k;

    for(i=0;i<N_BUCKETS;i++) bs[i].n=0; //inicializa todos os "n"

    for(i=0;i<size;i++){  //verifica em que bucket o elemento deve ficar
        j=(N_BUCKETS)-1;
        while(1){
            if(j<0) break;
            if(v[i]>=j*10){
                bs[j].bucket[bs[j].n]=v[i];
                (bs[j].n)++;
                break;
            }
            j--;
        }
    }

    for(i=0;i<N_BUCKETS;i++){  //ordena os buckets
        if(bs[i].n) quicksort(bs[i].bucket,0,(bs[i].n)-1);
    }
         
    i=0;
    for(j=0;j<N_BUCKETS;j++){       //põe os elementos dos buckets de volta no vetor
        for(k=0;k<bs[j].n;k++){
            v[i]=bs[j].bucket[k];
            i++;
        }
    }
}
 
void quicksort(int v[],int first,int last){
   int i, j, pivot, temp;
   if(first<last){
      pivot=first;
      i=first;
      j=last;
      while(i<j){
         while(v[i]<=v[pivot]&&i<last)
            i++;
         while(v[j]>v[pivot])
            j--;
         if(i<j){
            temp=v[i];
            v[i]=v[j];
            v[j]=temp;
         }
      }
      temp=v[pivot];
      v[pivot]=v[j];
      v[j]=temp;
      quicksort(v,first,j-1);
      quicksort(v,j+1,last);
   }
}

int main (int argc, char *argv[]) {
    int v[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++){
        v[i] = rand()%MAX;
    }
    //clock_t begin = clock();
    bucket_sort(v,ARRAY_SIZE);
    //clock_t end = clock();
    for (int i = 0; i < ARRAY_SIZE; i++){
        printf("%d\n",v[i]);
    }
    //printf("Time: %f", (end-begin)/CLOCKS_PER_SEC);
    return 0;
}