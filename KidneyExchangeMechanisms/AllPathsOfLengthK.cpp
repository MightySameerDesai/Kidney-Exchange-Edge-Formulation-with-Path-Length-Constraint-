//
//  AllPathsOfLengthK.cpp
//  KidneyExchangeMechanisms
//
//  Created by Sameer Desai on 06/08/18.
//  Copyright © 2018 Sameer Desai. All rights reserved.
//

#include "AllPathsOfLengthK.hpp"
#include <iostream>
using namespace std;

void Paths::getPathsHelper(vector<vector<unsigned int>>&AllPaths, vector<vector<unsigned int>>&Adj, unsigned int pLength, vector<bool>&seen, vector<unsigned int>&available,vector<unsigned int>&curPath,unsigned int Nodes)
{
    vector<unsigned int> neighbors(available);
    for(unsigned int i = 0; i < neighbors.size(); i++)
    {
        unsigned int curr = neighbors[i];
        curPath.push_back(curr);
        seen[curr] = true;
        if(curPath.size() == pLength)
        {
            AllPaths.push_back(curPath);
        }
        else
        {
            available.clear();
            // Initialize available vector here to nodes still unseen and are adjacent to ith nodes.
            for(unsigned int j = 0; j < Nodes; j++)
            {
                if(!seen[j] && Adj[curr][j]!=0)
                {
                    available.push_back(j);
                }
            }
            getPathsHelper(AllPaths, Adj, pLength, seen, available, curPath, Nodes);
            
        }
        curPath.pop_back();
        seen[curr] = false;
        
    }
}

void Paths::printPaths(vector<vector< unsigned int>>&AllPaths,unsigned int Nodes)
{
    for(unsigned int i = 0; i < AllPaths.size(); i++)
    {
        cout<<"[ ";
        for(unsigned int j = 0; j < AllPaths[i].size(); j++)
        {
            cout<<AllPaths[i][j]<<" ";
        }
        cout<<"]"<<endl;
    }
}

vector<vector<unsigned int>> Paths::getPaths(unsigned int pLength, vector<vector<unsigned int>>&Adj, unsigned int Nodes)
{
    vector<vector<unsigned int>> AllPaths;
    vector<bool> seen(Nodes,false);
    vector<unsigned int> available;
    vector<unsigned int> curPath;
    for(unsigned int i = 0; i < Nodes; i++)
    {
        for(unsigned int j = 0; j < Nodes; j++)
        {
            if(Adj[i][j]>0)
                available.push_back(j);
        }
        curPath.push_back(i);
        seen[i] = true;
        getPathsHelper(AllPaths,Adj,pLength,seen,available,curPath,Nodes);
        curPath.pop_back();
        seen[i] = false;
        available.clear();
        
    }
    //printPaths(AllPaths, Nodes);
    return AllPaths;
}

