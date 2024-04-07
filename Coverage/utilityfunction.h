#ifndef STREAMING_ALGORITHM_UTILITY_FUNCTIONS_H
#define STREAMING_ALGORITHM_UTILITY_FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sstream>
#include <climits>
#include <string>
#include <vector>
#include <cmath>
#include <unordered_set>
#include "list"
#include "random"
using namespace std;
const int d=1;

string edge_text="./Graph/epinions/renum_edge.txt";
string node_text="./Graph/epinions/renum_node.txt";
int node_num=0;
int edge_num=0;
//const int k=2;
double min_cost=999999999.0;
double max_cost=-1.0;
const int group_num=5;//h in KDD 2024
vector<int> group_upper;
vector<int> group_lower;
vector<int> group_node_num;
/*************???????????????****************/
const double ave_num=1.0;//normalize the cost to 1/ave_num in average

bool B_wrong=false;
const int q=0;//a number for modifying degree
//double budget;
class Node
{
public:
    Node(){};
    Node(int num, int g,double w,double c)
    {
        id=num;
        group=g;
        weight=w;
        cost=c;
    }
    int id;
    int group;
    double weight;
    double cost;
};
vector<Node> Groundset;
vector<vector<int>> Edge;
void read_data()
{
    ifstream in1(node_text);
    int node_temp;
    int group_temp;
    double weight_temp;
    int degree_temp;

//    uniform_real_distribution<double> ran(0.00001,1.0);
//    default_random_engine e(123);
    double sum_cost=0.0;
    group_node_num.resize(group_num,0);
    while(!in1.eof())
    {
        in1>>node_temp>>weight_temp>>degree_temp>>group_temp;
        //calculate cost
//        int cost_temp=degree_temp-q;
//        int cost=1+max(cost_temp,0);
        double cost=1.0+sqrt(degree_temp);
        cost=1.0-exp(-0.2*cost);

        if (in1.fail())
            break;
        group_node_num[group_temp]++;
        Groundset.emplace_back(node_temp,group_temp,1.0,cost);
        sum_cost+=cost;
    }
    in1.close();

    for(auto & i : Groundset) {
        Edge.emplace_back();
    }


    ifstream in2(edge_text);
    int v1,v2;
    edge_num=0;
    unordered_set<int> node;
    while(!in2.eof())
    {
        in2>>v1>>v2;
        if (in2.fail())
            break;
        edge_num++;
        node.insert(v1);
        node.insert(v2);
        Edge[v1].push_back(v2);
    }
    node_num=node.size();
    cout<<"map node size: "<<node_num<<endl;
    cout<<"map edge size: "<<edge_num<<endl;

//    for(auto & i : Groundset) {
//        double normalize_cost=(node_num*i.cost)/(sum_cost*ave_num);
//        i.cost=normalize_cost;
//    }

    in2.close();
}
double f_u(const int &node_id)
{
    double sum_weight=0.0;
    for(const auto &v: Edge[node_id])
    {
        sum_weight+=Groundset[v].weight;
    }
    return sum_weight;
}
double u_marginal_for_S(const int &node_id, const vector<int> &S)
{
    unordered_set<int> covered_vertex;
    for(const auto &u:S) {
        for (const auto &v:Edge[u]) {
            covered_vertex.emplace(v);
        }
    }
    double sum_weight=0.0;
    for(const auto &v:Edge[node_id]) {
        if (!covered_vertex.count(v)) {
            sum_weight += Groundset[v].weight;
        }
    }
    return sum_weight;
}
bool budget_feasible_single(const int &node_id,const double &budget)
{
    if(Groundset[node_id].cost<=budget)
        return true;
    else
        return false;
}
class S_class {
public:
    S_class() {
        s_cost=0.0;
        s_revenue = 0.0;
        for(int i=0;i<group_num;i++)
            group_exist.push_back(0);
        max_marginal=-999999999.0;
        max_element=-1;
    }
    //value
    double s_cost;
    double s_revenue;
    vector<int> group_exist;
    //set
    unordered_set<int> solution;
    unordered_set<int> node_covered;
    //used for offline algorithm
    double max_marginal;
    int max_element;
    void clear()
    {
        s_cost=0.0;
        s_revenue = 0.0;
        for(int i=0;i<group_num;i++)
            group_exist[i]=0;

        solution.clear();
        node_covered.clear();
    }
    void replace_with_singleton(const double &marginal,const int &node)
    {
        clear();
        add_element(marginal,node);
    }
    double f_S()
    {
        unordered_set<int> covered_vertex;

        double sum_weight=0.0;
        //double sum_cost=0.0;
        for(const auto &u:solution)
        {
            //sum weight of N(S)
            for(const auto &v:Edge[u])
            {
                if(!covered_vertex.count(v))
                {
                    sum_weight+=Groundset[v].weight;
                    covered_vertex.emplace(v);
                }
            }
/*            //sum weight of S
            if(!covered_vertex.count(u))
            {
                sum_weight+=Groundset[u].weight;
                covered_vertex.emplace(u);
            }*/
            //sum_cost+=Groundset[u].cost;
        }
      //  return sum_weight-sum_cost;
        return sum_weight;
    }
    void add_element(const double &marginal,const int &node_id)
    {
        s_revenue+=marginal;
        s_cost+=Groundset[node_id].cost;
        group_exist[Groundset[node_id].group]++;

        solution.emplace(node_id);
        for(const auto &v:Edge[node_id])
            node_covered.emplace(v);
        //node_covered.emplace(node_id);
    }

