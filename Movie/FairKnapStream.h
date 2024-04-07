//
// Created by CLAKERS on 2024/1/25.
//

#ifndef COVERAGE_FAIRKNAPSTREAM_H
#define COVERAGE_FAIRKNAPSTREAM_H
#include "MatKnapStream.h"
#include "map"
#include "unordered_set"
#include "algorithm"
#include "math.h"
//class At
//{
//public:
//    vector<int> set;
//
//};
class L_node
{
public:
    double cost;
    int index;
    L_node()=default;
    L_node(int ind,double cos)
    {
        cost=cos;
        index=ind;
    }
};
bool cmp(const L_node &a, const L_node &b)
{
    return a.cost < b.cost;
}
class FairStreamSolution
{
public:
    vector<vector<L_node>> set;
    vector<double> solution_cost;
    FairStreamSolution()
    {
        for (int i = 0; i < 2; ++i) {
            set.emplace_back();
            solution_cost.emplace_back(0.0);
        }
    }
};
void FairStream(FairStreamSolution &L)
{
    //cout<<"FairStream"<<endl;
    double eta=0.5;
    vector<multimap<double,int>> At;
    for (int t = 0; t < group_num; ++t) {
        At.emplace_back();
    }
    for(int e=0;e<node_num;e++)
    {
        if(At[Groundset[e].group].size()<group_lower[Groundset[e].group])
        {
            At[Groundset[e].group].emplace(Groundset[e].cost,e);
        }
        else
        {
            auto smallest=At[Groundset[e].group].end();
            smallest--;
            if(Groundset[e].cost<smallest->first)
            {
                At[Groundset[e].group].erase(smallest);
                At[Groundset[e].group].emplace(Groundset[e].cost,e);
            }
        }
    }
//    vector<int> L;
//    for(const auto &t:At) {
//        for (const auto &u:t) {
//            L.emplace_back(u.second);
//        }
//    }

    default_random_engine engine(499);
    for (int t = 0; t < group_num; ++t) {
        int sample_num=floor(group_lower[t]*eta);

        // cout<<sample_num<<endl;

        int max_index=At[t].size()-1;

        uniform_int_distribution<int> u(0, max_index);
        unordered_set<int> random_num;
        while (random_num.size() < sample_num) {
            int rn=u(engine);
            random_num.emplace(rn);
        }
        int index=0;
        for(const auto &iter:At[t]) {
            if (random_num.count(index)) {
                L.set[0].emplace_back(iter.second,Groundset[iter.second].cost);
                L.solution_cost[0]+=Groundset[iter.second].cost;
            } else {
                L.set[1].emplace_back(iter.second,Groundset[iter.second].cost);
                L.solution_cost[1]+=Groundset[iter.second].cost;
            }
            index++;
        }
    }
//
//    int violation=0;
//    cout<<" number of fairness violation: "<<endl;
//    for(int j=0;j<group_num;j++)
//    {
//        int err=max(0,group_lower[j]-(int)At[j].size());
//        violation+=err;
//        cout<<err<<" ";
//    }
//    cout<<endl;
//    cout<<"sum violation: "<<violation<<endl;
//    if(violation>0)
//        cout<<"Budget is wrong! "<<endl;

}
Result FairKnapStream(double eps,double B)
{
    cout<<"FairKnapStream & Budget: "<<B<<"---------start---------"<<endl;
    long long int oracle_times;

    FairStreamSolution L;
    FairStream(L);
    vector<S_class> Tx;

    for (int i = 0; i < 2; ++i) {

        if(B-L.solution_cost[i] <= 0)
        {
            cout << "Budget is wrong !" << endl;
            B_wrong=true;
        }

        Tx.emplace_back(MatKnapStream(eps,B-L.solution_cost[i], oracle_times));
    }

//    sort(L.set[0].begin(),L.set[0].end(),cmp);
//    sort(L.set[1].begin(),L.set[1].end(),cmp);

    for (int i = 0; i < 2; ++i) {
        for (const auto &e:L.set[i]) {
            if (Tx[i].fair_upper(e.index)) {
                Tx[i].add_element_without_marginal(e.index);
            }
        }
        //heuristic
        for (const auto &e:L.set[(i+1)%2]) {
            /********heuristic2********/
            if (Tx[i].is_fair_lower_solution(e.index)&&Tx[i].fair_upper(e.index)&&Tx[i].budget_feasible(e.index,B)) {
                /********heuristic2********/
                //if (Tx[i].fair_upper(e.index)&&Tx[i].budget_feasible(e.index,B)) {
                double marginal=Tx[i].marginal_gain(e.index);
                if(marginal>=0)
                    Tx[i].add_element_without_marginal(e.index);
                if(marginal<0)
                    cout<<"non-negative !"<<endl;
            }
        }
    }

    double max_utility=-1.0;
    S_class T_star;
    for (int i = 0; i < 2; ++i) {
        if (Tx[i].s_revenue >= max_utility) {
            max_utility = Tx[i].s_revenue;
            T_star = Tx[i];
        }
    }

    //construct solution L
    S_class solutionL;
    for(const auto &u:L.set[0])
    {
        solutionL.add_element(0.0,u.index);
    }
    for(const auto &u:L.set[1])
    {
        solutionL.add_element(0.0,u.index);
    }
    solutionL.s_revenue=solutionL.f_S();
    if(solutionL.s_revenue>=T_star.s_revenue)
        T_star=solutionL;

    cout<<"FairKnapStream & Budget: "<<B<<endl;
    cout<<"T*:"<<endl;
    cout << "  utility: " << T_star.s_revenue << " cost: " << T_star.s_cost << " size: " << T_star.solution.size() << endl;
//    cout<<" node"<<endl;
//    for(const auto &p:T_star.solution)
//        cout<<p<<" ";
//    cout<<endl;
    int violation=0;
//    cout<<"size in each group: "<<endl;
//    for(int j=0;j<group_num;j++)
//    {
//        cout<<T_star.group_exist[j]<<" ";
//    }
    cout<<endl;
    cout<<" number of fairness violation: "<<endl;
    for(int j=0;j<T_star.group_exist.size();j++)
    {
        int err=max(0,group_lower[j]-T_star.group_exist[j]);
        violation+=err;
        cout<<err<<" ";
    }
    cout<<endl;
    cout<<"sum violation: "<<violation<<endl;

    return Result(T_star.s_revenue, T_star.s_cost, T_star.solution.size(), oracle_times, violation);
}

#endif //COVERAGE_FAIRKNAPSTREAM_H
