//
// Created by Александр Кучеров on 06/03/2018.
//

// Methods
#include "Controllers/Methods/ExactMethod.cpp"
#include "Controllers/Methods/MonteCarloMethod.cpp"
#include "Controllers/Methods/Method.h"

#include <mpi.h>
#include "boost/filesystem.hpp"

using namespace boost;

void init_exact(int methodId) {
    LOG_INFO << "Exact Method - START";
    ExactMethod ExactMethod(100, 300, 300, 1, "png");
    LOG_INFO << "======= Method Init END =======";

    switch (methodId) {
        case 1:
            ExactMethod.recursiveTest();
            break;
        case 2:
            ExactMethod.recursiveRun();
            break;
        case 3:
            ExactMethod.recursiveWithComparsionTest(0.7);
            break;
        case 4:
            ExactMethod.recursiveWithComparsionRun(0.8);
            break;
        default:
            break;
    }

    LOG_INFO << "Exact Method - END";
}

void init_montecarlo(int methodId){
    LOG_INFO << "MonteCarlo Method - START";
    MonteCarloMethod MonteCarloMethod(100, 300, 300, 1, "png");
    LOG_INFO << "======= Method Init END =======";

    switch (methodId) {
        case 1:
            MonteCarloMethod.reliabilityTest(3, 0.7);
            break;
        case 2:
            MonteCarloMethod.reliabilityRun(5, 0.8);
            break;
        case 3:
            MonteCarloMethod.reliabilityExpectedTest(3);
            break;
        case 4:
            MonteCarloMethod.reliabilityExpectedRun(100);
            break;
        default:
            break;
    }

    LOG_INFO << "MonteCarlo Method - END";
}

void init_montecarlo_parallel(int methodId){
    LOG_INFO << "MonteCarlo Method - START";
    MonteCarloMethod MonteCarloMethod(100, 300, 300, 1, "png");
    LOG_INFO << "======= Method Init END =======";

    switch (methodId) {
        case 1:
            MonteCarloMethod.reliabilityTest(3, 0.7);
            break;
        case 2:
            MonteCarloMethod.reliabilityRun(100, 0.9);
            break;
        case 3:
            MonteCarloMethod.reliabilityExpectedTest(3);
            break;
        case 4:
            MonteCarloMethod.reliabilityExpectedRun(10);
            break;
        default:
            break;
    }

    LOG_INFO << "MonteCarlo Method - END";
}

int main(int argc, char **argv)
{
    // Prepare output directory
    string _filePath = "output";
    const char* path = _filePath.c_str();
    boost::filesystem::path dir(path);
    if (boost::filesystem::remove_all(dir)){
        LOG_INFO << "Directory Cleared: " << _filePath;
    }
    if(boost::filesystem::create_directory(dir))
    {
        LOG_INFO << "Directory Created: " << _filePath;
    }

    LOG_INFO << "Program - START";
    //init_exact(2);
    LOG_INFO << "************";
    //init_montecarlo(4);
    //init_montecarlo_parallel(0);
    LOG_INFO << "Program - END";

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d"
           " out of %d processors\n",
           processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();

    return EXIT_SUCCESS;
}