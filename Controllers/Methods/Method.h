//
// Created by Александр Кучеров on 22/03/2018.
//

#ifndef METHOD_H
#define METHOD_H

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/breadth_first_search.hpp"
#include "boost/graph/depth_first_search.hpp"
#include "boost/graph/graphviz.hpp"
#include "boost/algorithm/string.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

enum matrixCoverage {
    Realization,
    MaxCoverageAgainstAll,
    MaxCoverage
};

enum matrixDrawCircle {
    SimpleCircle,
    MidpointCircle
};

extern std::string INPUT_FILE_PATH("");//("input/graph_input.json");

#include "../../Models/Graph.h"

using namespace boost;

typedef std::pair<int, int> Edge;
typedef adjacency_list<vecS, vecS, undirectedS,
        property<edge_color_t, default_color_type> > graph_t;
typedef graph_traits<graph_t>::vertex_descriptor vertex_t;

class custom_bfs_visitor : public default_bfs_visitor
{
public:
    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex v, const Graph & g) const
    {
        LOG_DEBUG << "Visited vertex - " << v;
    }
};

class custom_dfs_visitor : public default_dfs_visitor
{
public:
    void discover_vertex(vertex_t v, const graph_t& g) const
    {
        LOG_DEBUG << "Visited vertex - " << v;
    }
};

// Graphviz property writer (vertex)
class custom_vertex_writer {
public:
    custom_vertex_writer( graph_t& g , Graph& gModel) : currentGraph( g ), currentGraphModel(gModel) {}

    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& e) const {
        float coverage = this->currentGraphModel.getNodes().at(e).getCoverage();
        float posX = this->currentGraphModel.getNodes().at(e).getCoordinates().at(0);
        float posY = this->currentGraphModel.getNodes().at(e).getCoordinates().at(1);
        if (e != 0) {
            out << "[shape=circle,width=" << coverage
                << ",style=filled,fillcolor=\"#000000\",pos=\""
                << posX << "," << posY << "!\", pin=true]";
        } else {
            out << "[shape=circle,width=" << coverage
                << ",style=invis,pos=\""
                << posX << "," << posY << "!\", pin=true]";
        }

    }
private:
    graph_t& currentGraph;
    Graph&   currentGraphModel;
};

// Graphviz property writer (edge)
class custom_edge_writer {
public:
    custom_edge_writer( graph_t& g) : currentGraph( g ) {}

    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& e) const {
        out << "[style=invis]";
    }
private:
    graph_t& currentGraph;
};

class Method {
public:
    Method(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned int oImgScale, string oImgFormat)
            : _accuracy(accuracy),
              _oImgSizeX(oImgSizeX),
              _oImgSizeY(oImgSizeY),
              _oImgCoordScale(oImgScale),
              _oImgFormat(oImgFormat),
              _graphModel(Graph::initWithFile(INPUT_FILE_PATH.c_str())){
        LOG_DEBUG << "Method - Initialized";
        this->init();
    }

    void init(){
        this->graphInit(this->getGraphModel());

        LOG_DEBUG << "Boost BFS - START";
        this->boost_bfs();
        LOG_DEBUG << "Boost BFS - END";

        LOG_DEBUG << "Boost DFS - START";
        this->boost_dfs();
        LOG_DEBUG << "Boost DFS - END";


        this->fileItr = 0;
        this->setMaxCoverageInit();

        LOG_INFO << "Method - Initialized";
    }
    unsigned int getAccuracy(){
        return this->_accuracy;
    }

    void setAccuracy(unsigned int accuracy){
        this->_accuracy = accuracy;
    }

    std::vector<Edge> getEdgeVector(){
        return this->_edgeVector;
    }

    void setEdgeVector(std::vector<Edge> edgeVector){
        this->_edgeVector = edgeVector;
    }

    graph_t getUndirectedGraph(){
        return this->_graph_t;
    }

    void setUndirectedGraph(graph_t undGraph){
        this->_graph_t = undGraph;
    }

    Graph getGraphModel(){
        return this->_graphModel;
    }

    void setGraphModel(Graph graphModel){
        this->_graphModel = graphModel;
    }

    unsigned long getMaxCoverage(){
        return this->_maxCoverage;
    }

    void setMaxCoverage(unsigned long maxCoverage){
        this->_maxCoverage = maxCoverage;
    }

    unsigned long getMaxCoverageMatrixSimple(){
        return this->_maxCoverageMatrixSimple;
    }

