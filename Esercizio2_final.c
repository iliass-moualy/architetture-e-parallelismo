#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

pthread_barrier_t   barrier; // the barrier synchronization object
// static volatile int NUM_TREADS = 4;

static volatile int MAX_THREADS = 8;


static volatile int A_rows = 250;
static volatile int A_cols = 250;
static volatile int B_rows = 250;
static volatile int B_cols = 250;
static volatile int C_rows = 250;
static volatile int C_cols = 250;

typedef struct args {
    float** LeftMatrix;
    float** RightMatrix;
    float** Result;
    int LeftCols, RightCols;
    int from, to; //block_dim
} Args;


Args Initialize_Args(float** LeftMatrix, float** RightMatrix, float ** result,
         int leftCols, int rightCols, int from, int to){
  
   return (Args) {.LeftMatrix = LeftMatrix, .RightMatrix = RightMatrix, 
   .LeftCols = leftCols, .RightCols = rightCols, .from = from, .to = to, .Result = result};
}



pthread_barrier_t   barrier;
//La dimensione della matrice risultante sarà uguale a: m ed y

void *RowColMultiplication(void *input)
{
    

    int Row_Dim = ((struct args*)input)->LeftCols; //dimensione della riga => LeftCols
    int cols = ((struct args*)input)->RightCols;
    float** result = ((struct args*)input)->Result;

    int from = ((struct args*)input)->from;
    int to = ((struct args*)input)->to;

    float buffer = 0;


    for (int m = from; m < to; m++)
    {
      //per ogni colonna della RightMatrix
      for (int i = 0; i < cols; i++)
      {
        //per ogni elem della riga LeftMatrix, la moltiplico con la colonna i-esima di RightMatrix
        for (int j = 0; j < Row_Dim; j++)
        {
          buffer = buffer + ((struct args*)input)->LeftMatrix[m][j] * ((struct args*)input)->RightMatrix[j][i] ;
        }
        result[m][i] = buffer;
        buffer = 0;
      }
    }
    pthread_barrier_wait (&barrier);
}


void prnt_matrix(float ** matrix,int m,int n)
{
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            printf(" %f ",matrix[i][j]);
        }
        printf("\n");
    }
}

float ** create_matrix(int m, int n)
{
  float ** ma = calloc(m, sizeof( float*));
    
  float a = 5.0;

  if (ma != NULL) {
    int i, j;
    for (i = 0; i != m; ++i) {
      if ((ma[i] = malloc(n*sizeof(float))) == NULL)
        return NULL;
      for (j = 0; j != n; ++j) {
        ma[i][j] = ((float)rand()/(float)(RAND_MAX)) * a;
      }
    }
  }
  return ma;
}

void free_matrix(float ** matrix, int m, int n)
{
    for(int i=0;i<m;i++){
      if (matrix[i] == NULL)
        /* matrix creation was aborted */
        break;
      free(matrix[i]);
    }
    free(matrix);
}

