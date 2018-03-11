//
// Created by Александр Кучеров on 06/03/2018.
//

#include "utils/libs/simpleLogger/simpleLogger.cpp"

#include "Controllers/Methods/AccurateMethod.cpp"

using namespace boost;


void init() {
    LOG_INFO << "Accurate Method - START";
    AccurateMethod::init();
    LOG_INFO << "Accurate Method - END";
}

int main(int argc, char *argv[])
{
    LOG_INFO << "Program - START";
    init();
    LOG_INFO << "Program - END";
    return 0;
}