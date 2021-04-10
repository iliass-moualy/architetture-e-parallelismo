#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


pthread_barrier_t   barrier; // the barrier synchronization object
// static volatile int NUM_TREADS = 4;

static volatile int A_rows = 5;
static volatile int A_cols = 6;
static volatile int B_rows = 6;
static volatile int B_cols = 2;
static volatile int C_rows = 4;
static volatile int C_cols = 5;




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
    float** result = ((struct args*)input)->Result;

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
    pthread_barrier_wait (&barrier);

  //  printf("Waiting...\n");
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
    
    void * returnCode;
    int Tresult;
    float **A, **B, **C;
    int thread_no = 0;     

    A = create_matrix( A_rows, A_cols, 1 );
    B = create_matrix( B_rows, B_cols, 5 );
    C = create_matrix( C_rows, C_cols, 9 );

    float **result;
    result = create_matrix( A_rows, B_cols, 1 );


    // prnt_matrix(result, A_rows, B_cols);
    prnt_matrix(A, A_rows, A_cols);
    printf("\n");
    prnt_matrix(B, B_rows, B_cols);
    printf("\n");

    bool isMultiple = true;
    int howManyBlocks = 5; //voglio howManyBlocks blocchi --> ad ogni blocco è associato un thread

    pthread_barrier_init (&barrier, NULL, howManyBlocks + 1); //dico alla barriera quanti thread dovrà fermare prima di proseguire
    pthread_t my_threads[howManyBlocks];


    if(A_rows % howManyBlocks != 0)
        isMultiple = false;
    
    int BlockLength = A_rows / howManyBlocks; //dimensione dei blocchi (nro)

      if(BlockLength == 0){
        printf("Too many blocks!");
        exit(-10);
      }


    //in questo caso si lavora con 4 threads
    int countBlock = 0;

    if(!isMultiple){
      --howManyBlocks;
      ++BlockLength;
    }  

    int i = 0;
    
    /* Start the threads */
    for (i = 0; i < A_rows && countBlock < howManyBlocks; i++)
    {
      if((i+1) % BlockLength == 0){
        struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
        *Matrixes = Initialize_Args(A,B, result, A_cols, B_cols, i+1 - BlockLength, i+1);
         Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
        ++countBlock;
      }          
    }

    //a questo punto mi manca solo l'ultimo blocco da aggiungere
    if(!isMultiple){
      struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
      *Matrixes = Initialize_Args(A,B, result, A_cols, B_cols,  i , A_rows);
       Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
    } 

    
    prnt_matrix(result, A_rows, B_cols);
    //C * (A * B)
    float ** FinalResult = create_matrix(C_rows, B_cols, 0);

    // Sincronizzazione tramite barriera
    pthread_barrier_wait (&barrier);

        for (i = 0; i < C_rows && countBlock < howManyBlocks; i++)
    {
      if((i+1) % BlockLength == 0){
        struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
        *Matrixes = Initialize_Args(C,result, FinalResult,C_cols, B_cols, i+1 - BlockLength, i+1);
         Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
        ++countBlock;
      }          
    }

    //a questo punto mi manca solo l'ultimo blocco da aggiungere
    if(!isMultiple){
      struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
      *Matrixes = Initialize_Args(C,result, FinalResult, C_cols, B_cols, i , C_rows);
       Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
    } 

    
    // /* Wait for the threads to end */
    // for (int i = 0; i < thread_no; i++)
    // {
    //    Tresult = pthread_join(my_threads[i], &returnCode);
    // }

    //Da notare che la dimensione della matrice risultate è nota!
    
    prnt_matrix(FinalResult, C_rows, B_cols);
    printf("\nAll %d threads terminated\n", thread_no);
    // prnt_matrix(finalResult, C_rows,B_cols);

}



