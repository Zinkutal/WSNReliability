//
// Created by Александр Кучеров on 31/03/2018.
//
// Methods
#include "../Controllers/Methods/ExactMethod.cpp"
#include "../Controllers/Methods/MonteCarloMethod.cpp"
#include "../Controllers/Methods/Method.h"

#include <iostream>
#include <string>
#include <vector>
#include "boost/filesystem.hpp"
#include <omp.h>

using namespace boost;

int accuracy, oImgSizeX, oImgSizeY, oImgScale, numThreads;
std::string oImgFormat;

void prepareOutputDir(){
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
}

void prepareInputDir(){
    // Prepare input directory
    string _filePath = "input";
    const char* path = _filePath.c_str();
    boost::filesystem::path dir(path);
    if(boost::filesystem::create_directory(dir))
    {
        LOG_INFO << "Directory Created: " << _filePath;
    }
}

void prepareConfigDir(){
    // Prepare input directory
    string _filePath = "config";
    const char* path = _filePath.c_str();
    boost::filesystem::path dir(path);
    if(boost::filesystem::create_directory(dir))
    {
        LOG_INFO << "Directory Created: " << _filePath;
    }
}

void setThreadsNum(){
    if (numThreads <= omp_get_max_threads()){
        omp_set_num_threads(numThreads);
    } else {
        LOG_INFO << "Provided number of threads is wrong, value is set to max available logical cores: " << omp_get_max_threads();
    }
}

void setConfig(){
    LOG_INFO << "Reading settings - START";

    std::ifstream fin("config/config.txt");
    std::string line;
    std::istringstream sin;

    while (std::getline(fin, line)) {
        sin.str(line.substr(line.find("=")+1));
        if (line.find("accuracy") != std::string::npos) {
            LOG_INFO << "Accuracy - " << sin.str ();
            sin >> accuracy;
        }
        else if (line.find("oImgSizeX") != std::string::npos) {
            LOG_INFO << "Image size X - " << sin.str ();
            sin >> oImgSizeX;
        }
        else if (line.find("oImgSizeY") != std::string::npos) {
            LOG_INFO << "Image size Y - " << sin.str ();
            sin >> oImgSizeY;
        }
        else if (line.find("oImgScale") != std::string::npos) {
            LOG_INFO << "Image Scale - " << sin.str ();
            sin >> oImgScale;
        }
        else if (line.find("oImgFormat") != std::string::npos) {
            LOG_INFO << "Image format - " << sin.str ();
            sin >> oImgFormat;
        }
        else if (line.find("numThreads") != std::string::npos) {
            LOG_INFO << "Number of threads - " << sin.str ();
            sin >> numThreads;
        }
        sin.clear();
    }

    setThreadsNum();
    LOG_INFO << "Reading settings - END";
}

vector<std::string>  listInputFiles(){
    string _filePath = "input";
    const char* path = _filePath.c_str();

    vector<std::string> pathArray;
    int i = 0;
    for(auto & p : boost::filesystem::directory_iterator( path )){
        std::cout << i << " - " << p << std::endl;

        // Convert path entry to string
        std::ostringstream oss;
        oss << p;
        std::string path_item = oss.str();

        // Unescape string
        path_item.erase(0,1);
        path_item.erase(path_item.size() - 1);

        pathArray.push_back(path_item);
        i++;
    }

    return pathArray;
}

void selectInputFile(){
    std::cout << "Input Files:" << std::endl;
    vector<std::string> pathArray = listInputFiles();
    int selectedFile = 0;

    std::cout << "Select Input File:" << std::endl;
    std::cin >> selectedFile;

    if((selectedFile < 0) || (selectedFile > (pathArray.size()-1))){
        std::cout << "Wrong Input File!" << std::endl;
        selectInputFile();
    }

    INPUT_FILE_PATH = pathArray.at(selectedFile);
    LOG_INFO << "Selected Input FILE - " << INPUT_FILE_PATH;
}

void exactInit(int methodId, float prFlag, int selectedCircle) {
    LOG_INFO << "Exact Method - START";
    ExactMethod ExactMethod(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat);
    LOG_INFO << "======= Method Init END =======";

    switch (methodId) {
        case 1:
            ExactMethod.recursiveTest();
            break;
        case 2:
            ExactMethod.recursiveImage();
            break;
        case 3:
            ExactMethod.recursiveMatrix(selectedCircle);
            break;
        case 4:
            ExactMethod.recursiveDiffTest(prFlag);
            break;
        case 5:
            ExactMethod.recursiveDiffImage(prFlag);
            break;
        case 6:
            ExactMethod.recursiveDiffMatrix(prFlag, selectedCircle);
            break;
        default:
            break;
    }

    LOG_INFO << "Exact Method - END";
}

