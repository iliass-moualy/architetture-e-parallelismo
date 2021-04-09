#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


static volatile int NUM_TREADS = 4;

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
    int whichRow;
    int howManyRows;
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
void tRowColMultiplication(float * LeftMatrix, float ** RightMatrix, int howManyCols, int whichRow)
{
    int dim = 3;

    float buffer = 0;

      for (int i = 0; i < howManyCols; i++)
      {
        for (int j = 0; j < dim; j++)
        {
            // printf("%f * %f =", LeftMatrix[j], RightMatrix[j][i]);
            buffer = buffer +  LeftMatrix[j] * RightMatrix[j][i] ;
            // printf("%f\t", buffer);
        }
        //printf("row:%d col:%d", whichRow, i);
        result[whichRow][i] = buffer;
        buffer = 0;
        // printf("\n");
      }
}




void *RowColMultiplication(void *input)
{
    //  printf("entering..");

    int Row_Dim = ((struct args*)input)->LeftCols; //dimensione della riga => LeftCols
    int cols = ((struct args*)input)->RightCols;
    // int row = ((struct args*)input)->whichRow;
    // int blockLength = ((struct args*)input)->howManyRows; //numero di righe per blocco

    int from = ((struct args*)input)->from;
    int to = ((struct args*)input)->to;

    // printf("from: %d to:%d\n", from, to);
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


    pthread_t my_threads[2];
    void * returnCode;

    int Tresult;
    float **A, **B, **C;

    A = create_matrix( A_rows, A_cols, 1 );
    B = create_matrix( B_rows, B_cols, 5 );
    C = create_matrix( C_rows, C_cols, 9 );

    result = create_matrix( A_rows, B_cols, 1 );


    // prnt_matrix(result, A_rows, B_cols);
    prnt_matrix(A, A_rows, A_cols);
    printf("\n");
    prnt_matrix(B, B_rows, B_cols);
    printf("\n");

    bool isMultiple = true;
    int howManyBlocks = 2; //voglio 2 blocchi
    int offset = 0;

    if((offset = A_rows % howManyBlocks) != 0)
        isMultiple = false;
    
    int BlockLength = A_rows / howManyBlocks; //dimensione dei blocchi
    int thread_no = 0;
    int from = 0, to = 0;
    //in questo caso si lavora con 4 threads


    /* Start the threads */
    for (int i = 0; i < A_rows; i++)
    {

      if(!isMultiple && i + 1 == offset ){
        /* Dichiaro struct di elementi */
        struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
        *Matrixes = Initialize_Args(A,B, A_cols, B_cols, i+1 - offset, i+1);
        // Matrixes->LeftMatrix  = A;
        // Matrixes->RightMatrix = B;
        // Matrixes->RightCols = B_cols;
        // Matrixes->LeftCols = A_cols;


        // Matrixes->to   = i+1;
        // Matrixes->from = i+1 - offset; 
        // printf("%d\t", Matrixes->whichRow);
        Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
        isMultiple = true;
      }
      else if((i+1)%BlockLength == 0 && isMultiple){
        /* Dichiaro struct di elementi */
        struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
        *Matrixes = Initialize_Args(A,B, A_cols, B_cols, i+1 - BlockLength, i+1);
        // Matrixes->LeftMatrix  = A;
        // Matrixes->RightMatrix = B;
        // Matrixes->RightCols = B_cols;
        // Matrixes->LeftCols = A_cols;

        // Matrixes->to   = i+1;
        // Matrixes->from = i+1 - BlockLength; 
        // printf("to: %d  from: %d\t", Matrixes->from, Matrixes->to );
        Tresult = pthread_create(&my_threads[thread_no++], NULL, &RowColMultiplication, (void*)Matrixes);
      }


    }


    // pthread_barrier_wait (&barrier);

    /* Wait for the threads to end */
    for (int i = 0; i < thread_no; i++)
    {
       Tresult = pthread_join(my_threads[i], &returnCode);
    }

    //Da notare che la dimensione della matrice risultate è nota!
    printf("All %d threads terminated\n", thread_no);
    prnt_matrix(result, A_rows, B_cols);
}
