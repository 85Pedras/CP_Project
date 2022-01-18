#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "papi.h"
#include "bucket_sort_omp.h"

#define NUM_EVENTS 4
int Events[NUM_EVENTS] = {PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_DCM};

// PAPI counters' values
long long values[NUM_EVENTS], min_values[NUM_EVENTS];

// number of times the function is executed and measured
#define NUM_RUNS 5

int MAX = N_BUCKETS*10;

typedef struct {
    int n;  // número de elementos de cada bucket - 1
    int *bucket;
} bucket;
                                                 
void bucket_sort (int v[],int size){      
    omp_set_num_threads(THREADS);                              
    bucket *bs = malloc(N_BUCKETS*sizeof(bucket)); //array de buckets                                  
    int i,j,k;

    for(i=0;i<N_BUCKETS;i++) bs[i].n=0; //inicializa todos os "n"

    for(i=0;i<N_BUCKETS;i++) bs[i].bucket = malloc(BUCKET_SIZE*sizeof(int));
    

    //#pragma omp parallel for shared(v) schedule(static,ARRAY_SIZE/THREADS)
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

    #pragma omp parallel for schedule(static)
    for(i=0;i<N_BUCKETS;i++){  //ordena os buckets
        if(bs[i].n) quicksort(bs[i].bucket,0,(bs[i].n)-1);
    }

    //Imprimir cada bucket
    /*for(i=0;i<N_BUCKETS;i++){
        printf("Bucket %d:\n",i);
        for(j=0;j<bs[i].n;j++){
            printf("%d\n",bs[i].bucket[j]);
        }
    }
    printf("--------\n");*/
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

    if(argc > 1 && strcmp(argv[1],"papi") == 0){
        long long start_usec, end_usec, elapsed_usec, min_usec=0L;
        int i, run;
        int num_hwcntrs = 0;

        fprintf (stdout,"\nSetting up PAPI...");
        PAPI_library_init (PAPI_VER_CURRENT); // Initialize PAPI
        
        if ((num_hwcntrs = PAPI_num_counters()) <= PAPI_OK)  { // Get the number of hardware counters available
            fprintf (stderr,"PAPI error getting number of available hardware counters!\n");
            return 0;
        }
        fprintf(stdout,"done!\nThis system has %d available counters.\n\n", num_hwcntrs);

        if (num_hwcntrs >= NUM_EVENTS) { // We will be using at most NUM_EVENTS counters
            num_hwcntrs = NUM_EVENTS;
        } else {
            fprintf (stderr,"Error: there aren't enough counters to monitor %d events!\n", NUM_EVENTS);
            return 0;
        }

        int *v;
        v = malloc(ARRAY_SIZE*sizeof(int));
        for (int i = 0; i < ARRAY_SIZE; i++) v[i] = rand()%MAX; // Initializing array
        fprintf(stderr,"Array initialized");

        // warmup caches
        fprintf (stderr,"Warming up caches...");
        bucket_sort(v,ARRAY_SIZE); // BucketSort
        fprintf (stderr,"done!\n");

        for (run=0 ; run < NUM_RUNS ; run++) {
            start_usec = PAPI_get_real_usec();
            if (PAPI_start_counters(Events, num_hwcntrs) != PAPI_OK) { // Start counting events
                fprintf (stderr,"PAPI error starting counters!\n");
                return 0;
            }
            
            bucket_sort(v,ARRAY_SIZE); // BucketSort
           
            if (PAPI_stop_counters(values, NUM_EVENTS) != PAPI_OK) {  // Stop counting events
                fprintf (stderr, "PAPI error stoping counters!\n");
                return 0;
            }
            end_usec = PAPI_get_real_usec();
            fprintf (stderr,"\ndone!\n");

            elapsed_usec = end_usec - start_usec;

            if ((run==0) || (elapsed_usec < min_usec)) {
                min_usec = elapsed_usec;
                for (i=0 ; i< NUM_EVENTS ; i++) min_values[i] = values [i];
            }
        }
        printf ("\nWall clock time: %lld usecs\n", min_usec);
        
        for (i=0 ; i< NUM_EVENTS ; i++) { // output PAPI counters' values
            char EventCodeStr[PAPI_MAX_STR_LEN];
            if (PAPI_event_code_to_name(Events[i], EventCodeStr) == PAPI_OK) {
                fprintf (stdout,"%s = %lld\n", EventCodeStr, min_values[i]);
            } else {
                fprintf (stdout,"PAPI UNKNOWN EVENT = %lld\n", min_values[i]);
            }
        }

        #if NUM_EVENTS > 1
        // evaluate CPI and Texec here
        if ((Events[0] == PAPI_TOT_CYC) && (Events[1] == PAPI_TOT_INS)) {
            float CPI = ((float) min_values[0]) / ((float) min_values[1]);
            fprintf (stdout,"CPI = %.2f\n", CPI);
        }
        #endif
        printf ("\nThat's all, folks\n");
    }

    else{
        int *v;
        v = malloc(ARRAY_SIZE*sizeof(int));
        for (int i = 0; i < ARRAY_SIZE; i++){
            v[i] = rand()%MAX;
        }
        double begin = omp_get_wtime();
        bucket_sort(v,ARRAY_SIZE);
        double end = omp_get_wtime();
        /*for (int i = 0; i < ARRAY_SIZE; i++){
            printf("%d\n",v[i]);
        }*/
        printf("Time: %fs", (end-begin));
    }
    return 0;
}