//
//  main.cpp
//  KidneyExchangeMechanisms
//
//  Created by Sameer Desai on 30/07/18.
//  Copyright © 2018 Sameer Desai. All rights reserved.
//  

#include <ilcplex/ilocplex.h>
#include "AllPathsOfLengthK.hpp"  /* Library to enumerate all paths of given length */
#include "jsonReader.hpp"  /* Library to read and manipulate JSON data */
#include <ilconcert/iloenv.h>
ILOSTLBEGIN

typedef IloArray<IloIntArray> IntMatrix; /* 2D Matrix to store Weight-Matrix*/
typedef IloArray<IloNumVarArray> NumVarMatrix; /* 2D Matrix of Decision Variables */

class LpSolver
{
private:
    string json_filename;
    string dat_filename;
    string default_filename_json;
    string default_filename_dat;
    
public:
    unsigned int nodes;
    unsigned int pLength;
    vector<vector<unsigned int>> Adj_Matrix;
    vector<vector<unsigned int>> Res_Matrix;
    vector<vector<unsigned int>> AllPaths;
    vector<pair<unsigned int, unsigned int>> MatchedPairs;
    Paths p;
    jsonData *jd;
    vector<vector<pair<int,int>>> edges;
    LpSolver()
    {
        this->default_filename_dat = "/Users/sameerdesai/Documents/KidneyExchangeMechanisms/KidneyExchangeMechanisms/kidneyexample.dat";
        this->default_filename_json = "/Users/sameerdesai/Documents/KE2.json";
        this->pLength = 3;
    }
    void setJsonFilename(string filename)
    {
        json_filename = filename;
        jd = new jsonData(json_filename);
    }
    
    void setDatFilename(string filename)
    {
        dat_filename = filename;
    }
    
    void setPathLength(unsigned int len)
    {
        pLength = len;
    }
    
    void readJsonFile()
    {
        jd->mapAllPairs();
        jd->buildCompatibilityGraph();
        nodes = (int)jd->Compatibility_Graph.size();
        Adj_Matrix.resize(nodes,vector<unsigned int>(nodes,0));
        Res_Matrix.resize(nodes,vector<unsigned int>(nodes,0));
        Adj_Matrix = jd->Compatibility_Graph;
    }
    /* Read in the Weight/Adjacency Matrix*/
    void readDatFile()
    {
        ifstream f(dat_filename,ios::in);
        if(!f)
        {
            cerr<<"File doesnot exist by the name of:"<<dat_filename<<endl;
            throw(1);
        }
        f>> nodes;
        Adj_Matrix.resize(nodes,vector<unsigned int>(nodes,0));
        Res_Matrix.resize(nodes,vector<unsigned int>(nodes,0));
        for(unsigned int i = 0; i < nodes; i++)
        {
            for(unsigned int j = 0; j < nodes; j++)
            {
                f>>Adj_Matrix[i][j];
            }
        }
        
    }
    /* Get all paths of given length from library subroutine*/
    void populateAllPaths()
    {
        AllPaths = p.getPaths(pLength+1,Adj_Matrix,nodes);
    }
    
