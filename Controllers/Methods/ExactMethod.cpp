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

    void init(){
        this->graphInit(this->getGraphModel());

        this->fileItr = 0;
        this->graphToImg("_max_coverage",this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg("_max_coverage"));
        LOG_INFO << "Exact method - Initialized";
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
        float result_fix = recursiveMethod(prVector);
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

    vector<float> getGraphProbabilities(){
        Graph g = this->_graphModel;
        vector<float> prVector;
        for ( Node node : g.getNodes()) {
            prVector.push_back(node.getReliablility());
        }
        return prVector;
    }

    float countSquareTest(vector<float> visited){
        int sum_square = 0;
        for (int i=1; i < visited.size(); i++ ){
            if (visited.at(i) == 1) sum_square++;
        }
        return sum_square;
    }

    float countSquare(vector<float> visited){
        Graph graphModel = this->getGraphModel();
        std::vector<Edge> edgeVec;
        unsigned long verticesNum = 0;
        LOG_INFO << "Adding edges to graph - END";
        for (unsigned long i=0; i < visited.size(); i++ ){
            if (visited.at(i) == 1) {
                for (unsigned int neighborVertex: graphModel.getNodes().at(i).getRelations()){
                    for (unsigned long j=0; j < visited.size(); j++ ){
                        if ((j == neighborVertex) && (visited.at(j) == 1)){
                            edgeVec.push_back(Edge(i, neighborVertex));
                        }
                    }
                }
                verticesNum++;
            }
        }
        LOG_INFO << "Adding edges to graph - END";

        LOG_INFO << "Initializing graph with edges - START";
        // GraphInit
        graph_t g(edgeVec.begin(), edgeVec.end(), verticesNum);
        LOG_INFO << "Initializing graph with edges - END";

        this->graphToImg(std::to_string(this->fileItr),g);
        return this->readImg();
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

        if (!(this->countSquare(nodeRel) > this->getCoverageFlag())) {
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
        } else {
            result = 1;
        }

        return result;
    }
};
