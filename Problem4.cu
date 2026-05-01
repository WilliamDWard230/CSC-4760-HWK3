#include <iostream>
#include <cuda_runtime.h>

using namespace std;

const int rows = 3;
const int cols = 3;

int A[rows*cols] = {
    130, 147, 115,
    224, 158, 187,
    54, 158, 120
};

int B[cols] = {
    221, 12, 157
};

int soln[rows*cols] = {
    351, 159, 272,
    445, 170, 344,
    275, 170, 277
};


const int S1 = rows * cols * 4;
const int S2 = cols * 4;

__global__ void vectorAdd(int* A, int* B, int rows, int cols){
    int index = blockIdx.x * blockDim.x + threadIdx.x;

    //if more to go
    if( index < rows * cols){
        
        int column = index%cols;
        A[index] = A[index] + B[column];
    }
}

int main(void){

    int* one;
    int* two;

    cudaMalloc((void**)&one, S1);
    cudaMalloc((void**)&two, S2);

    cudaMemcpy(one, A, S1, cudaMemcpyHostToDevice);
    cudaMemcpy(two, B, S2, cudaMemcpyHostToDevice);

    int threadCount = 32;
    int blocksCount = (rows * cols + threadCount -1) / threadCount;

    vectorAdd<<<blocksCount, threadCount>>>(one, two, rows, cols);

    cudaDeviceSynchronize();

    cudaMemcpy(A, one, S1, cudaMemcpyDeviceToHost);

    cout << "Updated: " << endl;

    for (int i = 0; i < rows; i++){

        for (int j = 0; j < cols; j++) {

            cout << A[i * cols + j] << " ";

        }

        cout << endl;
    }

    bool correct = true;

    for (int i = 0; i < rows * cols; i++){
        if (A[i] != soln[i]) {

            correct = false;
            break;
        }

    }

    if (correct){
        cout << "Matrix iscorrect." << endl;
    } 
    else{
        cout << "Matrix is wrong." << endl;
    }

    cudaFree(one);
    cudaFree(two);

    return 0;

}