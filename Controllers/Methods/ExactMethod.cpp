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
        LOG_INFO << "Exact Reliability (test) - START";
        float result_fix = recursiveMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact Reliability (test) - END";
    }

    void recursiveImage(){
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact Reliability (image) - START";
        float result_fix = recursiveMethodImage(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact Reliability (image) - END";
    }

    void recursiveMatrix(int drawCircle){
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact Reliability (matrix) - START";
        float result_fix = recursiveMethodMatrix(prVector, drawCircle);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact Reliability (matrix) - END";
    }

    void recursiveDiffTest(float coverageFlag){
        this->setCoverageFlag(coverageFlag);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact Diff Reliability (test) - START";
        float result_fix = recursiveMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact Diff Reliability (test) - END";
    }

    void recursiveDiffImage(float coverageFlag){
        this->setCoverageFlag(coverageFlag);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact Diff reliability (image) - START";
        float result_fix = recursiveDiffMethodImage(prVector);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact Diff reliability (image) - END";
    }

    void recursiveDiffMatrix(float coverageFlag, int drawCircle){
        this->setCoverageFlag(coverageFlag);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Exact Diff reliability (matrix) - START";
        float result_fix = recursiveDiffMethodMatrix(prVector, drawCircle);
        LOG_INFO << "WSN Network Reliability: " << result_fix;
        LOG_INFO << "Exact Diff reliability (matrix) - END";
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
        this->setMaxCoverageInit();

        LOG_INFO << "Exact method - Initialized";
    }

    float recursiveMethodImage(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
            if (nodeRel.at(i) == 1){
                for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
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
            result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveMethodImage(nodeRel);
            nodeRel.at(v) = 0;
            result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveMethodImage(nodeRel);
        } else if (v  == 0) result = this->countSquare(nodeRel);

        return result;
    }

    float recursiveMethodTest(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
            if (nodeRel.at(i) == 1){
                for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
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
            result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveMethodTest(nodeRel);
            nodeRel.at(v) = 0;
            result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveMethodTest(nodeRel);
        } else if (v  == 0) result = this->countSquareTest(nodeRel);

        return result;
    }

    float recursiveMethodMatrix(vector<float> nodeRel, int drawCircle){
        float result = 0;
        unsigned int v = 0;

        for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
            if (nodeRel.at(i) == 1){
                for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
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
            result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveMethodMatrix(nodeRel, drawCircle);
            nodeRel.at(v) = 0;
            result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveMethodMatrix(nodeRel, drawCircle);
        } else if (v  == 0) result = this->countSquareMatrix(nodeRel, Realization, drawCircle);

        return result;
    }

    float recursiveDiffMethodTest(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        if (!(this->countSquareTest(nodeRel) > this->getCoverageFlag())) {
            for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
                if (nodeRel.at(i) == 1){
                    for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
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
                result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveDiffMethodTest(nodeRel);
                nodeRel.at(v) = 0;
                result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveDiffMethodTest(nodeRel);
            } else if (v  == 0) result = this->countSquareTest(nodeRel);
        } else {
            result = 1;
        }

        return result;
    }

    float recursiveDiffMethodImage(vector<float> nodeRel){
        float result = 0;
        unsigned int v = 0;

        float sq = this->countSquare(nodeRel);

        if (sq > this->getCoverageFlag()) {
            result = 1;
        } else {
            for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
                if (nodeRel.at(i) == 1){
                    for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
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
                result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveDiffMethodImage(nodeRel);
                nodeRel.at(v) = 0;
                result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveDiffMethodImage(nodeRel);
            }
        }

        return result;
    }

    float recursiveDiffMethodMatrix(vector<float> nodeRel, int drawCircle){
        float result = 0;
        unsigned int v = 0;

        float sq = this->countSquareMatrix(nodeRel, Realization, drawCircle);

        if (sq > this->getCoverageFlag()) {
            result = 1;
        } else {
            for (unsigned int i=0; i<this->_graphModel.getNodes().size(); i++){
                if (nodeRel.at(i) == 1){
                    for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
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
                result = this->_graphModel.getNodes().at(v).getReliablility() * recursiveDiffMethodMatrix(nodeRel, drawCircle);
                nodeRel.at(v) = 0;
                result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * recursiveDiffMethodMatrix(nodeRel, drawCircle);
            }
        }

        return result;
    }
};
