#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREAD_NUMBER

static volatile int A_rows = 4;
static volatile int A_cols = 4;
static volatile int B_rows = 4;
static volatile int B_cols = 4;
static volatile int C_rows = 4;
static volatile int C_cols = 4;


//axb  bxc
struct args {
    float** LeftMatrix;
    float** RightMatrix;
    int LeftCols, RightCols;
    int whichRow;
};

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
//3x3 --> v(1x3)  3x1
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
    int row = ((struct args*)input)->whichRow;


    // printf("thread_no: %d\n", row);

    float buffer = 0;

      for (int i = 0; i < cols; i++)
      {
        //
        for (int j = 0; j < Row_Dim; j++)
        {
            // printf("%f * %f =", ((struct args*)input)->LeftMatrix[row][j], ((struct args*)input)->RightMatrix[j][i]);

            buffer = buffer + ((struct args*)input)->LeftMatrix[row][j] * ((struct args*)input)->RightMatrix[j][i] ;
            //  printf("%f\t", buffer);
        }

        result[row][i] = buffer;
        // printf("row:%d col:%d value: %f" , row, i, result[row][i]);
        buffer = 0;
        //  printf("\n");
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
  float ** ma = calloc(m, sizeof(float*));

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
        printf("\t- A(4x4)\n\t- B(4x4)\n\t- C(4x4)\n");
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


    pthread_t my_threads[3];
    void * returnCode;

    int Tresult;
    float ** A, **B, **C;

    A = create_matrix( A_rows, A_cols, 1 );
    B = create_matrix( B_rows, B_cols, 5 );
    C = create_matrix( C_rows, C_cols, 9 );

    result = create_matrix( A_rows, B_cols, 1 );


    // prnt_matrix(result, A_rows, B_cols);
    prnt_matrix(A, A_rows, A_cols);
    printf("\n");
    prnt_matrix(B, B_rows, B_cols);
    printf("\n");


    /* Start the threads */
    for (int i = 0; i < A_rows; i++)
    {
      /* Dichiaro struct di elementi */
        struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
        Matrixes->LeftMatrix  = A;
        Matrixes->RightMatrix = B;
        Matrixes->RightCols = B_cols;
        Matrixes->whichRow = i;
        Matrixes->LeftCols = A_cols;

        // printf("%d\t", Matrixes->whichRow);

        Tresult = pthread_create(&my_threads[i], NULL, &RowColMultiplication, (void*)Matrixes);
    }

      // pthread_barrier_wait (&barrier);

    /* Wait for the threads to end */
    for (int i = 0; i < 3; i++)
    {
       Tresult = pthread_join(my_threads[i], &returnCode);
    }

    //Da notare che la dimensione della matrice risultate è nota!
    printf("All threads terminated\n");
    // for (int i = 0; i < 3; i++)
    // {
    //   RowColMultiplication(matrix[i], m1, 3, i);
    // }

     prnt_matrix(result, A_rows, B_cols);
}
