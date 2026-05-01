#include <mpi.h>
#include <iostream>


using namespace std;

int main(int argc, char** argv){
    
    int rank, size;
    // init everything
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int P;
    int Q;

    //check for user args
    if(argc > 2){
        P = std::atoi(argv[1]);
        Q = std::atoi(argv[2]);

    }
    else{
        P = 2;
        Q = 4;
    }

    //check dims
    if ( P < 1 || Q < 1 || size != P * Q){
        if(rank == 0){
            cout << "Error with P and/or Q";
        }

        MPI_Finalize();
        return 0;
    }

    //linear part
    int linearC = rank/Q;

    MPI_Comm linear_COM;
    MPI_Comm_split(MPI_COMM_WORLD, linearC, rank, &linear_COM);

    int rankL;
    int sizeL;
    int sum = 0;
    int valueL = rank;

    MPI_Comm_rank(linear_COM, &rankL);
    MPI_Comm_size(linear_COM, &sizeL);


    MPI_Reduce(&valueL, &sum, 1, MPI_INT, MPI_SUM, 0 , linear_COM);


    if(rankL == 0){
        cout << "Linear Split  ";
        cout << "Color = " << linearC;
        cout << ", root rank = " << rank;
        cout << ", sum of ranks = " << sum << endl;
    }



    //scatter part
    int scatterC = rank%Q;

    MPI_Comm scatter_COM;
    MPI_Comm_split(MPI_COMM_WORLD, scatterC, rank, &scatter_COM);

    int rankS;
    int sizeS;
    int valueBcast;


    MPI_Comm_rank(scatter_COM, &rankS);
    MPI_Comm_size(scatter_COM, &sizeS);

    if (rankS == 0){
        valueBcast = rank;
    }

    MPI_Bcast(&valueBcast, 1, MPI_INT, 0 , scatter_COM);



    cout << "Scatter Split  ";
    cout << "Color = " << scatterC;
    cout << ", scatter rank = " << rankS;
    cout << ", bcast value = " << valueBcast << endl;

    //free comms
    MPI_Comm_free(&linear_COM);
    MPI_Comm_free(&scatter_COM);

    //finalize
    MPI_Finalize();

    return 0;

}