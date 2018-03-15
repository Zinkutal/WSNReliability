//
// Created by Александр Кучеров on 06/03/2018.
//

#include "Controllers/Methods/AccurateMethod.cpp"
#include <mpi.h>
#include "boost/filesystem.hpp"

using namespace boost;

void init() {
    LOG_INFO << "Accurate Method - START";
    AccurateMethod accurateMethod(100);
    accurateMethod.init();
    LOG_INFO << "Accurate Method - END";
}

int main(int argc, char **argv)
{
    string _filePath = "output";
    const char* path = _filePath.c_str();
    boost::filesystem::path dir(path);
    if(boost::filesystem::create_directory(dir))
    {
        std::cerr<< "Directory Created: "<<_filePath<<std::endl;
    }

    LOG_INFO << "Program - START";
    init();
    LOG_INFO << "Program - END";

    /*LOG_INFO << "MPI - START";
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
    LOG_INFO << "MPI - END";*/

    return EXIT_SUCCESS;
}