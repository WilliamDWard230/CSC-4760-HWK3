//sequential cpp not mpi


#include <iostream>


int main(int argc, char** argv){

    int M, P, p, i;

    if(argc > 4){
        M = std::atoi(argv[1]);
        P = std::atoi(argv[2]);
        p = std::atoi(argv[3]);
        i = std::atoi(argv[4]);
    }
    else{
        
        M = 8;
        P = 4;
        p = 0;
        i = 1;
    }


    int base = M/P;
    int mod = M%P;

    int local, start;

    if(p < mod) {
        local = base +1;
        start = p * local;
    }
    else{
        local = base;
        start = mod * (base+1) + (p-mod) * base;
    }


    int index = start + i;

    int rank = index%P;
    int local_index = index/P;

    std::cout << "Global Index = " << index << std::endl;
    std::cout << "Scatter Rank = " << rank << std:: endl;
    std::cout << "Local Index = " << local_index << std::endl;

    return 0;


}