//
// Created by Александр Кучеров on 12/03/2018.
//

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/breadth_first_search.hpp"
#include <boost/graph/undirected_dfs.hpp>
#include "boost/graph/graphviz.hpp"

#include "../../Models/Graph.h"

class custom_bfs_visitor : public boost::default_bfs_visitor
{
public:

    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph & g) const
    {
        LOG_DEBUG << "Visited vertex - " << u;
    }
};

using namespace boost;

typedef std::pair<int, int> Edge;
typedef adjacency_list<vecS, vecS, undirectedS, no_property,
        property<edge_color_t, default_color_type> > graph_t;
typedef graph_traits<graph_t>::vertex_descriptor vertex_t;

struct detect_loops : public boost::dfs_visitor<>
{
    template <class Edge, class Graph>
    void back_edge(Edge e, const Graph& g) {
        LOG_DEBUG << "Edge - " << e;
    }
};

class AccurateMethod {
public:
    AccurateMethod(unsigned int accuracy)
            : _accuracy(accuracy),
              _graphModel(Graph::initWithFile()){
        LOG_INFO << "Accurate Method - Initialized";
        this->init();
    }

    void init(){
        this->graphInit(this->getGraphModel());
        this->graphToImg();

        LOG_INFO << "Boost BFS - START";
        this->boost_bfs();
        LOG_INFO << "Boost BFS - END";

        LOG_INFO << "Boost DFS - START";
        this->boost_dfs();
        LOG_INFO << "Boost DFS - END";
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
        detect_loops vis;
        undirected_dfs(g, root_vertex(vertex_t(this->getGraphModel().getStockId())).visitor(vis)
                .edge_color_map(get(edge_color, g)));
    }

    void graphToImg(){
    }

    unsigned int _accuracy;
    std::vector<Edge> _edgeVector;
    graph_t _graph_t;
    Graph _graphModel;
};
