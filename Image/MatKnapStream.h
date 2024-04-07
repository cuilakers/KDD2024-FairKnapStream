//
// Created by CLAKERS on 2024/1/23.
//

#ifndef COVERAGE_MATKNAPSTREAM_H
#define COVERAGE_MATKNAPSTREAM_H
#include "utilityfunction.h"
int solution_number;
bool Monotone=true;
double calculate_before_value(int node,const unordered_set<int> &S)
{
    vector<int> tempS;
    for(const auto &e:S)
    {
        if(e<node)
            tempS.emplace_back(e);
    }
    return u_marginal_for_S(node,tempS);
}
pair<int,double> Exchange(int group, const unordered_set<int> &S,long long int &oracle_times)
{
    int v_ix=-1;
    double min_value=999999999.0;
    for(const auto &e:S)
    {
        if(Groundset[e].group==group)
        {
            double e_marginal=calculate_before_value(e,S);
            oracle_times++;
            if(e_marginal<=min_value)
            {
                v_ix=e;
                min_value=e_marginal;
            }
        }
    }
    return make_pair(v_ix,min_value);
}
class nodeI
{
public:
    int x_index;
    bool E_x;
    int v_x;
    double marginal_gain;
    nodeI(){};
    nodeI(int x,bool E,double marginal,int v)
    {
        x_index=x;
        E_x=E;
        v_x=v;
        marginal_gain=marginal;
    };
    nodeI(int x,bool E,double marginal)
    {
        x_index=x;
        E_x=E;
        marginal_gain=marginal;
    };
};
class ALG_Lite
{
public:
    double tau;
    vector<S_class> S;
    ALG_Lite(const double t)
    {
        tau=t;
        for (int i = 0; i < solution_number; ++i) {
            S.emplace_back();
        }
    }
};
class ALG_Full
{
public:
    ALG_Full(){};
    vector<ALG_Lite> alg;
    double max_tau=0.0;//max tau in C, help to update tau
    int min_tau_index_in_C=-1;//-1 represent C is empty, else represent min tau index which is still available now
    int max_tau_index_in_C=-1;
};
S_class MatKnapStream(double eps,double B, long long int &oracle_times)
{


    //long long int oracle_times=0;
    //int node_num=data->index.size();

    double lambda,alpha;

    if(Monotone) {
        lambda = 3.0;
        alpha = 2.0;
        solution_number=1;
    }
    else {
        lambda = 5.0 + 2.0 * sqrt(2.0);
        alpha = 1.0 + sqrt(2.0);
        solution_number=2;
    }
    ALG_Full Gamma;

    S_class S_best;

//    double max_gamma=0.0;//max gamma in Gamma, help to update Gamma
//    int min_gamma_index_in_C=-1;//-1 represent Gamma is empty, else represent min gamma index which is still available now
//    int max_gamma_index_in_C=-1;

    int counter=0;

    //for(int u=node_num-1;u>=0;u--)
    for(int u=0;u<node_num;u++)
    {
//        if(counter%50000==0)
//            cout<<"Counter: "<<counter<<endl;
//        counter++;

        if(!budget_feasible_single(u,B))
            continue;

        //generate Gamma
        double fu_value=f_u(u);
        oracle_times++;
        if(fu_value>S_best.s_revenue)
        {
            S_best.replace_with_singleton(fu_value,u);
        }
        if(fu_value<=0) continue;


        double left_temp= S_best.s_revenue / (lambda * B * (1.0 + eps));
        double right_temp=fu_value/(Groundset[u].cost);
        if(left_temp>right_temp) continue;

        double left=ceil(log(left_temp)/log(1.0+eps));
        double right=floor(log(right_temp)/log(1.0+eps));

        /*******fix bug*******/
        left=max(left,1.0);
        right=max(right,1.0);
        /*******fix bug*******/
        if(Gamma.min_tau_index_in_C==-1)//first visit C
        {
            for(int t=left;t<=right;t++)
            {
                double tau_temp=pow(1.0+eps,t);
                Gamma.alg.push_back(ALG_Lite(tau_temp));
            }
            Gamma.min_tau_index_in_C=0;//the index of min tau which is still available now
            Gamma.max_tau=pow(1.0+eps,right);//now max tau
            Gamma.max_tau_index_in_C=Gamma.alg.size()-1;//max tau index in C now
        }
        else//not first visit
        {
            double now_min_tau_in_C=pow(1.0+eps,left);//min tau in C now
            double now_max_tau_in_C=pow(1.0+eps,right);//max tau in C now
            if(now_min_tau_in_C>Gamma.max_tau)//all old S_array should be removed
            {
                Gamma.min_tau_index_in_C=Gamma.alg.size();//the index of min tau which is still available now
                for(int t=left;t<=right;t++)//then add new S gamma pair
                {
                    double tau_temp=pow(1.0+eps,t);
                    Gamma.alg.push_back(ALG_Lite(tau_temp));
                }
                Gamma.max_tau_index_in_C=Gamma.alg.size()-1;//max gamma index in C now
                Gamma.max_tau = Gamma.alg.back().tau;//the last element always is the max_gamma anyway
            }
            else//else find where is the min gamma index now, which is equivalent to remove all S whose gamma < left
            {
                bool need_update= true;//judge S_array should be updated or not
                for (int z = Gamma.min_tau_index_in_C; z < Gamma.alg.size(); z++)
                {
                    if (Gamma.alg[z].tau < now_min_tau_in_C)
                        Gamma.min_tau_index_in_C++;
                    if (Gamma.alg[z].tau >= now_max_tau_in_C)
                    {
                        Gamma.max_tau_index_in_C=z;
                        need_update=false;
                        break;
                    }
                }
                if(need_update)
                {
                    //finally, go through all gamma now, put new pair (S,gamma) if needed
                    for (int t = left; t <= right; t++)
                    {
                        double tau_temp = pow(1.0+eps, t);
                        if (tau_temp > Gamma.max_tau)
                            Gamma.alg.push_back(ALG_Lite(tau_temp));
                    }
                    Gamma.max_tau_index_in_C=Gamma.alg.size()-1;//max gamma index in C now
                    Gamma.max_tau = Gamma.alg.back().tau;//the last element always is the max_gamma anyway
                }
            }
        }
        //foreach gamma\in Gamma
        for(int b=Gamma.min_tau_index_in_C;b<=Gamma.max_tau_index_in_C;b++)
        {
            vector<nodeI> I;
            for (int x = 0; x < solution_number; ++x)
            {
                double marginal_gain=Gamma.alg[b].S[x].marginal_gain(u);
                oracle_times++;
                if(marginal_gain<Gamma.alg[b].tau)
                    continue;

                if(Gamma.alg[b].S[x].fair_upper(u))
                {
                    if(Gamma.alg[b].S[x].budget_feasible(u,B))
                    {
                        I.emplace_back(x,true,marginal_gain);
                    }
                }
                else
                {
                    pair<int,double> vx=Exchange(Groundset[u].group,Gamma.alg[b].S[x].solution,oracle_times);
                    if(vx.first==-1)
                        continue;
                    if(marginal_gain>=alpha*vx.second) {
                        if (Gamma.alg[b].S[x].budget_feasible(u, B)) {
                            I.emplace_back(x, false, marginal_gain, vx.first);
                        }
                    }
                }
            }

            if(I.empty())
                continue;

            double max_marginal=-999999999.0;
            nodeI x_star;
            for(const auto &x:I)
            {
                if(x.marginal_gain>=max_marginal)
                {
                    max_marginal=x.marginal_gain;
                    x_star=x;
                }
            }
            if(x_star.E_x)
            {
                Gamma.alg[b].S[x_star.x_index].add_element(x_star.marginal_gain,u);
            }
            else
            {
                Gamma.alg[b].S[x_star.x_index].deleta_add_element(u,x_star.v_x);
            }

            if(Gamma.alg[b].S[x_star.x_index].s_revenue > S_best.s_revenue)
            {
                S_best=Gamma.alg[b].S[x_star.x_index];
            }

        }

    }

//    cout<<"MatKnapStream & Budget: "<<B<<endl;
//    cout<<"S*:"<<endl;
//    cout << "  revenue: " << S_best.s_revenue << " cost: " << S_best.s_cost << " size: " << S_best.solution.size() << endl;
//    for(const auto &p:S_best.solution)
//        cout<<p<<" ";
//    cout<<endl;
//    cout<<" group limit: "<<endl;
//    for(int j=0;j<S_best.group_exist.size();j++)
//    {
//        cout<<S_best.group_exist[j]<<" ";
//    }
//    cout<<endl;

    return S_best;
}


#endif //COVERAGE_MATKNAPSTREAM_H