    /*Accumulate the edge data*/
    void populateEdgeData()
    {
        edges.resize(AllPaths.size());
        for(unsigned int i = 0; i < AllPaths.size(); i++)
        {
            for(unsigned int j = 1; j < AllPaths[i].size(); j++)
            {
                edges[i].push_back(make_pair(AllPaths[i][j-1],AllPaths[i][j]));
            }
        }
    }
    char* createConstraintName(string constraintText, unsigned int id )
    {
        string idx = to_string(id);
        string constraintID = constraintText + idx;
        unsigned long length = constraintID.size();
        char *constraintName = new char[length+1];
        strcpy(constraintName, constraintID.c_str());
        return constraintName;
    }
    char* createDVName(unsigned int id1, unsigned int id2)
    {
        string idx = to_string(id1);
        string idy = to_string(id2);
        string numericID = "s_" + idx + "_" + idy;
        unsigned long length = numericID.size();
        char *dvName = new char[length+1];
        strcpy(dvName, numericID.c_str());
        return dvName;
    }
    void printMatrix(vector<vector<unsigned int>>& Mat)
    {
        for(unsigned int i = 0; i <Mat.size(); i++)
        {
            for(unsigned int j = 0; j < Mat[i].size(); j++)
            {
                cout<<Mat[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    
    void setMatchedPairs()
    {
        for(unsigned int i = 0; i < Res_Matrix.size(); i++)
        {
            for(unsigned int j = 0; j < Res_Matrix.size(); j++)
            {
                if(Res_Matrix[i][j] == 1)
                {
                    int rID = jd->getID(i);
                    int dID = jd->getID(j);
                    MatchedPairs.push_back(make_pair(dID,rID));
                }
            }
        }
    }
    
    void printMatches()
    {
        for(unsigned int i = 0; i < MatchedPairs.size(); i++)
        {
            cout<<"Patient "<<MatchedPairs[i].second<<" gets Kidney from "<<MatchedPairs[i].first<<endl;
        }
        jd->printToJson(MatchedPairs);
    }
    
    
    
    
};

int main(int argc, const char * argv[])
{
    IloEnv env;
    try
    {
        LpSolver solver;
        char filechoice;
        string pathName;
        unsigned int len;
        cout<<"Welcome to Kidney Exchange Solver"<<endl;
        cout<<"Choose File Type: (a)JSON File, (b)DAT File"<<endl;
        cin>>filechoice;
        switch(filechoice)
        {
            case 'a':
                cout<<"Enter the pathname for your JSON file:"<<endl;
                cin>>pathName;
                solver.setJsonFilename(pathName);
                solver.readJsonFile();
                break;
                
                
            case 'b':
                cout<<"Enter the pathname for your DAT file:"<<endl;
                cin>>pathName;
                solver.setDatFilename(pathName);
                solver.readDatFile();
                break;
                
            default :
                cout<<"Going ahead with default files"<<endl;
                if(filechoice == 'a')
                    solver.readJsonFile();
                else
                    solver.readDatFile();
                
        }
        cout<<"Enter the value of L for path length constraint:";
        cin>>len;
        solver.setPathLength(len);
        
    
        /* Read in the Weight/Adjacency Matrix*/
        IntMatrix weights(env,solver.nodes);
        solver.printMatrix(solver.Adj_Matrix);
        for(unsigned int i = 0; i < solver.nodes; i++)
        {
            weights[i] = IloIntArray(env,solver.nodes);
        }
        
        for(unsigned int i = 0; i < solver.nodes; i++)
        {
            for(unsigned int j = 0; j < solver.nodes; j++)
            {
                weights[i][j] = solver.Adj_Matrix[i][j];
            }
        }
        

    
        IloModel model(env);
        
        /* Decision variables for each of the edges in the graph.*/
        NumVarMatrix s(env);
        
        /*Expression Arrays to store the sum of all edges into and out of that vertex in question.*/
        for(unsigned int i = 0; i < solver.nodes; i++)
        {
            s.add(IloNumVarArray(env,solver.nodes,0,1,ILOFLOAT)); // ILOBOOL for 0/1 values
        }
        
        /* Naming all the decision variables */
        for (unsigned int i=0; i<solver.nodes; i++)
        {
            for (unsigned int j=0; j<solver.nodes; j++)
            {
                char* name = solver.createDVName(i,j);
                s[i][j].setName(name);
            }
            
        }
        
        /* For all vertices   v in V , where e_out = (v_i,v_j) and e_in = (v_j,v_i) */
        /* Adding Constraint 1: Sigma(e_out) - Sigma(e_in) = 0       (aka) Conservation Constraint.*/
        /* Adding Constraint 2: Sigma(e_out) <= 1                    (aka) Capacity Constraint  */
        for(unsigned int i = 0; i < solver.nodes; i++)
        {
            IloNumVarArray InSum(env);
            IloNumVarArray OutSum(env);
            for(unsigned int j = 0; j < solver.nodes; j++)
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
            char* constraintName1 = solver.createConstraintName("Conservation_",i);
            conservationConstraint.setName(constraintName1);
            model.add(conservationConstraint);
            
            /*2nd constraint added here*/
            IloConstraint capacityConstraint(IloSum(OutSum)<=1);
            char* constraintName2 = solver.createConstraintName("Capacity_",i);
            capacityConstraint.setName(constraintName2);
            model.add(capacityConstraint);
        }
    
        
        /* Adding the path constraint to restrict the paths to given length */
        /* Adding Constraint 3: e_p1 + e_p2 + .... + e_pL <= L-1       (aka)  Path Length Constraint*/
        for(unsigned int i = 0; i < solver.edges.size(); i++)
        {
            IloNumVarArray PathSum(env);
            for(unsigned int j = 0; j < solver.edges[i].size(); j++)
            {
                unsigned int idx = solver.edges[i][j].first;
                unsigned int idy = solver.edges[i][j].second;
                PathSum.add(s[idx][idy]);
            }
            IloConstraint pathLengthConstraint(IloSum(PathSum) <= (IloInt)solver.pLength-1);
            char* constraintName3 = solver.createConstraintName("Path_Originating_From_",i);
            pathLengthConstraint.setName(constraintName3);
            model.add(pathLengthConstraint);
            PathSum.end();
        }
        
        IloExpr obj(env);
        for(IloInt i = 0; i < solver.nodes; i++)
        {
            for(IloInt j = 0; j < solver.nodes; j++)
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
        cplex.exportModel ("/Users/sameerdesai/Documents/KidneyExchangeMechanisms/KidneyExchangeMechanisms/KE.lp");
        IloTimer clock(env);
        clock.start();
        if (cplex.solve()) {
            cout << "Solution status: " << cplex.getStatus() << endl;
            cout << "Number of Exchanges(Optimal Value) = " << cplex.getObjValue() << endl;
            cout << "Solve Time    = "<<clock.getTime()<<" seconds"<<endl;
            clock.stop();
            for(IloInt i = 0; i < solver.nodes; i++)
            {
                for(IloInt j = 0; j < solver.nodes; j++)
                {
                    if(i == j)
                    {
                        continue;
                    }
                    else if(weights[i][j]>0)
                    {
                        solver.Res_Matrix[i][j] = cplex.getValue(s[i][j]);
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            
        }
        solver.printMatrix(solver.Res_Matrix);
        solver.setMatchedPairs();
        solver.printMatches();
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

