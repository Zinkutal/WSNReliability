//
// Created by Александр Кучеров on 10/03/2018.
//

#ifndef WSNRELIABILITY_GRAPH_H
#define WSNRELIABILITY_GRAPH_H


#include <cstdint>
#include <string>
#include "Node.h"
#include "../utils/filesUtil.cpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

using namespace std;

class Graph {
public:
    // Init
    Graph(string name, string author, string date,
         Node** nodes)
            : _name(name)
            , _author(author)
            , _date(date)
            , _nodes(nodes) {
        LOG_INFO << "Graph Initialized";
    }

    // Init with JSON file
    static Graph initWithFile(const char * file_path = "../src/data/input/json/graph_input.json") {
        LOG_INFO << "JSON reading - START";
        string jsonGraph = getFileContents(file_path);

        rapidjson::Document doc2;
        doc2.Parse(jsonGraph.c_str());
        Graph decodedUser = Graph::fromJSON(doc2);
        LOG_INFO << "JSON reading - END";
        return decodedUser;
    }

    // Getters & Setters
    string getName(){
        return this->_name;
    }

    void setName(string name){
        this->_name = name;
    }

    string getAuthor(){
        return this->_author;
    }

    void setAuthor(string author){
        this->_author = author;
    }

    string getDate(){
        return this->_date;
    }

    void setDate(string date){
        this->_date = date;
    }

    Node** getNodes(){
        return this->_nodes;
    }

    void setNodes(Node *&nodes){
        this->_nodes = &nodes;
    }

    rapidjson::Document toJSON() {
        rapidjson::Value json_val;
        rapidjson::Document doc;
        auto& allocator = doc.GetAllocator();

        doc.SetObject();

        json_val.SetString(_name.c_str(), allocator);
        doc.AddMember("name", json_val, allocator);

        json_val.SetString(_author.c_str(), allocator);
        doc.AddMember("author", json_val, allocator);

        json_val.SetString(_date.c_str(), allocator);
        doc.AddMember("date", json_val, allocator);

        //TODO Add serializing of nodes

        /*for (int i=0; i < sizeof(**_nodes); i++){
            // see http://rapidjson.org/md_doc_tutorial.html#DeepCopyValue
            json_val.CopyFrom(_nodes[i]->toJSON(), allocator);
            doc.AddMember("nodes", json_val, allocator);
        }*/

        return doc;
    }

    static Graph fromJSON(const rapidjson::Value& doc) {
        LOG_INFO << "Parsing Graph from JSON - START";
        if(!doc.IsObject())
            throw runtime_error("document should be an object");

        static const char* members[] = { "name", "author", "date",
                                         "nodes" };
        for(size_t i = 0; i < sizeof(members)/sizeof(members[0]); i++)
            if(!doc.HasMember(members[i])){
                LOG_ERROR << "JSON has missing fields";
                throw runtime_error("JSON has missing fields");
            }

        // Name
        string name = doc["name"].GetString();
        LOG_DEBUG << "Name = " << name;

        // Author
        string author = doc["author"].GetString();
        LOG_DEBUG << "Author = " << author;

        // Date
        string date = doc["date"].GetString();
        LOG_DEBUG << "Date = " << date;

        // Nodes
        const rapidjson::Value& arrJsonNodes = doc["nodes"];
        assert(arrJsonNodes.IsArray());
        Node* nodesArr[arrJsonNodes.Size()];
        for (rapidjson::SizeType i = 0; i < arrJsonNodes.Size(); i++){
            *(nodesArr + i) = new Node (Node::fromJSON(arrJsonNodes[i].GetObject()));
        }
        LOG_DEBUG << "Nodes Amount = " << arrJsonNodes.Size();

        LOG_INFO << "Parsing Graph from JSON - END";

        Graph result(name, author, date, nodesArr);
        return result;
    }

    // Serialize and write JSON to file
    void serializeJSON() {
        LOG_INFO << "Serializing Graph to JSON - START";
        rapidjson::Document doc = this->toJSON();
        rapidjson::StringBuffer buffer;
        //rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        LOG_INFO << "Serializing Graph to JSON - END";

        LOG_INFO << "Writing JSON to FILE - START";
        char* str;
        str = new char[buffer.GetSize()];
        strcpy(str, buffer.GetString());

        writeFileContents("../src/data/output/json/graph_output.json", str);
        LOG_INFO << "Writing JSON to FILE - END";
    }

private:
    string _name;
    string _author;
    string _date;
    Node** _nodes;
};


#endif //WSNRELIABILITY_GRAPH_H
