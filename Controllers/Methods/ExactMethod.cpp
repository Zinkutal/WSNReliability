//
// Created by Александр Кучеров on 12/03/2018.
//

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>

#include "Method.h"

using namespace boost;

class ExactMethod : protected Method
{
public:
    ExactMethod(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned int oImgScale,
                const string &oImgFormat) : Method(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat) {
        this->init();
    }

    void recursiveTest(){
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact reliability(test) - START";
        float result_fix = recursiveMethod(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact reliability(test) - END";
    }

    void recursiveRun(){
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact reliability - START";
        float result_fix = recursiveMethod(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact reliability - END";
    }

    void recursiveWithComparsionTest(float coverageFlag){
        this->setCoverageFlag(coverageFlag);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact reliability(test) with comparsion - START";
        float result_fix = recursiveMethod(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact reliability(test) with comparsion - END";
    }

    void recursiveWithComparsionRun(float coverageFlag){
        this->setCoverageFlag(coverageFlag);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact reliability with comparsion - START";
        float result_fix = recursiveComparsionMethodRun(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact reliability with comparsion - END";
    }

private:
    float _coverageFlag;

    float getCoverageFlag(){
        return this->_coverageFlag;
    }

    void setCoverageFlag(float coverageFlag){
        this->_coverageFlag = coverageFlag;
    }

    void init(){
        this->graphInit(this->getGraphModel());

        this->fileItr = 0;
        this->graphToImg("_max_coverage",this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg("_max_coverage"));
        LOG_INFO << "Exact method - Initialized";

        string maxCoveragePath = "_max_coverage";
        this->graphToImg(maxCoveragePath, this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg(maxCoveragePath)); // Count of black pixels for graph with max coverage
        LOG_INFO << "Ratio of black pixels agains all for graph with max coverage - "
                 << this->maxCoverageReadImgRatioAgainstAll(maxCoveragePath);
    }

    vector<float> getGraphProbabilities(){
        Graph g = this->_graphModel;
        vector<float> prVector;
        for ( Node node : g.getNodes()) {
            prVector.push_back(node.getReliablility());
        }
        return prVector;
    }

    float recursiveMethod(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
            if (nodeRel.at(i) == 1){
                for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
                    float pr = nodeRel.at(neighborVertexId);
                    if ((nodeRel.at(neighborVertexId) > 0) && (nodeRel.at(neighborVertexId) < 1)) {
                        v = neighborVertexId;
                        break;
                    }
                }
            }
            if (v > 0) break;
        }

        if (v > 0){
            nodeRel.at(v) = 1;
            result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveMethod(nodeRel);
            nodeRel.at(v) = 0;
            result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveMethod(nodeRel);
        } else if (v  == 0) result = this->countSquare(nodeRel);

        return result;
    }

    float recursiveMethodTest(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
            if (nodeRel.at(i) == 1){
                for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
                    float pr = nodeRel.at(neighborVertexId);
                    if ((nodeRel.at(neighborVertexId) > 0) && (nodeRel.at(neighborVertexId) < 1)) {
                        v = neighborVertexId;
                        break;
                    }
                }
            }
            if (v > 0) break;
        }

        if (v > 0){
            nodeRel.at(v) = 1;
            result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveMethod(nodeRel);
            nodeRel.at(v) = 0;
            result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveMethod(nodeRel);
        } else if (v  == 0) result = this->countSquareTest(nodeRel);

        return result;
    }

    float recursiveComparsionMethodTest(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        if (!(this->countSquareTest(nodeRel) > this->getCoverageFlag())) {
            for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
                if (nodeRel.at(i) == 1){
                    for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
                        float pr = nodeRel.at(neighborVertexId);
                        if ((nodeRel.at(neighborVertexId) > 0) && (nodeRel.at(neighborVertexId) < 1)) {
                            v = neighborVertexId;
                            break;
                        }
                    }
                }
                if (v > 0) break;
            }

            if (v > 0){
                nodeRel.at(v) = 1;
                result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveMethod(nodeRel);
                nodeRel.at(v) = 0;
                result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveMethod(nodeRel);
            } else if (v  == 0) result = this->countSquareTest(nodeRel);
        } else {
            result = 1;
        }

        return result;
    }

    float recursiveComparsionMethodRun(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        float sq = this->countSquare(nodeRel);

        if (sq > this->getCoverageFlag()) {
            result = 1;
        } else {
            for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
                if (nodeRel.at(i) == 1){
                    for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
                        float pr = nodeRel.at(neighborVertexId);
                        if ((nodeRel.at(neighborVertexId) > 0) && (nodeRel.at(neighborVertexId) < 1)) {
                            v = neighborVertexId;
                            break;
                        }
                    }
                }
                if (v > 0) break;
            }

            if (v > 0){
                nodeRel.at(v) = 1;
                result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveComparsionMethodRun(nodeRel);
                nodeRel.at(v) = 0;
                result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveComparsionMethodRun(nodeRel);
            }
        }

        return result;
    }
};