    void add_element_without_marginal(const int &node_id)
    {
        s_revenue+=marginal_gain(node_id);
        s_cost+=Groundset[node_id].cost;
        group_exist[Groundset[node_id].group]++;

        solution.emplace(node_id);
        for(const auto &v:Edge[node_id])
            node_covered.emplace(v);
        //node_covered.emplace(node_id);
    }
    void deleta_add_element(const int &node_id, const int &v_ix)
    {
        s_cost+=Groundset[node_id].cost;
        s_cost-=Groundset[v_ix].cost;
        //group_exist[Groundset[node_id].group]++;
        solution.emplace(node_id);
        solution.erase(v_ix);

        node_covered.clear();
        double sum_weight=0.0;
        for(const auto &u:solution)
        {
            //sum weight of N(S)
            for(const auto &v:Edge[u])
            {
                if(!node_covered.count(v))
                {
                    sum_weight+=Groundset[v].weight;
                    node_covered.emplace(v);
                }
            }
        }
        s_revenue=sum_weight;
    }
    const double marginal_gain(const int &node_id)
    {
        double sum_weight=0.0;
        for(const auto &v:Edge[node_id])
        {
            if(!node_covered.count(v))
            {
                sum_weight+=Groundset[v].weight;
            }
        }
       // return sum_weight-Groundset[node_id].cost;
        return sum_weight;
    }
    bool fair_upper(const int &node_id)
    {
        if(group_exist[Groundset[node_id].group]<group_upper[Groundset[node_id].group])
            return true;
        else
            return false;
    }
    //check the group of node_id whether already fair in the solution
    bool is_fair_lower_solution(const int &node_id)
    {
        if(group_exist[Groundset[node_id].group]<group_lower[Groundset[node_id].group])
            return true;
        else
            return false;
    }
    bool budget_feasible(const int &node_id,const double &budget)
    {
        if(s_cost + Groundset[node_id].cost <= budget)
            return true;
        else
            return false;
    }
};
class Result
{
public:
    Result(){}
    Result(double rev,double cos,int siz,long long int ora)
    {
        revenue=rev;
        cost=cos;
        size=siz;
        oracle=ora;
    }
    Result(double rev,double cos,int siz,long long int ora,long long int mem)
    {
        revenue=rev;
        cost=cos;
        size=siz;
        oracle=ora;
        memory=mem;
    }
    Result(double rev,double cos,int siz,long long int ora,int vio)
    {
        revenue=rev;
        cost=cos;
        size=siz;
        oracle=ora;
        violation_num=vio;
    }
    double revenue;
    long long int oracle;
    long long int round;
    double cost;
    int size;
    long long int time=0;
    long long int max_query=0;
    long long int memory=0;
    int violation_num;
};

#endif //STREAMING_ALGORITHM_UTILITY_FUNCTIONS_H