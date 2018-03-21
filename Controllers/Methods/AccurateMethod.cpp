//
// Created by Александр Кучеров on 12/03/2018.
//

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <string>
#include <math.h>

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/breadth_first_search.hpp"
#include "boost/graph/depth_first_search.hpp"
#include "boost/graph/graphviz.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../../Models/Graph.h"
#define INPUT_FILE "../src/data/input/json/graph_input_4.json"

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
    custom_vertex_writer( graph_t& g , Graph& gModel, unsigned int scale) : currentGraph( g ), currentGraphModel(gModel), imgCoordScale(scale) {}

    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& e) const {
        float coverage = this->currentGraphModel.getNodes().at(e).getCoverage();
        float posX = this->currentGraphModel.getNodes().at(e).getCoordinates().at(0);
        float posY = this->currentGraphModel.getNodes().at(e).getCoordinates().at(1);
        out << "[shape=circle,width=" << coverage
            << ",style=filled,fillcolor=\"#000000\",pos=\""
            << posX/imgCoordScale << "," << posY/imgCoordScale << "!\"]";
    }
private:
    graph_t& currentGraph;
    Graph&   currentGraphModel;
    unsigned int imgCoordScale;
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

class AccurateMethod {
public:
    AccurateMethod(unsigned int accuracy, unsigned int oImgSizeX, unsigned int oImgSizeY, unsigned int oImgScale, string oImgFormat)
            : _accuracy(accuracy),
              _oImgSizeX(oImgSizeX),
              _oImgSizeY(oImgSizeY),
              _oImgScale(oImgScale),
              _oImgFormat(oImgFormat),
              _graphModel(Graph::initWithFile(INPUT_FILE)){
        LOG_INFO << "Accurate Method - Initialized";
        this->init();
    }

    // Used by R function
    vector<int> visited;
    int fileItr;

