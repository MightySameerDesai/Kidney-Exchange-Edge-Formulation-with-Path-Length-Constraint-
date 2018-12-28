//
//  main.cpp
//  KidneyExchangeMechanisms
//
//  Created by Sameer Desai on 30/07/18.
//  Copyright © 2018 Sameer Desai. All rights reserved.
//  

#include <ilcplex/ilocplex.h>     /* Library for CPLEX */
#include "AllPathsOfLengthK.hpp"  /* Library to enumerate all paths of given length */
#include "jsonHandler.hpp"         /* Library to read and manipulate JSON data */
#include <chrono>
using namespace std::chrono;
ILOSTLBEGIN

typedef IloArray<IloIntArray> IntMatrix; /* 2D Matrix to store Weight-Matrix*/
typedef IloArray<IloNumVarArray> NumVarMatrix; /* 2D Matrix of Decision Variables */

class LpSolver
{
private:
    string json_filename; /* JSON filename specified by user */
    string dat_filename;  /* DAT filename specified by user */
    string default_filename_json; /* Default JSON filename */
    string default_filename_dat;  /* Default DAT filename */
    
public:
    unsigned int nodes;   /* Number of P-D Pairs / Vertices in the Compatibility Graph */
    unsigned int pLength; /* Disallow paths of length greater than or equal to */
    vector<vector<unsigned int>> Adj_Matrix; /* Stores the Adj-Matrix of given instance */
    vector<vector<double>> Res_Matrix;       /* Stores the result of LP Solver i.e. which pairs were matched*/
    vector<vector<unsigned int>> AllPaths;   /* Stores all pLength paths */
    //vector<pair<unsigned int, unsigned int>> MatchedPairs;  /* */
    Paths p;                                                /* Object of Paths Class*/
    jsonData *jd;                                           /* Object of JsonData Class*/
    vector<vector<pair<unsigned int,unsigned int>>> edges;  /* Stores all edges in the given instance*/
    vector<vector<unsigned int>>cycles; /* Stores all disjoint cycles/exchanges once LP solver solves */
    map<unsigned int, unsigned int> myPairs;   /* Map to store the matched pairs as in solution */
    int pairsIncluded = 0;
    high_resolution_clock::time_point start;
    /* Initializes the default parameters */
    LpSolver()
    {
        this->default_filename_dat = "/Users/sameerdesai/Documents/KidneyExchangeMechanisms/KidneyExchangeMechanisms/kidneyexample.dat";
        this->default_filename_json = "/Users/sameerdesai/Documents/KE2.json";
        this->pLength = 3;
    }
    
    /* Sets the JSON filename */
    void setJsonFilename(string filename)
    {
        json_filename = filename;
        jd = new jsonData(json_filename);
    }
    
    /* Sets the DAT filename */
    void setDatFilename(string filename)
    {
        dat_filename = filename;
    }
    
    /* Sets the path length specified */
    void setPathLength(unsigned int len)
    {
        pLength = len;
    }
    
    /* Read JSON file data */
    void readJsonFile()
    {
        jd->mapAllPairs(); /* Invokes the mapper from JSON class */
        jd->buildCompatibilityGraph(); /* Invokes the graph builder */
        
        nodes = (int)jd->Compatibility_Graph.size();
        Adj_Matrix.resize(nodes,vector<unsigned int>(nodes,0));
        Res_Matrix.resize(nodes,vector<double>(nodes,0.0));
        Adj_Matrix = jd->Compatibility_Graph;
    }
    
    /* Read in the Weight/Adjacency Matrix from a DAT file directly */
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
        Res_Matrix.resize(nodes,vector<double>(nodes,0.0));
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
    
    /* Builds a constraint name with given string identifier and integer ID */
    char* createConstraintName(string constraintText, unsigned int id )
    {
        string idx = to_string(id);
        string constraintID = constraintText + idx;
        unsigned long length = constraintID.size();
        char *constraintName = new char[length+1];
        strcpy(constraintName, constraintID.c_str());
        return constraintName;
    }
    /* Builds a Decision Variable name with given integer IDs */
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
    
    /* Prints the result matrix */
    void printMatrix(vector<vector<double>>& Mat)
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
    
    /* Map all cplex-matched donors and patients */
    void setMatchedPairs()
    {
        for(unsigned int i = 0; i < Res_Matrix.size(); i++)
        {
            for(unsigned int j = 0; j < Res_Matrix.size(); j++)
            {
                if(Res_Matrix[i][j] == 1)
                {
                    //int rID = jd->getID(i);
                    //int dID = jd->getID(j);
                    //MatchedPairs.push_back(make_pair(dID,rID));
                    //MatchedPairs.push_back(make_pair(i,j));
                    pairsIncluded++;
                    myPairs[i] = j;
                }
            }
        }
    }
    