    void setMaxCoverageMatrixSimple(unsigned long maxCoverageMatrixSimple){
        this->_maxCoverageMatrixSimple = maxCoverageMatrixSimple;
    }

    unsigned long getMaxCoverageMatrixMidpoint(){
        return this->_maxCoverageMatrixMidpoint;
    }

    void setMaxCoverageMatrixMidpoint(unsigned long maxCoverageMatrixMidpoint){
        this->_maxCoverageMatrixMidpoint = maxCoverageMatrixMidpoint;
    }
protected:
    vector<float> getVisitedNodes() {
        return this->_visited;
    }

    void setVisitedNodes(vector<float> visited) {
        this->_visited = visited;
    }

    void graphInit(Graph graphModel) {
        LOG_DEBUG << "Adding edges to graph - START";
        std::vector<Edge> edgeVec;
        for ( Node node : graphModel.getNodes()) {
            for (unsigned int neighborVertex: node.getRelations()){
                edgeVec.push_back(Edge(node.getId(), neighborVertex));
            }
        }
        this->setEdgeVector(edgeVec);
        LOG_DEBUG << "Adding edges to graph - END";

        LOG_DEBUG << "Initializing graph with edges - START";
        // GraphInit
        graph_t g(edgeVec.begin(), edgeVec.end(), graphModel.getNodes().size());

        graph_traits<graph_t>::vertex_iterator vi, vi_end, next;
        tie(vi, vi_end) = vertices(g);
        for (next = vi; vi != vi_end; vi = next) {
            ++next;
        }

        this->setUndirectedGraph(g);
        LOG_DEBUG << "Edges Num - " << num_edges(g);
        LOG_DEBUG << "Initializing graph with edges - END";

        // List all edges
        typedef graph_traits<graph_t>::edge_iterator edge_iterator;
        std::pair<edge_iterator, edge_iterator> ei = edges(g);
        for(edge_iterator edge_iter = ei.first; edge_iter != ei.second; ++edge_iter) {
            LOG_DEBUG << "Edge (" << source(*edge_iter, g) << ", " << target(*edge_iter, g) << ")";
        }
    }

    void setMaxCoverageInit(){
        string maxCoveragePath = "_max_coverage";
        this->graphToImg(maxCoveragePath, this->_graph_t);
        this->setMaxCoverage(this->countSquareImage(maxCoveragePath, MaxCoverage)); // Count of black pixels for graph with max coverage
        LOG_INFO << "Ratio of black pixels against all for graph with max coverage (image) - "
                 << this->countSquareImage(maxCoveragePath, MaxCoverageAgainstAll);

        vector<float> visited;
        for (int i=0; i < this->getGraphProbabilities().size(); i++){
            visited.push_back(1);
        }
        this->setMaxCoverageMatrixSimple(this->countSquareMatrix(visited, MaxCoverage, SimpleCircle)); // Count of black pixels for graph with max coverage
        LOG_INFO << "Ratio of black pixels against all for graph with max coverage (matrix - simpleCircle) - "
                  << this->countSquareMatrix(visited, MaxCoverageAgainstAll, SimpleCircle);
        this->setMaxCoverageMatrixMidpoint(this->countSquareMatrix(visited, MaxCoverage, MidpointCircle)); // Count of black pixels for graph with max coverage
        LOG_INFO << "Ratio of black pixels against all for graph with max coverage (matrix - midpointCircle) - "
                 << this->countSquareMatrix(visited, MaxCoverageAgainstAll, MidpointCircle);
    }

