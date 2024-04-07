//
// Created by CLAKERS on 2020/8/22.
//
#ifndef IMAGE_UTILITYFUNCTION_H
#define IMAGE_UTILITYFUNCTION_H
#include <iostream>
#include "fstream"
#include "time.h"
#include "vector"
#include "random"
#include "set"
#include "algorithm"
#include "list"
#include "map"
#include "unordered_set"
using namespace std;
const int node_num=600;
const int m=3072;
const int group_num=3;
const int d=1;

vector<int> label;
vector<vector<int>> feature;
vector<double> original_cost;
vector<vector<double>> similarity;

double min_cost=999999999;
double max_cost=-999999999;
string cost_text="./rdata/category1/rd_contrast.txt";
string feature_text="./rdata/category1/rd_feature.txt";

const double ave_num=0.5;
//const double ave_num=1.0;
vector<int> group_upper;
vector<int> group_lower;
vector<int> group_node_num;
vector<double> node_cost;
bool B_wrong=false;
class Node
{
public:
    Node(){};
    Node(int num, int g,double c)
    {
        id=num;
        group=g;
        cost=c;
    }
    int id;
    int group;
    double cost;
};
vector<Node> Groundset;
void read()
{
    feature.resize(node_num, vector<int>(m));
    group_node_num.resize(group_num,0);
    group_upper.resize(group_num);
    group_lower.resize(group_num);

    ifstream in1(feature_text);
    //ifstream in1("feature.txt");
    int i=0;
    int templable=-1;
    while(!in1.eof())
    {
        in1>>templable;
        if (in1.fail())
            break;
        label.push_back(templable);
        for(int j=0;j<m;j++)
            in1 >> feature[i][j];

        i++;

        //record the number of nodes in each group
        group_node_num[templable]++;
    }
    in1.close();

  //  cout << "label num: " << label.size() << " | " << i << endl;

    ifstream in2(cost_text);
    //ifstream in2("contrast.txt");
    i=0;
    double temp_cost;
    double sum_cost=0.0;
    while(!in2.eof())
    {
        in2>>temp_cost;
        if (in2.fail())
            break;

        if(temp_cost<min_cost)
            min_cost=temp_cost;
        if(temp_cost>max_cost)
            max_cost=temp_cost;

        original_cost.emplace_back(temp_cost);
        sum_cost+=temp_cost;
        i++;
        //cout<<temp_cost<<endl;
    }
    in2.close();
   // cout << "cost num: " << original_cost.size() << " | " << i << endl;


    //cout<<"normalize: "<<endl;
    //double ave_cost=sum_cost/node_num;
    //*
    for (int j = 0; j < original_cost.size(); ++j) {
        double normalize_cost=(node_num*original_cost[j])/(sum_cost*ave_num);
        Groundset.emplace_back(j,label[j],normalize_cost);
    }
//    for(auto &x:original_cost)
//    {
//        x=(node_num*x)/(sum_cost*ave_num);
//        //cout<<x<<endl;
//    }
    //*/
  //  cout<<"node num: "<<node_num<<endl;
}
void cal_similarity()
{
    similarity.resize(node_num, vector<double>(node_num));
    //inner product
    /*
    for(int i = 0;i < feature.size();i++)
    {
        for(int j = i;j < feature.size();j++)
        {
            double product = 0.0;
            for(int k = 0;k < feature[0].size();k++)
            {
                product+=(feature[i][k]*feature[j][k]);
            }
            similarity[i][j] = product;
            similarity[j][i] = product;
        }
    }
     */
    //distance
    /*
    for(int i = 0;i < feature.size();i++)
    {
        for(int j = i;j < feature.size();j++)
        {
            double distance = 0.0;
            for(int k = 0;k < feature[0].size();k++)
            {
                distance += pow((feature[i][k]-feature[j][k]),2);
            }
            distance = sqrt(distance);
            similarity[i][j] = distance;
            similarity[j][i] = distance;
        }
    }
     //*/
    //consine
    //*
    for(int i = 0;i < feature.size();i++)
    {
        for(int j = i;j < feature.size();j++)
        {
            double product = 0.0;
            double module_a=0.0;
            double module_b=0.0;
            for(int k = 0;k < feature[0].size();k++)
            {
                product+=(feature[i][k]*feature[j][k]);
                module_a+=pow(feature[i][k],2);
                module_b+=pow(feature[j][k],2);
            }
            module_a=sqrt(module_a);
            module_b=sqrt(module_b);
            similarity[i][j] = product/(module_a*module_b);
            similarity[j][i] = product/(module_a*module_b);
        }
    }
    // */
}
double f_u(int node)
{
    double sum_value=0.0;
    for(int i=0;i<node_num;i++)
    {
        sum_value += similarity[i][node];
    }
  //  return sum_value-similarity[node][node]/(node_num);
    return sum_value;
}
double u_marginal_for_S(const int &e, const vector<int> &solution)
{
    if(solution.empty())
    {
        return f_u(e);
    }
    double v1=0.0;
    //double v2=0.0;

//    for(const auto &i:solution)
//    {
//        if(e == i) return 0.0;
//
//        v2+=similarity[e][i];
//        v2+=similarity[i][e];
//    }
//    v2+=similarity[e][e];

    for(int i=0;i<node_num;i++)
    {
        double max_similarity=-999999999;
        for(auto j:solution)
        {
            if(similarity[i][j]>max_similarity)
                max_similarity=similarity[i][j];
        }

        if(similarity[i][e]>max_similarity)
        {
            v1+=(similarity[i][e]-max_similarity);
        }
    }
    return v1;
//    return v1-v2/(node_num);
}

