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
    
    vector<vector<int>> getPaths(int pathLen, vector<vector<int>>&AdjMatrix,int Nodes);
    
//    void getPathsHelper(vector<vector<int>>&AllPaths, vector<vector<int>>&Adj, int pLength, vector<bool>&seen, vector<int>&available,vector<int>&curPath,int Nodes);
//
//    void printPaths(vector<vector<int>>&AllPaths,int Nodes);
    
    
    
};




#endif /* AllPathsOfLengthK_hpp */
