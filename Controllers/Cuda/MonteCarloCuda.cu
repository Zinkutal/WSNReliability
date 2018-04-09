#include <cstdlib>
#include <ctime>
#include <vector>

#include "../../Models/Graph.h"

class MonteCarloCuda {
public:
    MonteCarloCuda(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned long kProvided,
                   unsigned long kConnected, unsigned long maxCoverage, float coverageFlag, Graph graphModel) :
            _accuracy(accuracy),
            _oImgSizeX(oImgSizeX),
            _oImgSizeY(oImgSizeY),
            _kProvided(kProvided),
            _kConnected(kConnected),
            _maxCoverage(maxCoverage),
            _coverageFlag(coverageFlag),
            _graphModel(graphModel){
        this->init();
    }

    void init() {
        srand(static_cast <unsigned> (time(0)));// Seeding random number
    }

    // Help function countSquareMatrix method
    void drawCircle(bool **matrix, int x0, int y0, int radius)
    {
        int x = radius;
        int y = 0;
        int xChange = 1 - (radius << 1);
        int yChange = 0;
        int radiusError = 0;

        while (x >= y)
        {
            for (int i = x0 - x; i <= x0 + x; i++)
            {
                if ((i >= 0) && (i < this->_oImgSizeX)) {
                    int yNew = y0 + y;
                    if ((yNew >= 0) && (yNew < this->_oImgSizeY)) matrix[i][yNew] = true;
                    yNew = y0 - y;
                    if ((yNew >= 0) && (yNew < this->_oImgSizeY)) matrix[i][yNew] = true;
                }
            }
            for (int i = x0 - y; i <= x0 + y; i++)
            {
                if ((i >= 0) && (i < this->_oImgSizeX)){
                    int yNew = y0 + x;
                    if ((yNew >= 0) && (yNew < this->_oImgSizeY)) matrix[i][yNew] = true;
                    yNew = y0 - x;
                    if ((yNew >= 0) && (yNew < this->_oImgSizeY)) matrix[i][yNew] = true;
                }
            }

            y++;
            radiusError += yChange;
            yChange += 2;
            if (((radiusError << 1) + xChange) > 0)
            {
                x--;
                radiusError += xChange;
                xChange += 2;
            }
        }
    }

    float countSquareMatrix(vector<float> visited){
        bool** matrix = new bool*[this->_oImgSizeX];
        for(int i = 0; i < this->_oImgSizeX; i++)
            matrix[i] = new bool[this->_oImgSizeY];

        int count_black = 0;

        // Initialize matrix
        for (unsigned int i=0; i < this->_oImgSizeX; i++) {
            for (unsigned int j=0; j < this->_oImgSizeY; j++) {
                matrix[i][j] = false;
            }
        }

        // Draw node circles
        for (int i=1; i < visited.size(); i++ ){
            if (visited.at(i) == 1) {
                int x = this->_graphModel.getNodes().at(i).getCoordinates().at(0);
                int y = this->_graphModel.getNodes().at(i).getCoordinates().at(1);
                int radius = this->_graphModel.getNodes().at(i).getCoverage() * this->_accuracy;
                this->drawCircle(matrix, x, y, radius);
            }
        }

        // Count covered area
        for (unsigned int i=0; i < this->_oImgSizeX; i++) {
            for (unsigned int j=0; j < this->_oImgSizeY; j++) {
                if (matrix[i][j]) {
                    count_black++;
                }
            }
        }

        // Debug Code
        /*for(int i = 0; i < this->_oImgSizeX; i++) {
            for (int j = 0; j < this->_oImgSizeY; j++)
                if (matrix[i][j]) { std::cout << "*"; } else std::cout << " ";
            std::cout << "\n";
        }
        std::cout << "\n --------------- \n";*/

        // Dealloc memory
        delete[] matrix;

        float square = count_black;
        int k = this->_maxCoverage;
        square /= k;

        return square;
    }

    // Return set of visited vertices in connected graph
    void recursiveVertexVisit(vector<float> nodeRel) {
        unsigned int v = 0;

        for (unsigned int i = 0; i < nodeRel.size(); i++) {
            if (nodeRel.at(i) == 1) {
                for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()) {
                    float visitedV = this->getVisitedNodes().at(neighborVertexId);
                    if ((nodeRel.at(neighborVertexId) > 0) && (visitedV != 1)) {
                        v = neighborVertexId;
                        break;
                    }
                }
            }
            if (v > 0) break;
        }

        if (v > 0) {
            vector<float> visited = this->getVisitedNodes();
            visited.at(v) = 1;
            this->setVisitedNodes(visited);
            this->recursiveVertexVisit(nodeRel);
        }
    }

    vector<float> updateGraphConnectivity(vector<float> nodeRel) {
        // Init & fill vector with non-visited vertices
        vector<float> visited;
        for (unsigned long i = 0; i < nodeRel.size(); i++) {
            visited.push_back(0);
        }
        visited.at(0) = 1; // Stock is always connected

        this->setVisitedNodes(visited);
        this->recursiveVertexVisit(nodeRel);

        visited = this->getVisitedNodes();

        for (unsigned long i = 1; i < visited.size(); i++) {
            if (visited.at(i) != 1) {
                nodeRel.at(i) = 0;
            }
        }

        return nodeRel;
    }

    float reliabilityParallelMethodMatrix(vector<float> nodeRel) {
        unsigned long kConn = this->getKConnected();
        vector<float> newRealization;

        unsigned long i, j;
        for (i = 0; i < this->_kProvided; i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }
            newRealization = this->updateGraphConnectivity(newRealization);
            if (this->countSquareMatrix(newRealization) >= this->_coverageFlag) this->setKConnected(kConn++);
        }

        float result = this->getKConnected();
        result /= this->_kProvided;

        return result;
    }
    float reliabilityParallelExpectedMethodMatrix(vector<float> nodeRel) {
        float *kConnectedArr = new float[this->_kProvided];
        vector<float> newRealization;

        unsigned long i, j;
        for (i = 0; i < this->_kProvided; i++) {
            newRealization = nodeRel;
            for (j = 1; j < nodeRel.size(); j++) {
                float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                newRealization.at(j) = (nodeRel.at(j) >= r) ? 1 : 0;
            }

            newRealization = this->updateGraphConnectivity(newRealization);
            kConnectedArr[i] = this->countSquareMatrix(newRealization);
        }

        float kConnected = 0, result = 0;
        for (unsigned long i = 0; i < this->_kProvided; i++) {
            kConnected += kConnectedArr[i];
        }
        result += kConnected;
        result /= this->_kProvided;

        return result;
    }
private:
    unsigned long getKConnected() {
        return this->_kConnected;
    }

    void setKConnected(unsigned long kConnected) {
        this->_kConnected = kConnected;
    }

    vector<float> getVisitedNodes() {
        return this->_visited;
    }

    void setVisitedNodes(vector<float> visited) {
        this->_visited = visited;
    }

    unsigned int _accuracy;
    unsigned int _oImgSizeX;
    unsigned int _oImgSizeY;
    unsigned long _kProvided;
    unsigned long _kConnected;
    unsigned long _maxCoverage;
    float _coverageFlag;
    vector<float> _visited;
    Graph _graphModel;
};