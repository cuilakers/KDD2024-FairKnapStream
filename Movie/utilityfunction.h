
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
#include <set>
#include "list"
#include "unordered_set"
#include "random"
//#define pi 3.1415926
using namespace std;
//Adventure Fantasy Animation
vector<float> sim_mat_sum;
vector<vector<float>> sim_mat;
vector<vector<int>> genres_index;//Adventure Fantasy Animation (1,1,1)
const int group_num=3;//h in KDD 2024
vector<int> group_upper;
vector<int> group_lower;
vector<int> group_node_num;
vector<double> node_cost;

int node_num=-1;
const int d=1;

double min_cost=999999999;
const int k=3;

double lambda_f;
const double ave_num=1.0;//normalize the cost to 1/ave_num in average
const int max_movie=999999999;
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
class dataset{
public:
    vector<int> index;
    set<int> v;
    vector<vector<float>> matrix;
    vector<vector<string>> genres;
    vector<string> titles;
    float lambda;
    vector<int> year;
    vector<float> rating;
    vector<float> year_costs;
    vector<float> rate_costs;
    vector<vector<double>> costs;
    //vector<vector<float>> sim_mat;
    dataset(string);
    void cost_normalize();
    void readfile(string filename);
    vector<float> feature_extractor(string features,char delim);
    vector<string> genre_extractor(string genres,char delim);
    void sim_mat_generator();
};
dataset::dataset(string filename)
{
    readfile(filename);
    cost_normalize();
    sim_mat_generator();

    node_num=index.size();
    sim_mat_sum = vector<float>(node_num,0.0);
    //for(int it=0;it < node_num;it++)
        //genres_index.push_back(vector<int>(k,0));
    group_node_num.resize(group_num,0);
    for(int it=0;it < node_num;it++)
    {
        genres_index.push_back(vector<int>(k,0));
        for(const auto& p: genres[it])
        {
            if(p=="Adventure")
                genres_index[it][0]=1;
            if(p=="Fantasy")
                genres_index[it][1]=1;
            if(p=="Animation")
                genres_index[it][2]=1;
        }
        int group;
        for(const auto& p: genres[it])
        {
            if(p=="Adventure") {
                group=0;
                break;
            }
            if(p=="Fantasy")
            {
                group=1;
                break;
            }
            if(p=="Animation")
            {
                group=2;
                break;
            }
        }
        /********test cost*******/
        Groundset.emplace_back(it,group,node_cost[it]+1.0);
        group_node_num[group]++;
        /********test cost*******/
        /*
        cout<<it<<"\t";
        for(auto p:genres_index[it])
            cout<<p<<"\t";
        cout<<endl;
*/
        for (int itv = 0; itv < node_num; itv++) {
            sim_mat_sum[it] += sim_mat[it][itv];
            //cout<<sim_mat_sum[it]<<endl;
        }
    }
}

void dataset::cost_normalize(){
    float rate = rating.size()/ave_num;
    float rate_coef,year_coef;
    float rate_sum=0.0;
    float year_sum=0.0;
    float rate_base = 10.0,year_base = 1985.0;
    for(int i = 0;i < rating.size();i++){
        rate_sum += rating.at(i);
        year_sum += abs(year.at(i)-year_base);
    }
    rate_sum = rating.size()*rate_base - rate_sum;
    //year_sum = rating.size()*year_base;
    rate_coef = rate/rate_sum;
    year_coef = rate/year_sum;

    for(int i=0;i<d;i++)
        costs.push_back(vector<double>());
    //normalization
    //*
    for(int i = 0;i < rating.size();i++){
        rate_costs.push_back(fabs(rating.at(i)-rate_base)*rate_coef);
        year_costs.push_back(fabs(year.at(i)-year_base)*year_coef);
       // cout<<rate_costs[i]<<"\t"<<year_costs[i]<<endl;

       costs[0].push_back(rate_costs[i]);
       if(d>=2) costs[1].push_back(year_costs[i]);

        node_cost.emplace_back(rate_costs[i]);
    }
    //*/
//*
    for(int i=0;i<rate_costs.size();i++)
    {
        double temp=max(rate_costs[i],year_costs[i]);
        if(temp<min_cost)
            min_cost=temp;
    }
    //*/
}


