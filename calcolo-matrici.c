#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static volatile int A_rows = 4;
static volatile int A_cols = 4;
static volatile int B_rows = 4;
static volatile int B_cols = 4;
static volatile int C_rows = 4;
static volatile int C_cols = 4;


void fill_matrix(float **matrix, int rows, int cols) {

    int i, j;
    float a = 5.0;

   
    for (i=0; i<rows; i++){
        matrix[i] = malloc(cols * sizeof *matrix[i]);
    }

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            matrix[i][j] = ((float)rand()/(float)(RAND_MAX)) * a;
        }
    }

}

void print_matrix(float **matrix, int rows, int cols) {
    int i = 0;
    int j = 0;

    for(i = 0 ; i < rows ; i++) {
        printf(" (");
        for(j = 0 ; j < cols ; j++){
            printf("%f     ", matrix[i][j]);
        }
        printf(")\n");
    }
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
    
    srand((unsigned int)time(NULL));
    
    int i;

    float **A = (float **)malloc(A_rows * A_cols * sizeof(float));
    for (i=0; i < A_rows; i++)
         A[i] = (float *)malloc(A_cols * sizeof(float));

    float **B = (float **)malloc(B_rows * B_cols * sizeof(float));
    for (i=0; i < B_rows; i++)
         B[i] = (float *)malloc(B_cols * sizeof(float));


    float **C = (float **)malloc(C_rows * C_cols * sizeof(float));
    for (i=0; i < C_rows; i++)
         C[i] = (float *)malloc(C_cols * sizeof(float));
    

    fill_matrix(A, A_rows, A_cols);
    fill_matrix(B, B_rows, B_cols);
    fill_matrix(C, C_rows, C_cols);
    
    printf("A:\n");
    print_matrix(A, A_rows, A_cols);

    printf("B:\n");
    print_matrix(B, B_rows, B_cols);

    printf("C:\n");
    print_matrix(C, C_rows, C_cols);

    return 0;
}