    vector<float> getGraphProbabilities() {
        Graph g = this->_graphModel;
        vector<float> prVector;
        for (Node node : g.getNodes()) {
            prVector.push_back(node.getReliablility());
        }
        return prVector;
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

    void graphToImg(string filename,graph_t g){
        //write_graphviz (std::cout, g);
        std::ofstream dmp;
        string imgPath = "output/graph" + filename + "." + this->_oImgFormat;
        string dotPath = "output/graph" + filename + ".dot";
        dmp.open(dotPath);
        write_graphviz(dmp, g, custom_vertex_writer(g, this->_graphModel), custom_edge_writer(g));
        string outFormat = "neato";
        string gScale = "-n -s" + std::to_string(this->_oImgCoordScale);
        string gFormat = "-T" + this->_oImgFormat;
        string gSize = "-Gsize="
                       + std::to_string(this->_oImgSizeX) + ","
                       + std::to_string(this->_oImgSizeY) + "!";
        string gViewport = "-Gviewport="
                           + std::to_string(this->_oImgSizeX) + ","
                           + std::to_string(this->_oImgSizeY) + ","
                           + std::to_string(this->_oImgCoordScale) + "!";
        string gDpi = "-Gdpi=" + std::to_string(this->_accuracy);
        string cmd = outFormat +
                     " " + gScale +
                     " " + dotPath +
                     " " + gFormat +
                     " " + gSize +
                     " " + gViewport +
                     " " + gDpi +
                     " -o " + imgPath;
        const char * dotCmd = cmd.c_str();

        std::system(dotCmd);
    }

    /*
     * Returns ratio of black pixels against amount of
     * black pixels for graph whose each node probability is 1
     * */
    float readImg(){
        string imgPath = "output/graph" + std::to_string(this->fileItr) + "." + this->_oImgFormat;

        cv::Mat image;
        image = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);

        if(! image.data) LOG_ERROR << "Could not open or find the image - " << imgPath;

        // Display img
        /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
        cv::imshow( "Display window", image );
        cv::waitKey(0);*/

        // Prepare Image
        cv::cvtColor(image, image, CV_BGR2GRAY);
        cv::threshold(image, image, 254, 255, cv::THRESH_BINARY );
        // Count Pixels
        int count_all   = image.cols * image.rows;
        int count_white = cv::countNonZero(image);
        int count_black = count_all - count_white;

        LOG_DEBUG << "All pixels - " << count_all << "; White pixels - " << count_white
                  << "; Black pixels - " << count_black;

        this->fileItr++;

        float square = count_black;
        square /= this->getMaxCoverage();

        return square;
    }

    /*
     * FOR PARALLEL METHODS
     * Returns ratio of black pixels against amount of
     * black pixels for graph whose each node probability is 1
     * */
    float readImgParallel(unsigned long fileItr){
        string imgPath = "output/graph" + std::to_string(fileItr) + "." + this->_oImgFormat;

        cv::Mat image;
        image = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);

        if(! image.data) LOG_ERROR << "Could not open or find the image - " << imgPath;

        // Display img
        /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
        cv::imshow( "Display window", image );
        cv::waitKey(0);*/

        // Prepare Image
        cv::cvtColor(image, image, CV_BGR2GRAY);
        cv::threshold(image, image, 254, 255, cv::THRESH_BINARY );
        // Count Pixels
        int count_all   = image.cols * image.rows;
        int count_white = cv::countNonZero(image);
        int count_black = count_all - count_white;

        LOG_DEBUG << "All pixels - " << count_all << "; White pixels - " << count_white
                  << "; Black pixels - " << count_black;

        float square = count_black;
        square /= this->getMaxCoverage();;