bool budget_feasible_single(const int &node,double Budget)
{
    /**********check single node Budget constraint**********/
    if(Groundset[node].cost > Budget)
        return false;
    else
        return true;
}

class S_class {
public:
    S_class() {
        s_cost=0.0;
        s_revenue=0.0;
        for(int iter=0; iter < group_num; iter++)
            group_exist.push_back(0);

    }
    //value
    double s_cost;
    double s_revenue;
    unordered_set<int> solution;
    vector<int> group_exist;

    void clear()
    {
        s_revenue = 0.0;
        s_cost=0.0;
        solution.clear();
        for(int iter=0; iter < group_num; iter++)
            group_exist[iter]=0;
    }
    void replace_with_singleton(const double &marginal,const int &node)
    {
        clear();
        add_element(marginal,node);
    }
    void add_element(const double &marginal,const int &node)
    {
        solution.emplace(node);
        s_revenue += marginal;
        s_cost+=Groundset[node].cost;

        group_exist[Groundset[node].group]++;;//S+e\in I
    }
    void add_element_without_marginal(const int &node_id)
    {
        s_revenue+=marginal_gain(node_id);
        s_cost+=Groundset[node_id].cost;
        group_exist[Groundset[node_id].group]++;

        solution.emplace(node_id);
    }
    void deleta_add_element(const int &node_id, const int &v_ix)
    {
        s_cost+=Groundset[node_id].cost;
        s_cost-=Groundset[v_ix].cost;
        solution.emplace(node_id);
        solution.erase(v_ix);
        s_revenue=f_S();
    }
    bool budget_feasible(const int &node_id,const double &budget)
    {
        if(s_cost + Groundset[node_id].cost <= budget)
            return true;
        else
            return false;
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
    double f_S()
    {
        if(solution.empty())
            return 0.0;

        double v1=0.0;
       // double v2=0.0;
        for(int i=0;i<node_num;i++)
        {
            double max_temp=-999999999;
            for(const auto &j:solution)
            {
                if(similarity[i][j] > max_temp)
                    max_temp=similarity[i][j];
            }
            v1+=max_temp;
        }
//        for(const auto &i:solution)
//        {
//            for(const auto &j:solution)
//            {
//                v2+=similarity[i][j];
//            }
//        }
//        return v1-v2/(node_num);
        return v1;
    }
    double marginal_gain(int e)
    {
        if(solution.empty())
        {
            return f_u(e);
        }
        double v1=0.0;
       // double v2=0.0;

//        for(const auto &i:solution)
//        {
//            if(e == i) return 0.0;
//
//            v2+=similarity[e][i];
//            v2+=similarity[i][e];
//        }
//        v2+=similarity[e][e];

        for(int i=0;i<node_num;i++)
        {
            double max_similarity=-999999999;
            for(auto j:solution)
            {
                if(similarity[i][j]>max_similarity)
                    max_similarity=similarity[i][j];
            }

            if(similarity[i][e]>max_similarity)
            {
                v1+=(similarity[i][e]-max_similarity);
            }
        }

//        return v1-v2/(node_num);
        return v1;
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
#endif //IMAGE_UTILITYFUNCTION_H
