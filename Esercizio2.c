#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



struct args {
    float* LeftMatrix;
    float** RightMatrix;
    int howManyCols, whichRow;
};


//Date 3 matrici A[m,n], B[o,p], C[x,y]
const int r1 = 3;
const int c1 = 3;
const int c2 = 6;
const int c3 = 1;

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
    // printf("entering..");
    int dim = 3;
    int row = ((struct args*)input)->whichRow;
    int cols = ((struct args*)input)->howManyCols;
    // printf("thread_no: %d\n", row);

    float buffer = 0;

      for (int i = 0; i < cols; i++)
      {
        for (int j = 0; j < dim; j++)
        {
            //  printf("%f * %f =", ((struct args*)input)->LeftMatrix[row][j], ((struct args*)input)->RightMatrix[j][i]);
            
            buffer = buffer + ((struct args*)input)->LeftMatrix[j] * ((struct args*)input)->RightMatrix[j][i] ;
            //  printf("%f\t", buffer);
        }
        //printf("row:%d col:%d", whichRow, i);
        result[row][i] = buffer;
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

int main()
{


    
    pthread_t my_threads[3];

    void * returnCode;

    int Tresult;
    float ** matrix,r0,c0;

    r0 = 3;
    c0 = 3;
    matrix          = create_matrix( r0, c0, 1 );

    int r1 = 3;
    int c1 = 8;
    float ** m1     = create_matrix(r1, c1, 5);
    result = create_matrix( 3, 3, 1 );


   


    
    prnt_matrix(matrix, r0, c0);
    printf("\n");
    prnt_matrix(m1, r1, c1);
    printf("\n");

    /* Start the threads */
    for (int i = 0; i < 3; i++)
    {
      /* Dichiaro struct di elementi */
        struct args *Matrixes = (struct args*)malloc(sizeof(struct args));
        Matrixes->LeftMatrix  = matrix[i];
        Matrixes->RightMatrix = m1; 
        Matrixes->howManyCols = 3;
        Matrixes->whichRow = i;
        // printf("%d\t", Matrixes->whichRow);

        Tresult = pthread_create(&my_threads[i], NULL, &RowColMultiplication, (void*)Matrixes);   
    }


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

     prnt_matrix(result, 3, 3);
}