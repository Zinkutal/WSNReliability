//
// Created by Александр Кучеров on 22/03/2018.
//


#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>

#include <cstdlib>
#include <ctime>

#include "Method.h"

using namespace boost;

class MonteCarloMethod : protected Method
{
public:
    MonteCarloMethod(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned int oImgScale,
                     const string &oImgFormat) : Method(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat) {
        this->init();
    }

    void reliabilityTest(unsigned long k, float f){
        this->setKProvided(k);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo reliability(test) - START";
        float result = reliabilityMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo reliability(test) - END";
    }

    void reliabilityRun(unsigned long k, float f){
        this->setKProvided(k);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo reliability - START";
        float result = reliabilityMethodRun(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo reliability - END";
    }

    void reliabilityExpectedTest(unsigned long k){
        this->setKProvided(k);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo reliability(test) - START";
        float result = reliabilityMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo reliability(test) - END";
    }

    void reliabilityExpectedRun(unsigned long k) {
        this->setKProvided(k);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo expected reliability - START";
        float result = reliabilityMethodRun(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo expected reliability - END";
    }

private:
    unsigned long _kConnected;
    unsigned long _kProvided;
    float _coverageFlag;

    unsigned long getKConnected(){
        return this->_kConnected;
    }

    void setKConnected(unsigned long kConnected){
        this->_kConnected = kConnected;
    }

    unsigned long getKProvided(){
        return this->_kProvided;
    }

    void setKProvided(unsigned long kProvided){
        this->_kProvided = kProvided;
    }

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
        this->setKConnected(0);
        // Seeding random number
        srand (static_cast <unsigned> (time(0)));

        LOG_INFO << "Exact method - Initialized";
    }

    vector<float> getGraphProbabilities(){
        Graph g = this->_graphModel;
        vector<float> prVector;
        for ( Node node : g.getNodes()) {
            prVector.push_back(node.getReliablility());
        }
        return prVector;
    }

    float reliabilityMethodTest(vector<float> nodeRel){
        if (this->getKProvided() > nodeRel.size()){
            string errorMsg = "Provided k cannnot be greater then amount of nodes";
            LOG_ERROR << errorMsg;
            throw (errorMsg);
        }

        for (unsigned long i; i < this->getKProvided(); i++){
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            nodeRel.at(i) = (nodeRel.at(i) >= r) ? 1 : 0;
            unsigned long kConn = this->getKConnected();
            if (this->countSquareTest(nodeRel) >= this->getCoverageFlag()) this->setKConnected(kConn++);
        }

        return this->getKConnected()/this->getKProvided();
    }

    float reliabilityMethodRun(vector<float> nodeRel){
        if (this->getKProvided() > nodeRel.size()){
            string errorMsg = "Provided k cannnot be greater then amount of nodes";
            LOG_ERROR << errorMsg;
            throw (errorMsg);
        }

        for (unsigned long i; i < this->getKProvided(); i++){
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            nodeRel.at(i) = (nodeRel.at(i) >= r) ? 1 : 0;
            unsigned long kConn = this->getKConnected();
            if (this->countSquare(nodeRel) >= this->getCoverageFlag()) this->setKConnected(kConn++);
        }

        return this->getKConnected()/this->getKProvided();
    }

    float reliabilityExpectedMethodTest(vector<float> nodeRel){
        if (this->getKProvided() > nodeRel.size()){
            string errorMsg = "Provided k cannnot be greater then amount of nodes";
            LOG_ERROR << errorMsg;
            throw (errorMsg);
        }

        float resultArr[this->getKProvided()];
        for (unsigned long i; i < this->getKProvided(); i++){
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            nodeRel.at(i) = (nodeRel.at(i) >= r) ? 1 : 0;
            resultArr[i] = this->countSquareTest(nodeRel);
        }

        float result = 0;
        for (unsigned long i=0; i< this->getKProvided(); i++){
            result+=resultArr[i];
        }

        return result/this->getKProvided();
    }

    float reliabilityExpectedMethodRun(vector<float> nodeRel){
        if (this->getKProvided() > nodeRel.size()){
            string errorMsg = "Provided k cannnot be greater then amount of nodes";
            LOG_ERROR << errorMsg;
            throw (errorMsg);
        }

        float resultArr[this->getKProvided()];
        for (unsigned long i; i < this->getKProvided(); i++){
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            nodeRel.at(i) = (nodeRel.at(i) >= r) ? 1 : 0;
            resultArr[i] = this->countSquare(nodeRel);
        }

        float result = 0;
        for (unsigned long i=0; i< this->getKProvided(); i++){
            result+=resultArr[i];
        }

        return result/this->getKProvided();
    }
};