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

void getPathsHelper(vector<vector<int>>&AllPaths, vector<vector<int>>&Adj, int pLength, vector<bool>&seen, vector<int>&available,vector<int>&curPath,int Nodes)
{
    vector<int> neighbors(available);
    for(int i = 0; i < neighbors.size(); i++)
    {
        int curr = neighbors[i];
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
            for(int j = 0; j < Nodes; j++)
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

void printPaths(vector<vector<int>>&AllPaths,int Nodes)
{
    for(int i = 0; i < AllPaths.size(); i++)
    {
        cout<<"[ ";
        for(int j = 0; j < AllPaths[i].size(); j++)
        {
            cout<<AllPaths[i][j]<<" ";
        }
        cout<<"]"<<endl;
    }
}

vector<vector<int>> Paths::getPaths(int pLength, vector<vector<int>>&Adj, int Nodes)
{
    vector<vector<int>> AllPaths;
    vector<bool> seen(Nodes,false);
    vector<int> available;
    vector<int> curPath;
    for(int i = 0; i < Nodes; i++)
    {
        for(int j = 0; j < Nodes; j++)
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

