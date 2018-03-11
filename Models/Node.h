//
// Created by Александр Кучеров on 10/03/2018.
//

#ifndef WSNRELIABILITY_NODE_H
#define WSNRELIABILITY_NODE_H

#include <cstdint>
#include <rapidjson/document.h>
#include <stdexcept>
#include <cmath>
#include "../utils/libs/simpleLogger/simpleLogger.h"

class Node {
public:
    // Init
    Node(unsigned int id, float* coordinates, unsigned int* relations, float reliability, float coverage, bool stock)
            : _id(id)
            , _coordinates (coordinates)
            , _relations (relations)
            , _reliability(reliability)
            , _coverage(coverage)
            , _stock(stock) {
        LOG_INFO << "Node Initialized";
    }

    // Getters & Setters
    unsigned int getId(){
        return this->_id;
    }

    void setId(unsigned int id){
        this->_id = id;
    }

    float * getCoordinates(){
        return this->_coordinates;
    }

    void setCoordinates(float &coordinates){
        this->_coordinates = &coordinates;
    }

    unsigned int* getRelations(){
        return this->_relations;
    }

    void setRelations(unsigned int &relations){
        this->_relations = &relations;
    }

    float getReliablility(){
        return this->_reliability;
    }

    void setReliablility(float reliablility){
        this->_reliability = reliablility;
    }

    float getCoverage(){
        return this->_coverage;
    }

    void setCoverage(float coverage){
        this->_coverage = coverage;
    }

    bool getStock(){
        return this->_stock;
    }

    void setStock(bool stock){
        this->_stock = stock;
    }


    rapidjson::Document toJSON() {
        rapidjson::Document doc;
        auto& allocator = doc.GetAllocator();

        //TODO Fix serializing of Node
        doc.PushBack(rapidjson::Value(_id), allocator);//.PushBack(_coordinates, allocator).PushBack(_relations, allocator).PushBack(_reliability, allocator).PushBack(_coverage, allocator).PushBack(_stock, allocator);
        return doc;
    }

    static Node fromJSON(const rapidjson::Value::ConstObject& doc) {
        LOG_INFO << "Parsing node from JSON - START";

        // Id
        const rapidjson::Value::ConstMemberIterator itr_id = doc.FindMember("id");
        unsigned int id = itr_id->value.GetUint();
        LOG_DEBUG << "Id = " << id;
        //printf("Id = %d\n", id);

        // Coordinates array
        const rapidjson::Value::ConstMemberIterator itr_coordinates = doc.FindMember("coordinates");
        assert(itr_coordinates->value.IsArray());
        float *coordinates = NULL;
        coordinates = new float[itr_coordinates->value.Size()];
        for (rapidjson::SizeType i = 0; i < itr_coordinates->value.Size(); i++) // Uses SizeType instead of size_t
        {
            coordinates[i] = itr_coordinates->value[i].GetFloat();
            LOG_DEBUG << "Coordinates[" << i << "] = " << coordinates[i];
            //printf("Coordinates[%d] = %f\n", i, coordinates[i]);
        }

        // Relations array
        const rapidjson::Value::ConstMemberIterator itr_relations = doc.FindMember("relations");
        assert(itr_relations->value.IsArray());
        unsigned int *relations = NULL;
        relations = new unsigned int[itr_relations->value.Size()];
        for (rapidjson::SizeType i = 0; i < itr_relations->value.Size(); i++) // Uses SizeType instead of size_t
        {
            relations[i] = itr_relations->value[i].GetUint();
            LOG_DEBUG << "Relations[" << i << "] = " << relations[i];
            //printf("Relations[%d] = %d\n", i, relations[i]);
        }

        // Reliability
        const rapidjson::Value::ConstMemberIterator itr_reliability = doc.FindMember("reliability");
        float reliability = itr_reliability->value.GetFloat();
        LOG_DEBUG << "Reliability = " << reliability;
        //printf("Reliability = %f\n", reliability);

        // Coverage
        const rapidjson::Value::ConstMemberIterator itr_coverage = doc.FindMember("coverage");
        float coverage = itr_coverage->value.GetFloat();
        LOG_DEBUG << "Coverage = " << coverage;
        //printf("Coverage = %f\n", coverage);

        // Coverage
        const rapidjson::Value::ConstMemberIterator itr_stock = doc.FindMember("stock");
        bool stock = itr_stock->value.GetBool();
        LOG_DEBUG << "Stock = " << stock;
        //printf("Stock = %d\n", stock);

        LOG_INFO << "Parsing Node from JSON - END";

        Node result(id, coordinates, relations, reliability, coverage, stock);
        return result;
    }



private:
    unsigned int    _id;
    float*          _coordinates;
    unsigned int*   _relations;
    float           _reliability;
    float           _coverage;
    bool            _stock;
};


#endif //WSNRELIABILITY_NODE_H
