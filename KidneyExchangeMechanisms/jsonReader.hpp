//
//  jsonReader.hpp
//  KidneyExchangeMechanisms
//
//  Created by Sameer Desai on 24/08/18.
//  Copyright © 2018 Sameer Desai. All rights reserved.
//

#ifndef jsonReader_hpp
#define jsonReader_hpp

#include <iostream>
#include <fstream>
#include <json/json.h>


using namespace std;

class jsonData
{
    
private:
    
    map<unsigned int,unsigned int> myMap;
    string filename;
    Json::Value Compatibilities;
    
    
public:
    vector<vector<unsigned int>> Compatibility_Graph;
    jsonData(string filename);
    unsigned int getID(unsigned int pid);
    void print2DVector(vector<vector<unsigned int>>&myVector);
    void mapAllPairs();
    void buildCompatibilityGraph();
    void printToJson(vector<pair<unsigned int, unsigned int>>&MatchedPairs);
};


#endif /* jsonReader_hpp */
