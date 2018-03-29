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
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo reliability(test) - START";
        float result = reliabilityMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo reliability(test) - END";
    }

    void reliabilityRun(unsigned long k, float f){
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo reliability - START";
        float result = reliabilityMethodRun(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo reliability - END";
    }

    void reliabilityExpectedTest(unsigned long k){
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo reliability(test) - START";
        float result = reliabilityExpectedMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo reliability(test) - END";
    }

    void reliabilityExpectedRun(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo expected reliability - START";
        float result = reliabilityExpectedMethodRun(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo expected reliability - END";
    }

private:
    unsigned long _kConnected;
    unsigned long _kProvided;
    float _coverageFlag;
    vector<float> _visited;

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

    vector<float> getVisitedNodes(){
        return this->_visited;
    }

    void setVisitedNodes(vector<float> visited){
        this->_visited = visited;
    }

    void init(){
        this->graphInit(this->getGraphModel());

        this->fileItr = 0;
        this->graphToImg("_max_coverage",this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg("_max_coverage"));
        this->setKConnected(0);
        // Seeding random number
        srand (static_cast <unsigned> (time(0)));

        LOG_INFO << "Monte-Carlo method - Initialized";

        this->graphToImg("_max_coverage",this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg("_max_coverage")); // Count of black pixels for graph with max coverage
        LOG_INFO << "Ratio of black pixels agains all for graph with max coverage - " << this->maxCoverageReadImgRatioAgainstAll("_max_coverage");
    }

    vector<float> getGraphProbabilities(){
        Graph g = this->_graphModel;
        vector<float> prVector;
        for ( Node node : g.getNodes()) {
            prVector.push_back(node.getReliablility());
        }
        return prVector;
    }

    // Return set of visited vertices in connected graph
    vector<float> recursiveVertexVisit(vector<float> nodeRel){
        unsigned int v = 0;

        for (unsigned int i=0; i < this->_graphModel.getNodes().size(); i++){
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
            this->recursiveVertexVisit(nodeRel);
        }

        return nodeRel;
    }

    vector<float> updateGraphConnectivity(vector<float> nodeRel){
        // Init & fill vector with non-visited vertices
        vector<float> visited;
        for (unsigned long i=0; i < nodeRel.size(); i++){
            visited.push_back(0);
        }

        visited = this->recursiveVertexVisit(nodeRel);

        for (unsigned long i=1; i < visited.size(); i++){
            if(visited.at(i) != 1){
                nodeRel.at(i) = 0;
            }
        }
        nodeRel.at(0) = 1; // Stock is always connected

        return nodeRel;
    }

    float reliabilityMethodTest(vector<float> nodeRel){
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization = nodeRel;

        for (unsigned long i=0; i < this->getKProvided(); i++){
            for (unsigned long j=1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            if (this->countSquareTest(newRealization) >= this->getCoverageFlag()) this->setKConnected(kConn++);
        }

        float result = this->getKConnected();
        result /= this->getKProvided();

        return result;
    }

    float reliabilityMethodRun(vector<float> nodeRel){
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization = nodeRel;

        for (unsigned long i=0; i < this->getKProvided(); i++){
            for (unsigned long j=1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            if (this->countSquare(newRealization) >= this->getCoverageFlag()) this->setKConnected(kConn++);
        }

        float result = this->getKConnected();
        result /= this->getKProvided();

        return result;
    }

    float reliabilityExpectedMethodTest(vector<float> nodeRel){
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization = nodeRel;

        for (unsigned long i=0; i < this->getKProvided(); i++){
            for (unsigned long j=1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareTest(newRealization);
        }

        float kConnected = 0 ,result = 0;
        for (unsigned long i=0; i < this->getKProvided(); i++){
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }

    float reliabilityExpectedMethodRun(vector<float> nodeRel){
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization = nodeRel;

        for (unsigned long i=0; i < this->getKProvided(); i++){
            for (unsigned long j=1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareTest(newRealization);
        }

        float kConnected = 0 ,result = 0;
        for (unsigned long i=0; i < this->getKProvided(); i++){
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }
};