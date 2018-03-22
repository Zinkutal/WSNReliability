//
// Created by Александр Кучеров on 22/03/2018.
//

#define INPUT_FILE "../src/data/input/json/graph_input_4.json"

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

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

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
              _graphModel(Graph::initWithFile(INPUT_FILE)){
        LOG_INFO << "Method - Initialized";
        this->init();
    }

    void init(){
        this->graphInit(this->getGraphModel());

        LOG_INFO << "Boost BFS - START";
        this->boost_bfs();
        LOG_INFO << "Boost BFS - END";

        LOG_INFO << "Boost DFS - START";
        this->boost_dfs();
        LOG_INFO << "Boost DFS - END";


        this->fileItr = 0;

        this->graphToImg("_max_coverage",this->_graph_t);
        this->setMaxCoverage(this->maxCoverageReadImg("_max_coverage"));
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
protected:
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

        LOG_DEBUG << "Square - " << count_black/count_all;

        return (count_black/this->getMaxCoverage());
    }

    unsigned long maxCoverageReadImg(string filename){
        string imgPath = "output/graph" + filename + "." + this->_oImgFormat;

        cv::Mat image;
        image = cv::imread(imgPath, CV_LOAD_IMAGE_COLOR);

        if(! image.data) LOG_ERROR << "Could not open or find the image - " << imgPath;

        // Prepare Image
        cv::cvtColor(image, image, CV_BGR2GRAY);
        cv::threshold(image, image, 254, 255, cv::THRESH_BINARY );
        // Count Pixels
        int count_all   = image.cols * image.rows;
        int count_white = cv::countNonZero(image);
        int count_black = count_all - count_white;

        LOG_DEBUG << "All pixels - " << count_all << "; White pixels - " << count_white
                  << "; Black pixels - " << count_black;

        return count_black;
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
    // Graph inited model
    Graph   _graphModel;
    // Img
    int fileItr;
};