void dataset::readfile(string filename){
    //dataset* data = new dataset();
    ifstream infofile;
    infofile.open(filename.c_str(), ios::in);
    int count = 0;
    char delim = '|';
    if (infofile.is_open()) {
        while (!infofile.eof()) {
            std::string line;
            getline(infofile, line);
            if (line.empty())
                continue;
            std::string::size_type pos = line.find_first_of(delim);
            v.insert(count++);
            int prevpos = 0;
            int length = line.length();
            // id
            string str = line.substr(prevpos, pos - prevpos);
            index.push_back(atoi(str.c_str()));
            // titles
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            titles.push_back(str);
            // genres
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            genres.push_back(genre_extractor(str,' '));
            // lambda
            lambda = 0.2;
            //features
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            matrix.push_back(feature_extractor(str,' '));

            // year
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            year.push_back(atoi(str.c_str()));

            // rating
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            rating.push_back(atof(str.c_str()));
        }
    }
}

void dataset::sim_mat_generator(){ //计算Mij
    //e^-lambda*dist(v_i,v_j)
    //*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for(int j = i;j < index.size();j++)
        {
            float distance = 0;
            float sim_dis = 0;
            for(int k = 0;k < matrix[0].size();k++)
            {
                distance += pow((matrix[i][k]-matrix[j][k]),2);
            }
            distance = sqrt(distance);
            sim_dis = exp(-1.0*lambda*distance);
            //cout<<sim_dis<<endl;
            sim_mat[i][j] = sim_dis;
            sim_mat[j][i] = sim_dis;
        }
    }
    //*/
    //inner product
    /*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for(int j = i;j < index.size();j++)
        {
            float product = 0;
            //float sim_dis = 0;
            for(int k = 0;k < matrix[0].size();k++)
            {
                product+=(matrix[i][k]*matrix[j][k]);
                //distance += pow((matrix[i][k]-matrix[j][k]),2);
            }
            //distance = sqrt(distance);
            //sim_dis = exp(-1*lambda*distance);
            //cout<<product<<endl;
            sim_mat[i][j] = product;
            sim_mat[j][i] = product;
        }
    }
    //*/
    //consine
    /*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for (int j = i; j < index.size(); j++)
        {
            double product = 0.0;
            double module_a=0.0;
            double module_b=0.0;
            for (int k = 0; k < matrix[0].size(); k++)
            {
                product += (matrix[i][k] * matrix[j][k]);
                module_a+=pow(matrix[i][k],2);
                module_b+=pow(matrix[j][k],2);
            }
            module_a=sqrt(module_a);
            module_b=sqrt(module_b);
            sim_mat[i][j] = product/(module_a*module_b);
            sim_mat[j][i] = product/(module_a*module_b);
        }
    }
    //*/
    //dist(v_i,v_j)
    /*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for(int j = i;j < index.size();j++)
        {
            float distance = 0;
            for(int k = 0;k < matrix[0].size();k++)
            {
                distance += pow((matrix[i][k]-matrix[j][k]),2);
            }
            distance = sqrt(distance);
            sim_mat[i][j] = distance;
            sim_mat[j][i] = distance;
        }
    }
*/
}

vector<float> dataset::feature_extractor(string features, char delim){
    vector<float> str;
    std::string::size_type pos = features.find_first_of(delim);
    int prevpos = 0;
    while(string::npos != pos || string::npos != prevpos){
        str.push_back(atof(features.substr(prevpos,pos-prevpos).c_str()));
        prevpos = features.find_first_not_of(delim, pos);
        pos = features.find_first_of(delim, prevpos);
    }
    return str;
}