    /* Fetches the exchange cycles involved using DFS*/
    void obtainCycles()
    {
        vector<bool>collected(myPairs.size(),false);
        vector<unsigned int> cyclops;
        cout<<myPairs.size()<<endl;
        for(auto it = myPairs.begin(); it!= myPairs.end(); it++)
        {
            if(!collected[it->first])
            {
                cyclops.push_back(it->first);
                collected[it->first] = true;
                //cyclops.push_back(it->second);
                auto itr = myPairs.find(it->second);
                while(itr!= myPairs.end() && !collected[itr->first])
                {
                    cyclops.push_back(itr->first);
                    collected[itr->first] = true;
                    //cyclops.push_back(itr->second);
                    itr = myPairs.find(itr->second);
                }
                cyclops.push_back(itr->first);
                cycles.push_back(cyclops);
                cyclops.clear();
            }
        }
        
    }
    /* Prints the cycles involved in the final exchange */
    void printCycles()
    {
        //jd->printExchangesToJson(cycles,pairsIncluded);
        for(unsigned int i = 0; i < cycles.size(); i++)
        {
            for(unsigned int j = 0; j < cycles[i].size(); j++)
            {
                cout<<cycles[i][j]<<"->";
            }
            cout<<endl;
        }
    }
    
    
    
    /* Prints the matched pairs to JSON file and logfile */
    void printMatches()
    {
        for(auto i = myPairs.begin(); i != myPairs.end(); i++)
        {
            cout<<"Patient "<<jd->getKeyID(i->first)<<" gets Kidney from "<<jd->getKeyID(i->second)<<endl;
        }
        jd->printToJson(myPairs,json_filename);
    }
    
    void interactor()
    {
        //char filechoice;
        string pathName;
        unsigned int len;
        cout<<"Welcome to Kidney Exchange Solver"<<endl;
        cout<<"Enter the pathname for your JSON file:"<<endl;
        cin>>pathName;
        cout<<"Enter the value of L for path length constraint:";
        cin>>len;
        setPathLength(len);
        setJsonFilename(pathName);
        start = high_resolution_clock::now();
        readJsonFile();
//        cout<<"Choose File Type: (a)JSON File, (b)DAT File"<<endl;
//                cin>>filechoice;
//        switch(filechoice)
//        {
//            case 'a':
//                cout<<"Enter the pathname for your JSON file:"<<endl;
//                cin>>pathName;
//                setJsonFilename(pathName);
//                auto start = high_resolution_clock::now();
//                readJsonFile();
//                break;
//
//
//            case 'b': // Not yet configured completely
//                cout<<"Enter the pathname for your DAT file:"<<endl;
//                cin>>pathName;
//                setDatFilename(pathName);
//                readDatFile();
//                break;
//
//            default : // Not yet configured completely
//                cout<<"Going ahead with default files"<<endl;
//                readJsonFile();
//
//        }
        
    }
    
};


int main(int argc, const char * argv[])
{
    IloEnv env;  /* Construct the CPLEX environment */
    try
    {
        LpSolver solver;
        solver.interactor(); /* Interact with user */
        
        /* Initialise the CPLEX variables and read in the Weight/Adjacency Matrix*/
        IntMatrix weights(env,solver.nodes);
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
        

        /* Define a complete optimization model that can later be extracted to a IloCplex object */
        IloModel model(env);
        
        /* Decision variables for each of the edges in the graph.*/
        NumVarMatrix s(env);
        
        /*Expression Arrays to store the sum of all edges into and out of that vertex in question.*/
        for(unsigned int i = 0; i < solver.nodes; i++)
        {
            s.add(IloNumVarArray(env,solver.nodes,0,1,ILOBOOL)); // ILOBOOL for 0/1 values
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
        /* Populate all paths and required edge data to be used in constraint making */
        solver.populateAllPaths();
        solver.populateEdgeData();
        
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
            IloConstraint pathLengthConstraint(IloSum(PathSum) <= (IloInt)solver.pLength - 1);
            char* constraintName3 = solver.createConstraintName("Path_Originating_From_",i);
            pathLengthConstraint.setName(constraintName3);
            model.add(pathLengthConstraint);
            PathSum.end();
        }
        
        /* Create and Add the Objective function: MAXIMIZE : Sigma (e_out * w(e)) */
        IloExpr obj(env);
        for(IloInt i = 0; i < solver.nodes; i++)
        {
            for(IloInt j = 0; j < solver.nodes; j++)
            {
                obj+=weights[i][j] * s[i][j];
            }
        }
        
        /* Adding the Objective Function */
        IloObjective MaxCovers = IloMaximize(env, obj);
        MaxCovers.setName("Max_Cycle_Covers_Objective");
        model.add(MaxCovers);
        obj.end();
        
        IloCplex cplex(env); /* Create a CPLEX object for solving th model */
        
        cplex.setName("KidneyExchangeByEdgeCovers");
        cplex.extract(model); /* Extract the model created into CPLEX object */
        cplex.exportModel ("/Users/sameerdesai/Downloads/MTP\ Project\ Workspace/KidneyExchangeMechanisms/KidneyExchangeMechanisms/KE1.lp");
        /* Export the OPtimization model created to a file */
        
        /* Keep track of time elapsed in solving the LP */
        IloTimer clock(env);
        clock.start();
        /* Solve the model */
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
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - solver.start);
        cout << "Time taken:"<< (double)duration.count()/1000 << " seconds" << endl;
        //solver.printMatrix(solver.Res_Matrix); /* Prints the Resultant Matrix */
        solver.setMatchedPairs(); /* Invokes the function to Map all matched patients and donors */
        //solver.printMatches();    /* Invokes the function to print all matched pairs */
        solver.obtainCycles();    /* Fetch all the cycles involved in the exchange */
        //solver.printCycles();     /* Print all cycles involved */
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