void montecarloInit(int methodId, int iterCount, float prFlag, int selectedCircle){
    LOG_INFO << "MonteCarlo Method - START";
    MonteCarloMethod MonteCarloMethod(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat);
    LOG_INFO << "======= Method Init END =======";

    switch (methodId) {
        case 1:
            MonteCarloMethod.reliabilityTest(iterCount, prFlag);
            break;
        case 2:
            MonteCarloMethod.reliabilityImage(iterCount, prFlag);
            break;
        case 3:
            MonteCarloMethod.reliabilityMatrix(iterCount, prFlag, selectedCircle);
            break;
        case 4:
            MonteCarloMethod.reliabilityExpectedTest(iterCount);
            break;
        case 5:
            MonteCarloMethod.reliabilityExpectedImage(iterCount);
            break;
        case 6:
            MonteCarloMethod.reliabilityExpectedMatrix(iterCount, selectedCircle);
            break;
        default:
            break;
    }

    LOG_INFO << "MonteCarlo Method - END";
}

void montecarloParallelInit(int methodId, int iterCount, float prFlag, int selectedCircle){
    LOG_INFO << "MonteCarlo Method - START";
    MonteCarloMethod MonteCarloMethod(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat);
    LOG_INFO << "======= Method Init END =======";

    switch (methodId) {
        case 1:
            MonteCarloMethod.reliabilityParallelTest(iterCount, prFlag);
            break;
        case 2:
            MonteCarloMethod.reliabilityParallelImage(iterCount, prFlag);
            break;
        case 3:
            MonteCarloMethod.reliabilityParallelMatrix(iterCount, prFlag, selectedCircle);
            break;
        case 4:
            MonteCarloMethod.reliabilityParallelCuda(iterCount, prFlag);
            break;
        case 5:
            MonteCarloMethod.reliabilityParallelExpectedTest(iterCount);
            break;
        case 6:
            MonteCarloMethod.reliabilityParallelExpectedImage(iterCount);
            break;
        case 7:
            MonteCarloMethod.reliabilityParallelExpectedMatrix(iterCount, selectedCircle);
            break;
        case 8:
            MonteCarloMethod.reliabilityParallelExpectedCuda(iterCount);
            break;
        default:
            break;
    }

    LOG_INFO << "MonteCarlo Method - END";
}

int selectMethod() {
    int selectedMethod = 0;
    std::cout << "Select Method:" << std::endl;
    std::cout << "1 - Exact Method" << std::endl;
    std::cout << "2 - MonteCarlo Method" << std::endl;
    std::cout << "3 - MonteCarlo Parallel Method" << std::endl;
    std::cin >> selectedMethod;

    return selectedMethod;
};

int selectExactMethod(){
    int selectedMethod = 0;
    std::cout << "Select Exact Method:" << std::endl;
    std::cout << "1 - Exact Test" << std::endl;
    std::cout << "2 - Exact Image" << std::endl;
    std::cout << "3 - Exact Matrix" << std::endl;
    std::cout << "-----------------------" << std::endl;
    std::cout << "4 - Exact Diff Test" << std::endl;
    std::cout << "5 - Exact Diff Image" << std::endl;
    std::cout << "6 - Exact Diff Matrix" << std::endl;
    std::cin >> selectedMethod;

    return selectedMethod;
}

int selectMonteCarloMethod(){
    int selectedMethod = 0;
    std::cout << "Select Monte Carlo Method:" << std::endl;
    std::cout << "1 - Reliability Test" << std::endl;
    std::cout << "2 - Reliability Image" << std::endl;
    std::cout << "3 - Reliability Matrix" << std::endl;
    std::cout << "-----------------------" << std::endl;
    std::cout << "4 - Expected Reliability Test" << std::endl;
    std::cout << "5 - Expected Reliability Image" << std::endl;
    std::cout << "6 - Expected Reliability Matrix" << std::endl;
    std::cin >> selectedMethod;

    return selectedMethod;
}

int selectMonteCarloParallelMethod(){
    int selectedMethod = 0;
    std::cout << "Select Monte Carlo Parallel Method:" << std::endl;
    std::cout << "1 - Reliability Parallel Test" << std::endl;
    std::cout << "2 - Reliability Parallel Image" << std::endl;
    std::cout << "3 - Reliability Parallel Matrix" << std::endl;
    std::cout << "4 - Reliability Parallel OpenCL" << std::endl;
    std::cout << "-----------------------" << std::endl;
    std::cout << "5 - Expected Reliability Parallel Test" << std::endl;
    std::cout << "6 - Expected Reliability Parallel Image" << std::endl;
    std::cout << "7 - Expected Reliability Parallel Matrix" << std::endl;
    std::cout << "8 - Expected Reliability Parallel OpenCL" << std::endl;
    std::cin >> selectedMethod;

    return selectedMethod;
}

