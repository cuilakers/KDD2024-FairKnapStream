//
// Created by CLAKERS on 2024/1/26.
//

#ifndef COVERAGE_SMKSTREAM_H
#define COVERAGE_SMKSTREAM_H
#define pi 3.1415926
#include "utilityfunction.h"
bool non_monotone=false;
class S_pair
{
public:
    vector<S_class> spair;
    double gamma;
    S_pair(){}
    S_pair(double g)
    {
        gamma=g;
        spair.emplace_back();
        if(non_monotone)
            spair.emplace_back();
    }
};
Result SmkStream(double B,double eps)
{
    long long int oracle_times=0;
    cout<<"SmkStream---------start--------- "<<endl;
    double alpha;
    int solution_num;
    //eps/=8.0;
    if(non_monotone)
    {
        alpha=6.0;
        solution_num=2;
    }
    else
    {
        alpha=2.0;
        solution_num=1;
    }

    oracle_times=0;
    S_class S_best;
    vector<S_pair> S_array;
    double max_gamma=0.0;//max gamma in S_array, help to update S_array
    int min_gamma_index_in_C=-1;//-1 means S_array is empty, else means min gamma index which is still available now
    int max_gamma_index_in_C=-1;
   // int onepass_i=0;
    //double K;

    //int sum_gamma_num=0;
    double LB_ave=0.0;
    double fu_ave=0.0;
    for(int u=0;u<node_num;u++)
    {
        //generate C
        if(!budget_feasible_single(u,B))
            continue;

        //onepass_i++;
       // K=(pi*pi*(1+eps)*(double)onepass_i*(double)onepass_i)/(6.0*eps);

        oracle_times++;

        double fu_value=f_u(u);
        if(fu_value>S_best.s_revenue)
        {
            S_best.replace_with_singleton(fu_value,u);
        }
        if(fu_value<=0) continue;
        double left_temp= S_best.s_revenue / (alpha * B * (1.0 + eps));
        double right_temp=fu_value/Groundset[u].cost;

        LB_ave+=S_best.s_revenue;
        fu_ave+=right_temp;

        if(left_temp>right_temp) continue;

        double left=ceil(log(left_temp)/log(1+eps));
        double right=floor(log(right_temp)/log(1+eps));

        //cout<<"gamma_num:"<<right-left<<endl;
        //sum_gamma_num+=(right-left);

        //vector<double> C;
        /*******fix bug*******/
        left=max(left,1.0);
        right=max(right,1.0);
        /*******fix bug*******/
        if(min_gamma_index_in_C==-1)//first visit S_array
        {
            for(int t=left;t<=right;t++)
            {
                double gamma_temp=pow(1+eps,t);
                S_array.push_back(S_pair(gamma_temp));
            }
            min_gamma_index_in_C=0;//the index of min gamma which is still available now
            max_gamma=pow(1+eps,right);//now max gamma
            max_gamma_index_in_C=S_array.size()-1;//max gamma index in C now
        }
        else//not first visit
        {
            double now_min_gamma_in_C=pow(1+eps,left);//min gamma in C now
            double now_max_gamma_in_C=pow(1+eps,right);//max gamma in C now
            if(now_min_gamma_in_C>max_gamma)//all old S_arrary shouble be removed
            {
                min_gamma_index_in_C=S_array.size();//the index of min gamma which is still available now
                for(int t=left;t<=right;t++)//then add new S gamma pair
                {
                    double gamma_temp=pow(1+eps,t);
                    S_array.push_back(S_pair(gamma_temp));
                }
                max_gamma_index_in_C=S_array.size()-1;//max gamma index in C now
                max_gamma = S_array[S_array.size() - 1].gamma;//the last element always is the max_gamma anyway
            }
            else//else find where is the min gamma index now, is equivalent to remove all S gamma < left
            {
                bool need_update= true;//judge S_array shouble be updated or not
                for (int iter = min_gamma_index_in_C; iter < S_array.size(); iter++)
                {
                    if (S_array[iter].gamma < now_min_gamma_in_C)
                        min_gamma_index_in_C++;
                    if (S_array[iter].gamma >= now_max_gamma_in_C)
                    {
                        max_gamma_index_in_C=iter;
                        need_update=false;
                        break;
                    }
                }
                if(need_update)
                {
                    //finally, go through all gamma now, put new S gamma pair if needed
                    for (int t = left; t <= right; t++)
                    {
                        double gamma_temp = pow(1 + eps, t);
                        if (gamma_temp > max_gamma)
                            S_array.push_back(S_pair(gamma_temp));
                    }
                    max_gamma = S_array[S_array.size() - 1].gamma;//the last element always is the max_gamma anyway
                    max_gamma_index_in_C=S_array.size() - 1;
                }
            }
        }
        //foreach gamma\in C
        for(int iter=min_gamma_index_in_C;iter<=max_gamma_index_in_C;iter++)
        {
            int max_solution=-1;
            double max_marginal=-999.0;
            for (int x = 0; x < solution_num; ++x) {
                double temp_marginal=S_array[iter].spair[x].marginal_gain(u);
                oracle_times++;
                if(temp_marginal>=max_marginal)
                {
                    max_marginal=temp_marginal;
                    max_solution=x;
                }
            }
            if(max_solution!=-1)
            {
                if(max_marginal>=S_array[iter].gamma&&S_array[iter].spair[max_solution].budget_feasible(u,B))
                {
                    S_array[iter].spair[max_solution].add_element(max_marginal,u);
                }
            }
            if(S_array[iter].spair[max_solution].s_revenue > S_best.s_revenue)
            {
                S_best=S_array[iter].spair[max_solution];
            }
        }
    }

    //second pass
    for(int e=0;e<node_num;e++)
    {
        //foreach S^gamma
        for(int iter=min_gamma_index_in_C;iter<S_array.size();iter++)
        {
            for(auto &S_g:S_array[iter].spair)
            {
                //c(S \cup e)>B
                if(!S_g.budget_feasible(e,B))
                    continue;
                //judge if e\in S^gamma
                if(S_g.solution.count(e))
                    continue;

                oracle_times++;
                double marginal=S_g.marginal_gain(e);
                double f_S_g_and_e=marginal+S_g.s_revenue;
                if(f_S_g_and_e>S_best.s_revenue)
                {
                    S_best=S_g;
                    S_best.add_element(marginal,e);
                }
            }
        }
    }


    cout<<"S*:"<<endl;
    cout << "  utility: " << S_best.s_revenue << " cost: " << S_best.s_cost << " size: " << S_best.solution.size() << endl;
    int violation=0;
    cout<<" number of fairness violation: "<<endl;
    for(int j=0;j<S_best.group_exist.size();j++)
    {
        int err=max(S_best.group_exist[j]-group_upper[j],group_lower[j]-S_best.group_exist[j]);
        err=max(err,0);
        violation+=err;
        cout<<err<<" ";
    }
    cout<<endl;
    cout<<"sum violation: "<<violation<<endl;
    cout<<"SmkStream ---------end--------- "<<endl<<endl;
    return Result(S_best.s_revenue, S_best.s_cost, S_best.solution.size(), oracle_times, violation);

}
#endif //COVERAGE_SMKSTREAM_H