        return square;
    }

    float countSquareTest(vector<float> visited){
        int sum_square = 0;
        for (int i=1; i < visited.size(); i++ ){
            if (visited.at(i) == 1) sum_square++;
        }
        return sum_square;
    }

    float countSquareImage(string filename, int type){
        string imgPath = "output/graph" + filename + "." + this->_oImgFormat;

        cv::Mat image;
        image = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);

        if(! image.data) LOG_ERROR << "Could not open or find the image - " << imgPath;

        // Display img
        /*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
        cv::imshow( "Display window", image );
        cv::waitKey(0);*/

        // Prepare Image
        cv::cvtColor(image, image, CV_BGR2GRAY);
        cv::threshold(image, image, 254, 255, cv::THRESH_BINARY );
        // Count Pixels
        int count_all   = image.cols * image.rows;
        int count_white = cv::countNonZero(image);
        int count_black = count_all - count_white;

        LOG_DEBUG << "All pixels - " << count_all << "; White pixels - " << count_white
                  << "; Black pixels - " << count_black;

        float square = 0;

        switch(type){
            case Realization: // Check readImg() && readImgParallel() methods
                break;
            case MaxCoverageAgainstAll: //Returns ratio black/all pixel
                square = count_black;
                square /= count_all;
                break;
            case MaxCoverage:// Returns count of black pixels for all graph, where p of every node is 1
                square = count_black;
            default:
                break;
        }

        return square;
    }

    float countSquareMatrix(vector<float> visited, int type, int drawCircle){
        bool** matrix = new bool*[this->_oImgSizeX];
        for(int i = 0; i < this->_oImgSizeX; i++)
            matrix[i] = new bool[this->_oImgSizeY];

        int count_all = this->_oImgSizeX * this->_oImgSizeY;
        int count_black = 0;

        // Initialize matrix
        for (unsigned int i=0; i < this->_oImgSizeX; i++) {
            for (unsigned int j=0; j < this->_oImgSizeY; j++) {
                matrix[i][j] = false;
            }
        }

        // Draw node circles
        for (int i=1; i < visited.size(); i++ ){
            if (visited.at(i) == 1){
                int x = this->_graphModel.getNodes().at(i).getCoordinates().at(0);
                int y = this->_graphModel.getNodes().at(i).getCoordinates().at(1);
                int radius = this->_graphModel.getNodes().at(i).getCoverage() * this->getAccuracy();
                switch (drawCircle){
                    case SimpleCircle:
                        this->drawCircleSimple(matrix, x, y, radius);
                        break;
                    case MidpointCircle:
                        this->drawCircle(matrix, x, y, radius);
                        break;
                    default:
                        break;
                }
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

        float square = 0;
        int k = 0;

        switch(type){
            case Realization:
                square = count_black;
                switch(drawCircle){
                    case SimpleCircle:
                        k = this->getMaxCoverageMatrixSimple();
                        break;
                    case MidpointCircle:
                        k = this->getMaxCoverageMatrixMidpoint();
                        break;
                    default:
                        k=1;
                        break;
                }
                square /= k;
                break;
            case MaxCoverageAgainstAll:
                square = count_black;
                square /= count_all;
                break;
            case MaxCoverage:
                square = count_black;
                break;
            default:
                break;
        }

        return square;
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

    // Simple draw circle, less accurate not suggested for usage
    void drawCircleSimple(bool **matrix, int x0, int y0, int r){
        for(int i = x0 - r; i <= x0 + r; i++)
        {
            for(int j = y0 - r; j <= y0 + r; j++)
            {
                if(((i-x0)*(i-x0) + (j-y0)*(j-y0)) <= r*r)
                {
                    if (((i >= 0) && (i < this->_oImgSizeX)) && ((j >= 0) && (j < this->_oImgSizeY)))
                        matrix[i][j] = true;
                }
            }
        }
    }

    graph_t genNewGraph(vector<float> visited){
        Graph graphModel = this->getGraphModel();
        std::vector<Edge> edgeVec;
        unsigned long verticesNum = 0;
        LOG_DEBUG << "Adding edges to graph - END";
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
        LOG_DEBUG << "Adding edges to graph - END";

        LOG_DEBUG << "Initializing graph with edges - START";
        // GraphInit
        graph_t g(edgeVec.begin(), edgeVec.end(), verticesNum);
        LOG_DEBUG << "Initializing graph with edges - END";

        return g;
    }

    float countSquare(vector<float> visited){
        graph_t g = genNewGraph(visited);
        this->graphToImg(std::to_string(this->fileItr),g);
        return this->readImg();
    }

    //FOR PARALLEL METHODS
    float countSquareParallel(vector<float> visited, unsigned long fileItr){
        graph_t g = genNewGraph(visited);
        #pragma omp critical
        {
            this->graphToImg(std::to_string(fileItr), g);
        }
        return this->readImgParallel(fileItr);
    }

    void boost_bfs(){
        graph_t g = this->getUndirectedGraph();
        custom_bfs_visitor vis;
        breadth_first_search(g, vertex(this->getGraphModel().getStockId(), g), visitor(vis));
    }

    void boost_dfs() {
        graph_t g = this->getUndirectedGraph();
        auto indexmap = boost::get(boost::vertex_index, g);
        auto colormap = boost::make_vector_property_map<boost::default_color_type>(indexmap);

        custom_dfs_visitor vis;
        depth_first_search(g, visitor(vis));
    }

    // Output image properties
    unsigned int _accuracy;
    unsigned int _oImgSizeX;
    unsigned int _oImgSizeY;
    unsigned int _oImgCoordScale;
    string  _oImgFormat;
    // Boost graph entities
    std::vector<Edge>  _edgeVector;
    graph_t _graph_t;
    unsigned long _maxCoverage;
    unsigned long _maxCoverageMatrixSimple;
    unsigned long _maxCoverageMatrixMidpoint;
    // Graph inited model
    Graph   _graphModel;
    // Img
    unsigned long fileItr;
    // Visited nodes
    vector<float> _visited;
};
#endif