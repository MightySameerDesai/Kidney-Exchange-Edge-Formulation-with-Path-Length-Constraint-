//
//  jsonReader.cpp
//  KidneyExchangeMechanisms
//
//  Created by Sameer Desai on 24/08/18.
//  Copyright © 2018 Sameer Desai. All rights reserved.
//

#include "jsonReader.hpp"


jsonData::jsonData(string filename)
{
        this->filename = filename;
        ifstream ifs(filename);
        ifs >> Compatibilities;
        ifs.close();
}
    
unsigned int jsonData::getID(unsigned int pid)
{
        return myMap[pid];
}
    
void jsonData::print2DVector(vector<vector<unsigned int>>&myVector)
{
        for(unsigned int i = 0; i < myVector.size(); i++)
        {
            for(unsigned int j = 0; j < myVector[i].size(); j++)
            {
                cout<<myVector[i][j]<<" ";
            }
            cout<<endl;
        }
}
    
void jsonData::mapAllPairs()
{
        unsigned int id_seed = 0;
        unsigned int n = Compatibilities["data"].size();
        Compatibility_Graph.resize(n);
        for(unsigned int i = 0; i < n; i++)
        {
            Compatibility_Graph[i].resize(n,0);
        }
        /* Map every patient to a UID */
        for (Json::Value::iterator it = Compatibilities["data"].begin(); it != Compatibilities["data"].end(); ++it)
        {
            
            auto itr = (*it)["sources"].begin();
            unsigned int source = (*itr).asInt();
            myMap[source] = id_seed++;
        }
}
    
void jsonData::buildCompatibilityGraph()
{
        /* Populate the Compatibility Graph from JSON data*/
        for (Json::Value::iterator it = Compatibilities["data"].begin(); it != Compatibilities["data"].end(); ++it)
        {
            
            auto it_source = (*it)["sources"].begin();
            unsigned int source = (*it_source).asInt();
            unsigned int sourceID = getID(source);
            
            auto it_recipients = (*it)["matches"].begin();
            for(    ;   it_recipients !=(*it)["matches"].end()  ;   it_recipients++)
            {
                
                unsigned int recipient = (*it_recipients)["recipient"].asInt();
                unsigned int recipientID = getID(recipient);
                Compatibility_Graph[recipientID][sourceID] = 1;
            }
        }
        
}

void jsonData::printToJson(vector<pair<unsigned int, unsigned int>>&MatchedPairs)
{
    Json::Value root;
    for(unsigned int i = 0; i < MatchedPairs.size(); i++)
    {
        string id = to_string(i);
        root[id]["Patient"] = MatchedPairs[i].first;
        root[id]["Donor"] = MatchedPairs[i].second;
    }
    ofstream file;
    file.open("KEResults.json",ios::out);
    file << root;
    file.close();
}
    




