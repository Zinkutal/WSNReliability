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
#include <omp.h>
#include "../OpenCL/MonteCarloOpenCL.cpp"

using namespace boost;

class MonteCarloMethod : protected Method {
public:
    MonteCarloMethod(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned int oImgScale,
                     const string &oImgFormat) : Method(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat) {
        this->init();
    }

    void reliabilityTest(unsigned long k, float f) {
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo reliability (test) - START";
        float result = reliabilityMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo reliability (test) - END";
    }

    void reliabilityImage(unsigned long k, float f) {
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Reliability (image) - START";
        float result = reliabilityMethodImage(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Reliability (image) - END";
    }

    void reliabilityMatrix(unsigned long k, float f) {
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Reliability (matrix) - START";
        float result = reliabilityMethodMatrix(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Reliability (matrix) - END";
    }

    void reliabilityExpectedTest(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Expected Reliability (test) - START";
        float result = reliabilityExpectedMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Expected Reliability (test) - END";
    }

    void reliabilityExpectedImage(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Expected Reliability (image) - START";
        float result = reliabilityExpectedMethodImage(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Expected Reliability (image) - END";
    }

    void reliabilityExpectedMatrix(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Expected Reliability (matrix) - START";
        float result = reliabilityExpectedMethodMatrix(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Expected Reliability (matrix) - END";
    }

/*
 * Parallel
 */

    void reliabilityParallelTest(unsigned long k, float f) {
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Reliability (test) - START";
        float result = reliabilityParallelMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Reliability (test) - END";
    }

    void reliabilityParallelImage(unsigned long k, float f) {
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Reliability (image) - START";
        float result = reliabilityParallelMethodImage(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Reliability (image) - END";
    }

    void reliabilityParallelMatrix(unsigned long k, float f) {
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Reliability (matrix) - START";
        float result = reliabilityParallelMethodMatrix(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Reliability (matrix) - END";
    }

    void reliabilityParallelCuda(unsigned long k, float f) {
        this->setKProvided(k);
        this->setKConnected(0);
        this->setCoverageFlag(f);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Reliability (opencl) - START";
        mainx();
        float result = 0.0;
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Reliability (opencl) - END";
    }

    void reliabilityParallelExpectedTest(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (test) - START";
        float result = reliabilityParallelExpectedMethodTest(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (test) - END";
    }

    void reliabilityParallelExpectedImage(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (image) - START";
        float result = reliabilityParallelExpectedMethodImage(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (image) - END";
    }

    void reliabilityParallelExpectedMatrix(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (matrix) - START";
        float result = reliabilityParallelExpectedMethodMatrix(prVector);
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (matrix) - END";
    }

    void reliabilityParallelExpectedCuda(unsigned long k) {
        this->setKProvided(k);
        this->setKConnected(0);
        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (opencl) - START";
        mainx();
        float result = 0.0;
        LOG_INFO << "WSN Network Reliability: " << result;
        LOG_INFO << "MonteCarlo Parallel Expected Reliability (opencl) - END";
    }

private:
    unsigned long _kConnected;
    unsigned long _kProvided;
    float _coverageFlag;

    unsigned long getKConnected() {
        return this->_kConnected;
    }

    void setKConnected(unsigned long kConnected) {
        this->_kConnected = kConnected;
    }

    unsigned long getKProvided() {
        return this->_kProvided;
    }

    void setKProvided(unsigned long kProvided) {
        this->_kProvided = kProvided;
    }

    float getCoverageFlag() {
        return this->_coverageFlag;
    }

    void setCoverageFlag(float coverageFlag) {
        this->_coverageFlag = coverageFlag;
    }

    void init() {
        this->graphInit(this->getGraphModel());

        this->fileItr = 0;
        this->setMaxCoverageInit();
        this->setKConnected(0);
        srand(static_cast <unsigned> (time(0)));// Seeding random number

        LOG_INFO << "Monte-Carlo method - Initialized";
    }

    float reliabilityMethodTest(vector<float> nodeRel) {
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization;

        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (unsigned long j = 1; j < nodeRel.size(); j++) {
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

    float reliabilityMethodImage(vector<float> nodeRel) {
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization;

        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (unsigned long j = 1; j < nodeRel.size(); j++) {
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

    float reliabilityMethodMatrix(vector<float> nodeRel) {
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization;

        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (unsigned long j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            if (this->countSquareMatrix(newRealization, Realization) >= this->getCoverageFlag()) this->setKConnected(kConn++);
        }

        float result = this->getKConnected();
        result /= this->getKProvided();

        return result;
    }

    float reliabilityExpectedMethodTest(vector<float> nodeRel) {
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization;

        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (unsigned long j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareTest(newRealization);
        }

        float kConnected = 0, result = 0;
        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }

    float reliabilityExpectedMethodImage(vector<float> nodeRel) {
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization;

        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (unsigned long j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquare(newRealization);
        }

        float kConnected = 0, result = 0;
        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }

    float reliabilityExpectedMethodMatrix(vector<float> nodeRel) {
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization;

        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (unsigned long j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareMatrix(newRealization, Realization);
        }

        float kConnected = 0, result = 0;
        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }

/*
 * Parallel
 */

    float reliabilityParallelMethodTest(vector<float> nodeRel) {
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization;

        unsigned long i, j;
#pragma omp parallel for private(j, newRealization)
        for (i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
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

    float reliabilityParallelMethodImage(vector<float> nodeRel) {
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization;

        unsigned long i, j, itr=0;
#pragma omp parallel for private(j, newRealization)
        for (i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            if (this->countSquareParallel(newRealization, itr) >= this->getCoverageFlag()) this->setKConnected(kConn++);
            itr++;
        }

        float result = this->getKConnected();
        result /= this->getKProvided();

        return result;
    }

    float reliabilityParallelMethodMatrix(vector<float> nodeRel) {
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization;

        unsigned long i, j;
#pragma omp parallel for private(j, newRealization)
        for (i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            if (this->countSquareMatrix(newRealization, Realization) >= this->getCoverageFlag()) this->setKConnected(kConn++);
        }

        float result = this->getKConnected();
        result /= this->getKProvided();

        return result;
    }

    float reliabilityParallelExpectedMethodTest(vector<float> nodeRel) {
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization;

        unsigned long i, j;
#pragma omp parallel for private(j, newRealization)
        for (i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareTest(newRealization);
        }

        float kConnected = 0, result = 0;
        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }

    float reliabilityParallelExpectedMethodImage(vector<float> nodeRel) {
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization;

        unsigned long i, j, itr=0;
#pragma omp parallel for private(j, newRealization)
        for (i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }

            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareParallel(newRealization, itr);
            itr++;
        }

        float kConnected = 0, result = 0;
        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }

    float reliabilityParallelExpectedMethodMatrix(vector<float> nodeRel) {
        float *kConnectedArr = new float[this->getKProvided()];
        vector<float> newRealization;

        unsigned long i, j;
#pragma omp parallel for private(j, newRealization)
        for (i = 0; i < this->getKProvided(); i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }

            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareMatrix(newRealization, Realization);
        }

        float kConnected = 0, result = 0;
        for (unsigned long i = 0; i < this->getKProvided(); i++) {
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->getKProvided();

        return result;
    }
};