float setProbabilityFlag(){
    float probabilityFlag = 0;
    std::cout << "Set Probability Flag(value should be between 0 & 1):" << std::endl;
    std::cin >> probabilityFlag;

    if ((probabilityFlag > 0) && (probabilityFlag < 1)){
        return probabilityFlag;
    } else {
        std::cout << "Wrong Probability Flag, value should be between 0 & 1" << std::endl;
        setProbabilityFlag();
    }

    return probabilityFlag;
}

unsigned long setRealizationsCount(){
    unsigned long realizationsCount = 0;
    std::cout << "Set Realizations Count:" << std::endl;
    std::cin >> realizationsCount;

    if (realizationsCount > 0){
        return realizationsCount;
    } else {
        std::cout << "Wrong Realizations Count, value should be >= 0" << std::endl;
        setRealizationsCount();
    }

    return realizationsCount;
}

int setDrawCircleAlgorithm(){
    unsigned long selected = 0;
    std::cout << "Select draw circle algorithm:" << std::endl;
    std::cout << "0 - Simple Circle" << std::endl;
    std::cout << "1 - Midpoint Circle" << std::endl;
    std::cin >> selected;

    if (selected >= 0){
        return selected;
    } else {
        std::cout << "Wrong selected algorithm, value should be > 0" << std::endl;
        setDrawCircleAlgorithm();
    }

    return selected;
}

void expectedMethodInitWithArgs(){
expectedMethodSelect:
    int selectedMethod = selectExactMethod();
    switch (selectedMethod){
        case 1:
            exactInit(1, 1, 0);
            break;
        case 2:
            exactInit(2, 1, 0);
            break;
        case 3:
            exactInit(3, 1, setDrawCircleAlgorithm());
            break;
        case 4:
            exactInit(4, setProbabilityFlag(), 0);
            break;
        case 5:
            exactInit(5, setProbabilityFlag(), 0);
            break;
        case 6:
            exactInit(6, setProbabilityFlag(), setDrawCircleAlgorithm());
            break;
        default:
            std::cout << "Wrong Selected Method!" << std::endl;
            goto expectedMethodSelect;
            break;
    }
}

void monteCarloMethodInitWithArgs(){
    monteCarloMethodSelect:
    int selectedMethod = selectMonteCarloMethod();
    switch (selectedMethod){
        case 1:
            montecarloInit(1, setRealizationsCount(), setProbabilityFlag(), 0);
            break;
        case 2:
            montecarloInit(2, setRealizationsCount(), setProbabilityFlag(), 0);
            break;
        case 3:
            montecarloInit(3, setRealizationsCount(), setProbabilityFlag(), setDrawCircleAlgorithm());
            break;
        case 4:
            montecarloInit(4, setRealizationsCount(), 1, 0);
            break;
        case 5:
            montecarloInit(5, setRealizationsCount(), 1, 0);
            break;
        case 6:
            montecarloInit(6, setRealizationsCount(), 1, setDrawCircleAlgorithm());
            break;
        default:
            std::cout << "Wrong Monte Carlo Method!" << std::endl;
            goto monteCarloMethodSelect;
            break;
    }
}

void monteCarloParallelMethodInitWithArgs(){
    monteCarloParallelMethodSelect:
    int selectedMethod = selectMonteCarloParallelMethod();
    switch (selectedMethod){
        case 1:
            montecarloParallelInit(1, setRealizationsCount(), setProbabilityFlag(), 0);
            break;
        case 2:
            montecarloParallelInit(2, setRealizationsCount(), setProbabilityFlag(), 0);
            break;
        case 3:
            montecarloParallelInit(3, setRealizationsCount(), setProbabilityFlag(), setDrawCircleAlgorithm());
            break;
        case 4:
            montecarloParallelInit(4, setRealizationsCount(), setProbabilityFlag(), 0);
            break;
        case 5:
            montecarloParallelInit(5, setRealizationsCount(), 1, 0);
            break;
        case 6:
            montecarloParallelInit(6, setRealizationsCount(), 1, 0);
            break;
        case 7:
            montecarloParallelInit(7, setRealizationsCount(), 1, setDrawCircleAlgorithm());
            break;
        case 8:
            montecarloParallelInit(8, setRealizationsCount(), 1, 0);
            break;
        default:
            std::cout << "Wrong Monte Carlo Parallel Method!" << std::endl;
            goto monteCarloParallelMethodSelect;
            break;
    }
}

void ompTest(){
#pragma omp parallel
    printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
}