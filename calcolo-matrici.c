#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


static volatile int NUM_THREADS = 4;


static volatile int A_rows = 3;
static volatile int A_cols = 3;
static volatile int B_rows = 3;
static volatile int B_cols = 3;
static volatile int C_rows = 4;
static volatile int C_cols = 4;




typedef struct args {
    float** LeftMatrix;
    float** RightMatrix;
    int LeftCols, RightCols;
    int from, to;
} Args;



Args Initialize_Args(float** LeftMatrix, float** RightMatrix,
         int leftCols, int rightCols, int from, int to){
  
   return (Args) {.LeftMatrix = LeftMatrix, .RightMatrix = RightMatrix, 
   .LeftCols = leftCols, .RightCols = rightCols, .from = from, .to = to};
}



// pthread_barrier_t   barrier;
//La dimensione della matrice risultante sarà uguale a: m ed y
float **result;

//Precondition:
/*
    TestCases:
        LeftMatrix[3][3] && RightMatrix[3][1] --> testando...
        LeftMatrix[3][3] && RightMatrix[3][8]
        LeftMatrix[1][3] && RightMatrix[3][8]
*/
//Questi vettori, per vincolo moltiplicazione, hanno la stessa lunghezza!

void *RowColMultiplication(void *input)
{
    //  printf("entering..");

    int Row_Dim = ((struct args*)input)->LeftCols; //dimensione della riga => LeftCols
    int cols = ((struct args*)input)->RightCols;
    // int row = ((struct args*)input)->whichRow;
    // int blockLength = ((struct args*)input)->howManyRows; //numero di righe per blocco

    int from = ((struct args*)input)->from;
    int to = ((struct args*)input)->to;

    printf("from: %d to:%d\n", from, to);
    float buffer = 0;


    for (int m = from; m < to; m++)
    {
      //per ogni colonna della RightMatrix
      for (int i = 0; i < cols; i++)
      {
        //per ogni elem della riga LeftMatrix, la moltiplico con la colonna i-esima di RightMatrix
        for (int j = 0; j < Row_Dim; j++)
        {
            // printf("%f * %f =", ((struct args*)input)->LeftMatrix[row][j], ((struct args*)input)->RightMatrix[j][i]);

            buffer = buffer + ((struct args*)input)->LeftMatrix[m][j] * ((struct args*)input)->RightMatrix[j][i] ;
            //  printf("%f\t", buffer);
        }

        result[m][i] = buffer;
        // printf("row:%d col:%d value: %f" , row, i, result[row][i]);
        buffer = 0;
        //  printf("\n");
      }
    }

}


void prnt_matrix(float ** matrix,int m,int n)
{

    for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            printf("%f ",matrix[i][j]);
        }
        printf("\n");
    }
}

float ** create_matrix(int m, int n, float starter)
{
  float ** ma = calloc(m, sizeof( float*));

  if (ma != NULL) {
    int i, j;

    for (i = 0; i != m; ++i) {
      if ((ma[i] = malloc(n*sizeof(float))) == NULL)
        return NULL;
      for (j = 0; j != n; ++j) {
        ma[i][j] = i*n + j + starter;
      }
    }

  }
  return ma;
}


void obtn_matrix(float *** matrix, float * m, float * n)
{
    printf("Please enter the row number: ");
    fflush(stdin);
    scanf("%f",m);

    printf("Please enter the column number: ");
    fflush(stdin);
    scanf("%f",n);

    printf("Please enter starter number: ");
    fflush(stdin);

    *matrix=create_matrix(*m,*n, 0);
}

void free_matrix(float ** matrix, int m, int n)
{
    for(int i=0;i<m;i++)
    {
      if (matrix[i] == NULL)
        /* matrix creation was aborted */
        break;
      free(matrix[i]);
    }

    free(matrix);
}

int main(int argc, char *argv[])
{
  int i = 0;

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

/***************************
   **  DECOMPOSIZIONE DI A  ** 
   ***************************/
  // se righe di A sono multiplo del numero massimo di blocchi allora block size sarà:
  //    A_rows/MAX_BLOCKS
  // altrimenti:
  //    prendiamo A' che è la più grande sottomatrice di A t.c. abbia numero di righe multiplo di MAX_BLOCKS
  //    determiniamo block_size tramite = A'_rows/MAX_BLOCKS
  //    a questo punto le k righe lasciate fuori saranno di numero al massimo: MAX_BLOCK - 1
  //    queste k righe rimanenti le assegniamo ai primi k thread
  //
  //in questo modo decomponiamo uniformemente nel caso la matrice avesse una size multipla del numero di blocchi massimo supportato
  //ed avremmo una decomposizione quasi uniforme nel caso di size non multipla
  

  int blocks_per_thread = 1;

  const int MAX_BLOCKS = blocks_per_thread * NUM_THREADS;
  int blocks_number = 0;
  int blocks_size = 0;
  int rows_left = 0;
  int is_multiple = true;
  
  printf("Decomponing matrix A...\n");

  if(A_rows <= MAX_BLOCKS){
    blocks_number = A_rows;
    blocks_size = 1;
  }
  else {
    if(A_rows % MAX_BLOCKS == 0){
      blocks_size = A_rows / MAX_BLOCKS;
      blocks_number = MAX_BLOCKS; 
    }
    else{
      rows_left = A_rows % MAX_BLOCKS;
      blocks_size = (A_rows - rows_left) / MAX_BLOCKS;
      is_multiple = false;
      blocks_number = MAX_BLOCKS; 

    }
  }

  // assegnazione ai thread 

  int k = 1;
  int prev = 1;

  pthread_t my_threads[NUM_THREADS];
  float **A, **B, **C;

  A = create_matrix( A_rows, A_cols, 1 );
  B = create_matrix( B_rows, B_cols, 1 );
  C = create_matrix( C_rows, C_cols, 9 );

  
  // prnt_matrix(result, A_rows, B_cols);
  prnt_matrix(A, A_rows, A_cols);
  printf("\n");
  prnt_matrix(B, B_rows, B_cols);
  printf("\n");


  result = create_matrix( A_rows, B_cols, 1 );

  printf("Assigning threads...\n");

  for(i = 0; i < blocks_number; ++i){
    if(i == 0)
        prev = 0;
    else 
        prev = 1;

   if(rows_left == 0)
    k = 0;

    struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
    *Matrixes = Initialize_Args(A,B, A_cols, B_cols, i * blocks_size + prev, (i+1) * blocks_size - 1 + k);
    pthread_create(&my_threads[i++], NULL, &RowColMultiplication, (void*)Matrixes);
    --rows_left;
  }

  printf("Result matrix:\n");

  prnt_matrix(result, A_rows, B_cols);

}