    void init(){
        this->graphInit(this->getGraphModel());

        /*LOG_INFO << "Boost BFS - START";
        this->boost_bfs();
        LOG_INFO << "Boost BFS - END";

        LOG_INFO << "Boost DFS - START";
        this->boost_dfs();
        LOG_INFO << "Boost DFS - END";*/


        this->fileItr = 0;

        // Start from stock
        int stockId = this->getGraphModel().getStockId();
        int stockPr = this->getPr(stockId);
        this->visited.push_back(stockId);

        LOG_INFO << "Custom accurate reliability - START";
        float result = this->R(this->visited, stockId, stockPr);
        LOG_INFO << "Custom accurate reliability - END";
        LOG_INFO << "WSN Network Reliability: " << result;

        vector<float> prVector = this->getGraphProbabilities();
        LOG_INFO << "Custom accurate reliability(fix) - START";
        float result_fix = factorialR(prVector);
        LOG_INFO << "Custom accurate reliability(fix) - END";
        LOG_INFO << "WSN Network Reliability: " << result_fix;
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

private:
    void graphInit(Graph graphModel) {
        LOG_INFO << "Adding edges to graph - START";
        std::vector<Edge> edgeVec;
        for ( Node node : graphModel.getNodes()) {
            for (unsigned int neighborVertex: node.getRelations()){
                edgeVec.push_back(Edge(node.getId(), neighborVertex));
            }
        }
        this->setEdgeVector(edgeVec);
        LOG_INFO << "Adding edges to graph - END";

        LOG_INFO << "Initializing graph with edges - START";
        // GraphInit
        graph_t g(edgeVec.begin(), edgeVec.end(), graphModel.getNodes().size());

        graph_traits<graph_t>::vertex_iterator vi, vi_end, next;
        tie(vi, vi_end) = vertices(g);
        for (next = vi; vi != vi_end; vi = next) {
            ++next;
        }

        this->setUndirectedGraph(g);
        LOG_DEBUG << "Edges Num - " << num_edges(g);
        LOG_INFO << "Initializing graph with edges - END";

        // List all edges
        typedef graph_traits<graph_t>::edge_iterator edge_iterator;
        std::pair<edge_iterator, edge_iterator> ei = edges(g);
        for(edge_iterator edge_iter = ei.first; edge_iter != ei.second; ++edge_iter) {
            LOG_DEBUG << "Edge (" << source(*edge_iter, g) << ", " << target(*edge_iter, g) << ")";
        }
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

    void graphToImg(graph_t g){
        //write_graphviz (std::cout, g);
        std::ofstream dmp;
        string imgPath = "output/graph" + std::to_string(this->fileItr) + "." + this->_oImgFormat;
        string dotPath = "output/graph" + std::to_string(this->fileItr) + ".dot";
        dmp.open(dotPath);

        write_graphviz(dmp, g, custom_vertex_writer(g, this->_graphModel, this->_oImgCoordScale), custom_edge_writer(g));
        string outFormat = "fdp";
        string gFormat = "-T" + this->_oImgFormat;
        string gSize = "-Gsize=" + std::to_string(this->_oImgSizeX) + "," + std::to_string(this->_oImgSizeY) + "!";
        string gDpi = "-Gdpi=" + std::to_string(this->_accuracy);
        string cmd = outFormat +
                     " " + dotPath +
                     " " + gFormat +
                     " " + gSize +
                     " " + gDpi +
                     " -o " + imgPath;
        const char * dotCmd = cmd.c_str();

        std::system(dotCmd);
    }
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

        return (count_black/count_all) * this->_accuracy;
    }

    float countSquare(vector<int> visited){
        // TODO FIX
        Graph graphModel = this->getGraphModel();

        std::vector<Edge> edgeVec;
        for (int visitedId: visited){
            for (unsigned int neighborVertex: graphModel.getNodes().at(visitedId).getRelations()){
                if((std::find(visited.begin(), visited.end(), neighborVertex) != visited.end())){
                    edgeVec.push_back(Edge(graphModel.getNodes().at(visitedId).getId(), neighborVertex));
                }
            }
        }
        this->setEdgeVector(edgeVec);
        LOG_INFO << "Adding edges to graph - END";

        LOG_INFO << "Initializing graph with edges - START";
        // GraphInit
        graph_t g(edgeVec.begin(), edgeVec.end(), graphModel.getNodes().size());
        LOG_INFO << "Initializing graph with edges - END";

        this->graphToImg(g);
        return this->readImg();
    }

    float countSquare(vector<float> visited){
        // TODO FIX
        Graph graphModel = this->getGraphModel();

        std::vector<Edge> edgeVec;
        for (int visitedId: visited){
            for (unsigned int neighborVertex: graphModel.getNodes().at(visitedId).getRelations()){
                if((std::find(visited.begin(), visited.end(), neighborVertex) != visited.end())){
                    edgeVec.push_back(Edge(graphModel.getNodes().at(visitedId).getId(), neighborVertex));
                }
            }
        }
        this->setEdgeVector(edgeVec);
        LOG_INFO << "Adding edges to graph - END";

        LOG_INFO << "Initializing graph with edges - START";
        // GraphInit
        graph_t g(edgeVec.begin(), edgeVec.end(), graphModel.getNodes().size());
        LOG_INFO << "Initializing graph with edges - END";


        //this->graphToImg(this->_graph_t);
        return this->readImg();
    }

    bool hasNeighbor(vector<int> visited, int currV){
        Graph g = this->_graphModel;
        vector <unsigned int> neighbors = g.getNodes().at(currV).getRelations();
        for (int neighbId: neighbors){
            if(!(std::find(visited.begin(), visited.end(), neighbId) != visited.end())){
                return true;
            }
        }
        return false;
    }

    unsigned int getNeighbor(vector<int> visited, int currV){
        Graph g = this->_graphModel;
        vector <unsigned int> neighbors = g.getNodes().at(currV).getRelations();
        for (int neighbId: neighbors){
            if(!(std::find(visited.begin(), visited.end(), neighbId) != visited.end())){
                return (unsigned)neighbId;
            }
        }
        return 0;
    }

    float getPr(int currV){
        Graph g = this->_graphModel;
        return g.getNodes().at(currV).getReliablility();
    }

    vector<float> getGraphProbabilities(){
        Graph g = this->_graphModel;
        vector<float> prVector;
        for ( Node node : g.getNodes()) {
            prVector.push_back(node.getReliablility());
        }
        return prVector;
    }

    float R(vector<int> visited, int currV, float p){
        if (this->hasNeighbor(visited, currV)){
            LOG_DEBUG << "Vertex with id (" << currV << ") has a non-visited neighbor vertex";
            currV = this->getNeighbor(visited, currV);
            if(!(std::find(visited.begin(), visited.end(), currV) != visited.end())){
                visited.push_back(currV);
            }
        } else {
            LOG_DEBUG << "Vertex with id (" << currV << ") doesn't have a neighbor vertex";
            return this->countSquare(visited)*p;
        }
        if (!this->hasNeighbor(visited, currV)) {
            currV = this->_graphModel.getStockId();
            LOG_DEBUG << "Starting from stock vertex with id (" << currV << ")";
        }
        p = this->getPr(currV);
        return R(visited, currV, 1-p) + R(visited, currV, 1);
    }

    float factorialR(vector<float> nodeRel){
        float result = 0;
        unsigned int v = this->_graphModel.getStockId(); // 0

        for (unsigned int i=0; i<= this->_graphModel.getNodes().size(); i++){
            if (nodeRel.at(i) == 1){
                for (unsigned int neighborVertexId: this->_graphModel.getNodes().at(i).getRelations()){
                    if ((this->_graphModel.getNodes().at(neighborVertexId).getReliablility() > 0)
                        && (this->_graphModel.getNodes().at(neighborVertexId).getReliablility() < 1)) {
                        v = neighborVertexId;
                        break;
                    }
                }
            }
            if (v > 0){
                nodeRel.at(v) = 1;
                result = this->_graphModel.getNodes().at(v).getReliablility() * factorialR(nodeRel);
                nodeRel.at(v) = 0;
                result += (1 - this->_graphModel.getNodes().at(v).getReliablility()) * factorialR(nodeRel);
            } else if (v == 0) result = this->countSquare(nodeRel);
        }

        return result;
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
    // Graph inited model
    Graph   _graphModel;
};