vector<string> dataset::genre_extractor(string genres, char delim) {
    vector<string> str;
    std::string::size_type pos = genres.find_first_of(delim);
    int prevpos = 0;
    while (string::npos != pos || string::npos != prevpos) {
        str.push_back(genres.substr(prevpos, pos - prevpos).c_str());
        prevpos = genres.find_first_not_of(delim, pos);
        pos = genres.find_first_of(delim, prevpos);
    }
    return str;
}
double u_marginal_for_S(const int &e, const vector<int> &solution)
{
    float M1=0,M2=0;
    M1 += sim_mat_sum[e];
    for(const auto &it:solution)
    {
        if(e == it) return 0;
        M2 += sim_mat[e][it];
        M2 += sim_mat[it][e];
    }
    M2 += sim_mat[e][e];
    return M1-lambda_f*M2;
}
double f_S(const unordered_set<int> &solution)
{
    double sum_value=0.0;
    float M1 = 0,M2 = 0;
    for(const auto &it:solution){
        M1 += sim_mat_sum[it];
        for(const auto &its:solution)
            M2 += sim_mat[it][its];
    }
    sum_value = M1-lambda_f*M2;
    return sum_value;
}
double f_u(int node)
{
    double sum_value=sim_mat_sum[node];
    /*for(int i=0;i<node_num;i++)
    {
        sum_value += sim_mat[node][i];
    }*/
    return sum_value-lambda_f*sim_mat[node][node];
}
bool budget_feasible_single(const int &node_id,const double &budget)
{
    if(Groundset[node_id].cost<=budget)
        return true;
    else
        return false;
}

class S_class
{
public:
    S_class()
    {
        max_marginal=-999999999.0;
        max_element=-1;

        s_cost=0.0;
        s_revenue=0.0;
        for(int iter=0;iter<group_num;iter++)
            group_exist.push_back(0);

//        for(int i=0;i<node_num;i++)
//        {
//            marginal_store.push_back(make_pair(0,-999999.0));
//        }
    }
    //value
    vector<int> group_exist;
    double s_cost;
    double s_revenue;
    //solution
    unordered_set<int> solution;

//optimize for offline algorithm
    double max_marginal;
    int max_element;

    vector<pair<int,double>> marginal_store;
    void clear()
    {
        s_cost=0.0;
        s_revenue = 0.0;
        for(int i=0;i<group_num;i++)
            group_exist[i]=0;

        solution.clear();
    }
    void replace_with_singleton(const double &marginal,const int &node)
    {
        clear();
        add_element(marginal,node);
    }
    void add_element(const double &marginal,const int &node_id)
    {
        s_revenue+=marginal;
        s_cost+=Groundset[node_id].cost;
        group_exist[Groundset[node_id].group]++;

        solution.emplace(node_id);
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
    double marginal_gain(const int &e) const
    {
        float M1=0,M2=0;
        M1 += sim_mat_sum[e];
        for(const auto &it:solution)
        {
            if(e == it) return 0;
            M2 += sim_mat[e][it];
            M2 += sim_mat[it][e];
        }
        M2 += sim_mat[e][e];
        return M1-lambda_f*M2;
    }
    double f_S()
    {
        double sum_value=0.0;
        float M1 = 0,M2 = 0;
        for(const auto &it:solution){
            M1 += sim_mat_sum[it];
            for(const auto &its:solution)
                M2 += sim_mat[it][its];
        }
        sum_value = M1-lambda_f*M2;
        return sum_value;
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
//class S_array
//{
//public:
//    vector<S_class> sarray;
//    double gamma;
//    //S_array(){}
//    S_array(const double g,const int h)
//    {
//        gamma=g;
//        for(int i=0;i<h;i++)
//            sarray.push_back(S_class());
//    }
//};
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
