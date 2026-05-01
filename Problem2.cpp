#include <mpi.h>
#include <iostream>
#include <vector>


using namespace std;

int main(int argc, char** argv){
    
    int rank, size;
    // init everything
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int P;
    int Q;
    int M;

    //check for user args
    if(argc > 3){
        P = std::atoi(argv[1]);
        Q = std::atoi(argv[2]);
        M = std::atoi(argv[3]);

    }
    else{
        P = 2;
        Q = 4;
        M = 15;
    }

    if ( P < 1 || Q < 1 || size != P * Q){
        
        if(rank == 0){
            cout << "Error with P and/or Q";
        }

        MPI_Finalize();
        return 0;
    }

    //new material

    int row = rank / Q;
    int col = rank % Q;


    //create comms
    MPI_Comm row_COM;
    MPI_Comm_split(MPI_COMM_WORLD, row, col, &row_COM);
    MPI_Comm col_COM;
    MPI_Comm_split(MPI_COMM_WORLD, col, row, &col_COM);
    

    vector<int> X_train(P);
    vector<int> X_val(P);
    int X_base = M/P;
    int X_mod = M%P;

    for(int i = 0; i < P; i++){
        X_train[i] = X_base;

        if ( i < X_mod){
            X_train[i] = X_train[i]+1;
        }
    }

    X_val[0] = 0;
    for(int i=1; i<P; i++){
        X_val[i] = X_val[i-1] + X_train[i-1];
    }

    vector<int> y_train(Q);
    vector<int> y_val(Q);
    int y_base = M/Q;
    int y_mod = M%Q;

    for(int i = 0; i < Q; i++){
        y_train[i] = y_base;

        if ( i < y_mod){
            y_train[i] = y_train[i]+1;
        }
    }

    y_val[0] = 0;
    for(int i=1; i<Q; i++){
        y_val[i] = y_val[i-1] + y_train[i-1];
    }

    int localX = X_train[row];

    vector<int> localX_V(localX);
    vector<int> TotalX(M);
    vector<int> globalX;

    int* buffer = nullptr;

    //fill in vector
    if(rank==0){
        globalX .resize(M);

        for(int i=0; i<M; i++){
            globalX[i] = i;
        }

        buffer = globalX.data();
    }


    if(col == 0){
        MPI_Scatterv(buffer, X_train.data(),X_val.data(),MPI_INT,localX_V.data(),localX,MPI_INT,0,col_COM);
    }
    MPI_Bcast(localX_V.data(), localX, MPI_INT,0,row_COM);
    MPI_Allgatherv(localX_V.data(), localX, MPI_INT, TotalX.data(), X_train.data(),X_val.data(),MPI_INT,col_COM);


    int localy = y_train[col];
    vector<int> localy_V(localy);

    for ( int i = 0; i < localy; i++){

        int globaly = y_val[col] + i;
        localy_V[i] = TotalX[globaly];
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for(int r = 0 ; r < size; r++){

        if ( rank == r){

            cout << "Rank: " << rank;
            cout << " | Row: " << row;
            cout << " | col: " << col;
            cout << " X: ";

            for(int i=0; i<localX; i++){
                cout << localX_V[i] << " ";
        
            }

            cout << " | y: ";

            for(int j=0; j<localy; j++){

                cout<< localy_V[j] << " ";
            }

            cout << endl;
        }
        
        MPI_Barrier(MPI_COMM_WORLD);

    }

    MPI_Comm_free(&row_COM);
    MPI_Comm_free(&col_COM);

    MPI_Finalize();

    return 0;

}