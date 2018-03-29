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

void init() {
    LOG_INFO << "Exact Method - START";
    ExactMethod ExactMethod(100, 300, 300, 1, "png");
    LOG_INFO << "======= Method Init END =======";
//    ExactMethod.recursiveTest();
    ExactMethod.recursiveRun();
//    ExactMethod.recursiveWithComparsionTest(0.7);
//    ExactMethod.recursiveWithComparsionRun(0.1);
    LOG_INFO << "Exact Method - END";

    LOG_INFO << "MonteCarlo Method - START";
//    MonteCarloMethod MonteCarloMethod(100, 300, 300, 1, "png");
    LOG_INFO << "======= Method Init END =======";
//    MonteCarloMethod.reliabilityExpectedTest(3);
//    MonteCarloMethod.reliabilityExpectedRun(5);
//    MonteCarloMethod.reliabilityTest(3, 0.7);
//    MonteCarloMethod.reliabilityRun(3, 0.7);
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
    init();
    LOG_INFO << "Program - END";

    LOG_INFO << "MPI - START";
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
    LOG_INFO << "Hello world from processor " << processor_name
             << ", rank " << world_rank << " out of " << world_size << " processors";

    // Finalize the MPI environment.
    MPI_Finalize();
    LOG_INFO << "MPI - END";

    return EXIT_SUCCESS;
}