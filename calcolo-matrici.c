#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static volatile int A_rows = 4;
static volatile int A_cols = 4;
static volatile int B_rows = 4;
static volatile int B_cols = 4;
static volatile int C_rows = 4;
static volatile int C_cols = 4;


void fill_matrix(int **matrix, int rows, int cols) {

    int i, j;
    float a = 5.0;

    matrix = malloc(rows * sizeof *matrix);
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

void print_matrix(int **matrix, int rows, int cols) {
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
    if(argc != 4) {
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

    int **A;
    int **B;
    int **C;
    
    
    fill_matrix(A, A_rows, A_cols);
    fill_matrix(B, B_rows, B_cols);
    fill_matrix(C, C_rows, C_cols);
    
    print_matrix(A, A_rows, A_cols);
    print_matrix(B, B_rows, B_cols);
    print_matrix(C, C_rows, C_cols);

    return 0;
}