float** triple_matrix_mul(float **A, float **B, float **C, int threads_number){
    time_t  now;
    struct timespec start, finish;
    double elapsed;
    int thread_no = 0;    
    int Tresult;
    float** FinalResult;

    bool isMultiple = true;

    int howManyBlocks = threads_number; //voglio howManyBlocks blocchi --> ad ogni blocco è associato un thread

    pthread_barrier_init (&barrier, NULL, howManyBlocks + 1); //dico alla barriera quanti thread dovrà fermare prima di proseguire
    pthread_t my_threads[howManyBlocks];


    float **result;
    result = create_matrix( A_rows, B_cols);

    int maximumBlocksToAdd = A_rows % howManyBlocks;

    if(maximumBlocksToAdd != 0)
        isMultiple = false;
    
    int BlockLength = A_rows / howManyBlocks; //dimensione dei blocchi (nro)

    if(BlockLength == 0){
      printf("Too many blocks!");
      exit(-10);
    }

    int countBlock = 0; 

    int i = 0;
    int howManyRounds = 100;

for (int p = 0; p<howManyRounds; p++){
    clock_gettime(CLOCK_MONOTONIC, &start);
    /* Start the threads */
    for (i = 0; i < A_rows; i++){
      if(countBlock < maximumBlocksToAdd){
        if((i+1) % (BlockLength + 1)  == 0){
          struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
          
          int from = (i+1) - (BlockLength + 1);
          int to = i+1;
          // printf("from: %d to:%d\n", from, to);

          *Matrixes = Initialize_Args(A,B, result, A_cols, B_cols, from, to);
          Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
          ++countBlock;
        }
      }
      else{

          if((i+1-maximumBlocksToAdd) % BlockLength == 0){
            struct args *Matrixes = (struct args*)malloc(sizeof(struct args));

            int from = i+1 - BlockLength;
            int to = i+1;
            // printf("from: %d to:%d\n", from, to);

            *Matrixes = Initialize_Args(A,B, result, A_cols, B_cols, i+1 - BlockLength, i+1);
            Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
          }      
        // struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
        // *Matrixes = Initialize_Args(A,B, result, A_cols, B_cols,  from , to);
        // Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
      }
    }
     

    // Sincronizzazione tramite barriera
    // printf("Last waiting...\n");
    pthread_barrier_wait (&barrier);
    // printf("Next round...\n");
    FinalResult = create_matrix(C_rows, B_cols);
    countBlock = 0;
    thread_no = 0;


    //Da notare che se C_rows < A_Rows, allora è possibile che ci siano meno blocchi e di conseguenza, non tutti i thread parteciperanno alla seconda moltiplicazione
    for (i = 0; i < C_rows; i++)
    {
      if(countBlock < maximumBlocksToAdd){
        if((i+1) % (BlockLength + 1)  == 0){
          struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
          
          int from = (i+1) - (BlockLength + 1);
          int to = i+1;
          // printf("from: %d to:%d\n", from, to);

          *Matrixes = Initialize_Args(C,result, FinalResult, C_cols, B_cols, from, to);
          Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
          ++countBlock;
        }
      }
      else{

       if((i+1-maximumBlocksToAdd) % BlockLength == 0){
            struct args *Matrixes = (struct args*)malloc(sizeof(struct args));

            int from = i+1 - BlockLength;
            int to = i+1;
            // printf("from: %d to:%d\n", from, to);

            *Matrixes = Initialize_Args(C,result, FinalResult, C_cols, B_cols, from, to);
            Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
          } 
      }
    }

    pthread_barrier_wait (&barrier);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed += (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    countBlock = 0;
    thread_no = 0;
  }    

  printf("\n All %d thread terminated (%d rounds) in: %f\n", threads_number, howManyRounds, elapsed/howManyRounds);
  return FinalResult;
}

bool equals(float** A, float** B, int rows, int cols){
  
  for(int i=0; i<rows; i++)
  {
        for(int j=0; j<cols; j++)
        {
      if(A[i][j] != B[i][j])
      {
        printf("A[%d][%d]: %f\tB[%d][%d]: %f", i,j, A[i][j],i,j,B[i][j]);
        return false;
      }
        }
    }
    
  return true;
}


int main(int argc, char *argv[])
{

    if(argc != 5) {
        printf("Invalid arguments, matrixes will have default sizes:\n");
        printf("\t- A(%dx%d)\n\t- B(%dx%d)\n\t- C(%dx%d)\n", A_rows, A_cols, B_rows, B_cols, C_rows, C_cols);
    }
    else{
        A_rows = atoi(argv[1]);
        A_cols = atoi(argv[2]);
        B_rows = atoi(argv[2]);
        B_cols = atoi(argv[3]);
        C_rows = atoi(argv[3]);
        C_cols = atoi(argv[4]);

        printf("Matrixes sizes:\n");
        printf("\t- A(%dx%d)\n\t- B(%dx%d)\n\t- C(%dx%d)\n", A_rows, A_cols, B_rows, B_cols, C_rows, C_cols);
    }
      

    float **A, **B, **C;
   
    srand((unsigned int)time(NULL));

    A = create_matrix( A_rows, A_cols);
    B = create_matrix( B_rows, B_cols);
    C = create_matrix( C_rows, C_cols);


    float **expected;
    float **current;
    int i = 0;

    // current = triple_matrix_mul(A,B,C, 6);

    for(i= 1; i <= MAX_THREADS; ++i){
      printf("Performing matrix multiplication with %d threads:\n", i);
      current = triple_matrix_mul(A,B,C, i);
      if(i == 1)
        expected = current;
      else{
        if(!equals(expected, current, C_rows, B_cols)){
          printf("Result got is not what was expected!\n");
          exit(-1);
        }
        
      }
      printf("\n\n\n");
      printf("------------------------------------------------------------------------------------------\n");
    }



}



