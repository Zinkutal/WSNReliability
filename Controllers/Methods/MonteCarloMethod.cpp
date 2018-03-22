//
// Created by Александр Кучеров on 22/03/2018.
//


#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>

#include "Method.h"

using namespace boost;

class MonteCarloMethod : protected Method
{
public:
    MonteCarloMethod(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned int oImgScale,
                     const string &oImgFormat) : Method(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat) {
        this->init();
    }
private:
    void init(){
        this->graphInit(this->getGraphModel());

        this->fileItr = 0;
        this->graphToImg("_max_coverage",this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg("_max_coverage"));
        LOG_INFO << "Exact method - Initialized";
    }
};