
#ifndef COVERAGE_STREAMINGHEURISTIC_H
#define COVERAGE_STREAMINGHEURISTIC_H
#include "utilityfunction.h"
Result StreamingHeuristicRandom(double eps,double B)
{
    cout<<"RandomStream & Budget: "<<B<<"---------start---------"<<endl;
    S_class S;
    bernoulli_distribution ran(1.0/2.0);
    default_random_engine e(12345);
    for(int u=0;u<node_num;u++)
    {
        if(S.budget_feasible(u,B))
        {
            if(ran(e)==1)
                S.add_element(0.0,u);
        }
    }
    S.s_revenue=S.f_S();

    cout<<"RandomStream & Budget: "<<B<<endl;
    cout<<"S*:"<<endl;
    cout << "  utility: " << S.s_revenue << " cost: " << S.s_cost << " size: " << S.solution.size() << endl;
//    for(const auto &p:S.solution)
//        cout<<p<<" ";
//    cout<<endl;

    int violation=0;
//    cout<<"size in each group: "<<endl;
//    for(int j=0;j<S.group_exist.size();j++)
//    {
//        cout<<S.group_exist[j]<<" ";
//    }
//    cout<<endl;
    cout<<" number of fairness violation: "<<endl;
    for(int j=0;j<S.group_exist.size();j++)
    {
        int err=max(S.group_exist[j]-group_upper[j],group_lower[j]-S.group_exist[j]);
        err=max(err,0);
        violation+=err;
        cout<<err<<" ";
    }
    cout<<endl;
    cout<<"sum violation: "<<violation<<endl;

    return Result(S.s_revenue, S.s_cost, S.solution.size(), 0, violation);
}


#endif //COVERAGE_STREAMINGHEURISTIC_H
