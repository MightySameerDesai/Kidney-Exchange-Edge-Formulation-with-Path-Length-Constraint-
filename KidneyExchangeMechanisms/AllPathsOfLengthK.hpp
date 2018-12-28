//
//  AllPathsOfLengthK.hpp
//  KidneyExchangeMechanisms
//
//  Created by Sameer Desai on 06/08/18.
//  Copyright © 2018 Sameer Desai. All rights reserved.
//

#ifndef AllPathsOfLengthK_hpp
#define AllPathsOfLengthK_hpp

#include <stdio.h>
#include <vector>
using namespace std;

class Paths
{
    public:
    
    vector<vector<unsigned int>> getPaths(unsigned int pathLen, vector<vector<unsigned int>>&AdjMatrix,unsigned int Nodes);
    
    void printPaths(vector<vector<unsigned int>>&AllPaths,unsigned int Nodes);
    
    void getPathsHelper(vector<vector<unsigned int>>&AllPaths, vector<vector<unsigned int>>&Adj, unsigned int pLength, vector<bool>&seen, vector<unsigned int>&available,vector<unsigned int>&curPath,unsigned int Nodes);
    
    
    
};




#endif /* AllPathsOfLengthK_hpp */
