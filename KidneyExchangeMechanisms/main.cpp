//
//  main.cpp
//  KidneyExchangeMechanisms
//
//  Created by Sameer Desai on 30/07/18.
//  Copyright © 2018 Sameer Desai. All rights reserved.
//

#include <ilcplex/ilocplex.h>
#include "AllPathsOfLengthK.hpp"  /* Library to enumerate all paths of given length */
#include <ilconcert/iloenv.h>
ILOSTLBEGIN

typedef IloArray<IloIntArray> IntMatrix; /* 2D Matrix to store Weight-Matrix*/
typedef IloArray<IloNumVarArray> NumVarMatrix; /* 2D Matrix of Decision Variables */


int main(int argc, const char * argv[])
{
    IloEnv env;
    try
    {
        Paths p;                    /* Instantiate the Paths class */
        int pLength = 3;            /* Set path length*/
        
        IloInt nodes;
        //IloTimer(env);
        
        
        /*Read in the Adjacency Matrix from File*/
        string filename;
        filename = "/Users/sameerdesai/Documents/KidneyExchangeMechanisms/KidneyExchangeMechanisms/kidneyexample.dat";
        ifstream f(filename,ios::in);
        if(!f)
        {
            cerr<<"File doesnot exist by the name of:"<<filename<<endl;
            throw(1);
        }
    
        f>> nodes;
        
        /* Read in the Weight/Adjacency Matrix from File*/
        IntMatrix weights(env,nodes);
        for(IloInt i = 0; i < nodes; i++)
        {
            weights[i] = IloIntArray(env,nodes);
        }
        
        vector<vector<int>> Adj(nodes,vector<int>(nodes));
        for(int i = 0; i < nodes; i++)
        {
            for(int j = 0; j < nodes; j++)
            {
                f>>Adj[i][j];
                weights[i][j] = Adj[i][j];
            }
        }
        
        vector<vector<int>> AllPaths = p.getPaths(pLength+1,Adj,(int)nodes); /* Get all paths of given length from library subroutine*/
        
        /*Accumulate the edge data*/
        vector<vector<pair<int,int>>> edges(AllPaths.size());
        for(int i = 0; i < AllPaths.size(); i++)
        {
            for(int j = 1; j < AllPaths[i].size(); j++)
            {
                edges[i].push_back(make_pair(AllPaths[i][j-1],AllPaths[i][j]));
            }
        }
      
        IloModel model(env);
        
        /* Decision variables for each of the edges in the graph.*/
        NumVarMatrix s(env);
        
        /*Expression Arrays to store the sum of all edges into and out of that vertex in question.*/
        for(IloInt i = 0; i < nodes; i++)
        {
            s.add(IloNumVarArray(env,nodes,0,1,ILOFLOAT)); // ILOBOOL for 0/1 values
        }
        
        /* Naming all the decision variables */
        for (IloInt i=0; i<nodes; i++)
        {
            for (IloInt j=0; j<nodes; j++)
            {
                string idx = to_string(i);
                string idy = to_string(j);
                
                string numericID = "s_" + idx + "_" + idy;
                unsigned long length = numericID.size();
                char name[length+1];
                strcpy(name, numericID.c_str());
                s[i][j].setName(name);
            }
            
        }
        
        /* For all vertices   v in V , where e_out = (v_i,v_j) and e_in = (v_j,v_i) */
        /* Adding Constraint 1: Sigma(e_out) - Sigma(e_in) = 0       (aka) Conservation Constraint.*/
        /* Adding Constraint 2: Sigma(e_out) <= 1                    (aka) Capacity Constraint  */
        for(int i = 0; i < nodes; i++)
        {
            IloNumVarArray InSum(env);
            IloNumVarArray OutSum(env);
            for(IloInt j = 0; j < nodes; j++)
            {
                if(i == j)
                {
                    continue;
                }
                else
                {
                    
                    if(weights[j][i] > 0)
                    {
                            InSum.add(s[j][i]);
                    }
                    if(weights[i][j] > 0)
                    {
                            OutSum.add(s[i][j]);
                    }
                }
            }
            
            /*1st constraint added here*/
            IloConstraint conservationConstraint(IloSum(OutSum) - IloSum(InSum) == 0);
            string idx = to_string(i);
            string conservationID = "Conservation_" + idx;
            unsigned long length1 = conservationID.size();
            char conservation_constraint[length1+1];
            strcpy(conservation_constraint, conservationID.c_str());
            conservationConstraint.setName(conservation_constraint);
            model.add(conservationConstraint);
            
            /*2nd constraint added here*/
            IloConstraint capacityConstraint(IloSum(OutSum)<=1);
            string idy = to_string(i);
            string capacityID = "Capacity_" + idy;
            unsigned long length2 = capacityID.size();
            char capacity_constraint[length2+1];
            strcpy(capacity_constraint, capacityID.c_str());
            capacityConstraint.setName(capacity_constraint);
            model.add(capacityConstraint);
        }
    
        
        /* Adding the path constraint to restrict the paths to given length */
        /* Adding Constraint 3: e_p1 + e_p2 + .... + e_pL <= L-1       (aka)  Path Length Constraint*/
        for(IloInt i = 0; i < edges.size(); i++)
        {
            IloNumVarArray PathSum(env);
            for(IloInt j = 0; j < edges[i].size(); j++)
            {
                int idx = edges[i][j].first;
                int idy = edges[i][j].second;
                PathSum.add(s[idx][idy]);
            }
            IloConstraint pathLengthConstraint(IloSum(PathSum) <= pLength-1);
            string idz = to_string(i);
            string pathID = "Path_Originating_From_" + idz;
            unsigned long length3 = pathID.size();
            char path_len_constraint[length3+1];
            strcpy(path_len_constraint, pathID.c_str());
            pathLengthConstraint.setName(path_len_constraint);
            model.add(pathLengthConstraint);
            PathSum.end();
        }
        
        IloExpr obj(env);
        for(IloInt i = 0; i < nodes; i++)
        {
            for(IloInt j = 0; j < nodes; j++)
            {
                obj+=weights[i][j] * s[i][j];
            }
        }
        IloObjective MaxCovers = IloMaximize(env, obj);
        MaxCovers.setName("Max_Cycle_Covers_Objective");
        model.add(MaxCovers);
        obj.end();
        
        IloCplex cplex(env);
        
        
        cplex.setName("KidneyExchangeByEdgeCovers");
        cplex.extract(model);
        cplex.exportModel ("/Users/sameerdesai/Documents/KidneyExchangeMechanisms/KidneyExchangeMechanisms/example.lp");
        IloTimer clock(env);
        clock.start();
        if (cplex.solve()) {
            cout << "Solution status: " << cplex.getStatus() << endl;
            cout << "Number of Exchanges(Optimal Value) = " << cplex.getObjValue() << endl;
            cout << "Solve Time    = "<<clock.getTime()<<" seconds"<<endl;
            clock.stop();
            for(IloInt i = 0; i < nodes; i++)
            {
                for(IloInt j = 0; j < nodes; j++)
                {
                    if(i == j)
                    {
                        cout<<0<<" ";
                    }
                    else if(weights[i][j]>0)
                    {
                        cout<<cplex.getValue(s[i][j])<<" ";
                    }
                    else{
                        cout<<0<<" ";
                    }
                }
                cout<<endl;
            }
            
        }
        
        
        
        
    }
    catch(IloException &e)
    {
        cerr<<"Error:"<<e<<endl;
    }
    catch(...)
    {
        cerr<<"Encountered Error"<<endl;
    }
    env.end();
    return 0;
}

