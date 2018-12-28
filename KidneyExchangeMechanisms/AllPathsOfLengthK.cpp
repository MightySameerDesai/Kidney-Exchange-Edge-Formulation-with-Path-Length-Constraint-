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

/* DFS and Backtracking based approach to fetch all possible paths of given length */
void Paths::getPathsHelper(vector<vector<unsigned int>>&AllPaths, vector<vector<unsigned int>>&Adj, unsigned int pLength, vector<bool>&seen, vector<unsigned int>&available,vector<unsigned int>&curPath,unsigned int Nodes)
{
    vector<unsigned int> neighbors(available);   // Keeps track of available neighbours for each node
    for(unsigned int i = 0; i < neighbors.size(); i++)
    {
        unsigned int curr = neighbors[i];
        curPath.push_back(curr);
        seen[curr] = true;
        if(curPath.size() == pLength)  // If we have a path of pLength, save it to AllPaths
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
            //Recur on remaining available nodes
            getPathsHelper(AllPaths, Adj, pLength, seen, available, curPath, Nodes);
            
        }
        curPath.pop_back();
        seen[curr] = false;
        
    }
}

/* Prints the paths populated in the previous method */
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

/* Wrapper function that returns all paths of given length */
vector<vector<unsigned int>> Paths::getPaths(unsigned int pLength, vector<vector<unsigned int>>&Adj, unsigned int Nodes)
{
    vector<vector<unsigned int>> AllP; // To store paths of given length.
    vector<bool> seen(Nodes,false);    // To make sure we don't visit seen nodes.
    vector<unsigned int> available;    // To keep track of which nodes are available.
    vector<unsigned int> curPath;      // To keep track of current path generated.
    // For every node in the graph, do the following:
    for(unsigned int i = 0; i < Nodes; i++)
    {
        // Initialise available nodes from the vertex
        for(unsigned int j = 0; j < Nodes; j++)
        {
            if(Adj[i][j]>0)
                available.push_back(j);
        }
        // Add the node to path and mark it as seen.
        curPath.push_back(i);
        seen[i] = true;
        // Call the helper function to recursively traverse and fetch paths of given length.
        getPathsHelper(AllP,Adj,pLength,seen,available,curPath,Nodes);
        // Reset the path and mark the node as unvisited and also reset the available nodes vector.
        curPath.pop_back();
        seen[i] = false;
        available.clear();
        
    }
    //printPaths(AllP, Nodes);
    return AllP;
}

