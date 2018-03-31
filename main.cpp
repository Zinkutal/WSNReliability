//
// Created by Александр Кучеров on 06/03/2018.
//

#include "Utils/initUtil.cpp"

int main(int argc, char **argv)
{
    LOG_INFO << "Program - START";
    prepareOutputDir();
    prepareInputDir();
    prepareConfigDir();
    LOG_INFO << "Directories - Prepared";
    setConfig();
    LOG_INFO << "Settings - Applied";

    selectInputFile();

selectMethod:
    int selectedMethod = selectMethod();
    switch (selectedMethod) {
        case 0:
            ompTest();
            break;
        case 1:
            expectedMethodInitWithArgs();
            break;
        case 2:
            monteCarloMethodInitWithArgs();
            break;
        case 3:
            monteCarloParallelMethodInitWithArgs();
            break;
        default:
            std::cout << "Wrong method.." << std::endl;
            goto selectMethod;
            break;
    }
    LOG_INFO << "Program - END";

    return EXIT_SUCCESS;
}