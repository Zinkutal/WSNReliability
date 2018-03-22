//
// Created by Александр Кучеров on 12/03/2018.
//

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>

#include "Method.h"
#define INPUT_FILE "../src/data/input/json/graph_input_4.json"

using namespace boost;

class ExactMethod : protected Method
{
public:
    ExactMethod(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned int oImgScale,
                const string &oImgFormat) : Method(accuracy, oImgSizeX, oImgSizeY, oImgScale, oImgFormat) {

    }

    void init(){
        this->graphInit(this->getGraphModel());

        this->fileItr = 0;
        this->graphToImg("_max_coverage",this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg("_max_coverage"));

        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Custom accurate reliability(fix) - START";
        float result_fix = factorialR(prVector);
        LOG_INFO << "Custom accurate reliability(fix) - END";
        LOG_INFO << "WSN Network Reliability: " << result_fix;
    }

private:
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

        /*int sum_square = 0;
        for (int i=1; i < visited.size(); i++ ){
            if (visited.at(i) == 1) sum_square++;
        }
        return sum_square;*/
    }

    vector<float> getGraphProbabilities(){
        Graph g = this->_graphModel;
        vector<float> prVector;
        for ( Node node : g.getNodes()) {
            prVector.push_back(node.getReliablility());
        }
        return prVector;
    }

    float factorialR(vector<float> nodeRel){
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
            result = this->_graphModel.getNodes().at(v).getReliablility() * factorialR(nodeRel);
            nodeRel.at(v) = 0;
            result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * factorialR(nodeRel);
        } else if (v  == 0) result = this->countSquare(nodeRel);

        return result;
    }
};
