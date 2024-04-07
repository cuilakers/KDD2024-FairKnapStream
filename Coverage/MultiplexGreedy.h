//
// Created by CLAKERS on 2024/1/26.
//

#ifndef COVERAGE_MULTIPLEXGREEDY_H
#define COVERAGE_MULTIPLEXGREEDY_H
#include "utilityfunction.h"
class ALG2
{
public:
    double tau;
    vector<vector<S_class>> S;
    ALG2(const double t)
    {
        tau=t;
    }
};
class ALG3
{
public:
    double gamma;
    vector<ALG2> alg3;
    double max_tau=0.0;//max tau in C, help to update tau
    int min_tau_index_in_C=-1;//-1 represent C is empty, else represent min tau index which is still available now
    int max_tau_index_in_C=-1;
    ALG3(const double g)
    {
        gamma=g;
    }
};
Result MultiplexGreedy(double B,double eps)
{
    double eps_cs=0.1;

    cout<<"MultiplexGreedy ---------start--------- "<<endl;
    //int max_movie=999999999;//+inf
    long long int oracle_times=0;

    //old setting
    //int h=ceill(  1 + sqrt(  2.0*k+1.0+ (2*d+k-1.0/4.0) / (6.0+2*log2(k))   )  );
    //new setting
    int h=2;
    int k=1;
    //old lambda
    //double lambda=(  2.0*(1.0+eps)*(1.0-1.0/(double)h)   )   /  (  (2.0*k+h)*(6.0+2*log2(k*node_num) )-k+1.0/4.0+2.0*(1.0+eps)*d   );
    //new lambda
    double lambda=(  3.0*(1.0-1.0/(double)h)   )   /  (  (2.0*k+h)*(3.0+2*log2(k*node_num) )-k+1.0+3.0*d   );


    vector<ALG3> Gamma;
    S_class S_best;

    double max_gamma=0.0;//max gamma in Gamma, help to update Gamma
    int min_gamma_index_in_C=-1;//-1 represent Gamma is empty, else represent min gamma index which is still available now
    int max_gamma_index_in_C=-1;

    S_class G;
    pair<int,double> M=make_pair(-1,0.0);
    int counter=0;
    //for(int u=node_num-1;u>=0;u--)
    for(int u=0;u<node_num;u++)
    {
        if(counter%100==0)
            cout<<"Counter: "<<counter<<endl;
        counter++;
        //a new element comes, record the time of beginning
        //chrono_timer.reset();

        //judge G+{u}\in I or not
        bool flag=G.fair_upper(u);

        //if G+{u}\in I
        if(flag) {
            G.add_element_without_marginal(u);
        }

        //cout<<"node: "<<u<<" G.size: "<<G.Set.size()<<endl;
        //node feasible ?
        if(!budget_feasible_single(u,B))
            continue;
        //Every single element satisfies u\in I, so there is no need to judge

        //generate ALG3
        double fu_value=f_u(u);
        if(fu_value>M.second)
        {
            M.first=u;
            M.second=fu_value;
        }

        oracle_times++;

        if(fu_value>S_best.s_revenue)
        {
            S_best.replace_with_singleton(fu_value,u);
        }
        if(fu_value<=0) continue;
        double left_temp= (lambda*S_best.s_revenue) / (B * (1.0 + eps_cs));

//        double u_cost_sum=0.0;
//        for(int r=0;r<d;r++)
//            u_cost_sum+=data->costs[r][u];

        double right_temp=fu_value/(Groundset[u].cost);
        if(left_temp>right_temp) continue;

        double left=ceil(log(left_temp)/log(1.0+eps_cs));
        double right=floor(log(right_temp)/log(1.0+eps_cs));

        /*******fix bug*******/
        left=max(left,1.0);
        right=max(right,1.0);
        /*******fix bug*******/

        if(min_gamma_index_in_C==-1)//first visit Gamma
        {
            for(int t=left;t<=right;t++)
            {
                double gamma_temp=pow(1.0+eps_cs,t);
                Gamma.push_back(ALG3(gamma_temp));
            }
            min_gamma_index_in_C=0;//the index of min gamma which is still available now
            max_gamma=pow(1.0+eps_cs,right);//now max gamma
            max_gamma_index_in_C=Gamma.size()-1;//max gamma index in C now
        }
        else//not first visit
        {
            double now_min_gamma_in_C=pow(1.0+eps_cs,left);//min gamma in C now
            double now_max_gamma_in_C=pow(1.0+eps_cs,right);//max gamma in C now
            if(now_min_gamma_in_C>max_gamma)//all old S_array should be removed
            {
                min_gamma_index_in_C=Gamma.size();//the index of min gamma which is still available now
                for(int t=left;t<=right;t++)//then add new S gamma pair
                {
                    double gamma_temp=pow(1.0+eps_cs,t);
                    Gamma.push_back(ALG3(gamma_temp));
                }
                max_gamma_index_in_C=Gamma.size()-1;//max gamma index in C now
                max_gamma = Gamma[Gamma.size() - 1].gamma;//the last element always is the max_gamma anyway
            }
            else//else find where is the min gamma index now, which is equivalent to remove all S whose gamma < left
            {
                bool need_update= true;//judge S_array should be updated or not
                for (int iter = min_gamma_index_in_C; iter < Gamma.size(); iter++)
                {
                    if (Gamma[iter].gamma < now_min_gamma_in_C)
                        min_gamma_index_in_C++;
                    if (Gamma[iter].gamma >= now_max_gamma_in_C)
                    {
                        max_gamma_index_in_C=iter;
                        need_update=false;
                        break;
                    }
                }
                if(need_update)
                {
                    //finally, go through all gamma now, put new pair (S,gamma) if needed
                    for (int t = left; t <= right; t++)
                    {
                        double gamma_temp = pow(1.0 + eps_cs, t);
                        if (gamma_temp > max_gamma)
                            Gamma.push_back(ALG3(gamma_temp));
                    }
                    max_gamma = Gamma[Gamma.size() - 1].gamma;//the last element always is the max_gamma anyway
                    max_gamma_index_in_C=Gamma.size() - 1;
                }
            }
        }
        //generate ALG2
        for(int iter=min_gamma_index_in_C;iter<=max_gamma_index_in_C;iter++)
        {
            double left_temp=M.second;
            //double right_temp=M.second*pow(2.0,4.0+2.0*log2(k)+1.0+2.0*log2(G.Set.size()));
            double right_temp=M.second*pow(2.0,3.0+2.0*log2(k)+2.0*log2(G.solution.size()));

            if(left_temp>right_temp) continue;

            double left=ceil(log(left_temp)/log(2.0));
            double right=floor(log(right_temp)/log(2.0));

            /*******fix bug*******/
            left=max(left,1.0);
            right=max(right,1.0);
            /*******fix bug*******/

            if(Gamma[iter].min_tau_index_in_C==-1)//first visit C
            {
                for(int t=left;t<=right;t++)
                {
                    double tau_temp=pow(2.0,t);
                    Gamma[iter].alg3.push_back(ALG2(tau_temp));
                }
                Gamma[iter].min_tau_index_in_C=0;//the index of min tau which is still available now
                Gamma[iter].max_tau=pow(2.0,right);//now max tau
                Gamma[iter].max_tau_index_in_C=Gamma[iter].alg3.size()-1;//max tau index in C now
            }
            else//not first visit
            {
                double now_min_tau_in_C=pow(2.0,left);//min tau in C now
                double now_max_tau_in_C=pow(2.0,right);//max tau in C now
                if(now_min_tau_in_C>Gamma[iter].max_tau)//all old S_array should be removed
                {
                    Gamma[iter].min_tau_index_in_C=Gamma[iter].alg3.size();//the index of min tau which is still available now
                    for(int t=left;t<=right;t++)//then add new S gamma pair
                    {
                        double tau_temp=pow(2.0,t);
                        Gamma[iter].alg3.push_back(ALG2(tau_temp));
                    }
                    Gamma[iter].max_tau_index_in_C=Gamma[iter].alg3.size()-1;//max gamma index in C now
                    Gamma[iter].max_tau = Gamma[iter].alg3.back().tau;//the last element always is the max_gamma anyway
                }
                else//else find where is the min gamma index now, which is equivalent to remove all S whose gamma < left
                {
                    bool need_update= true;//judge S_array should be updated or not
                    for (int z = Gamma[iter].min_tau_index_in_C; z < Gamma[iter].alg3.size(); z++)
                    {
                        if (Gamma[iter].alg3[z].tau < now_min_tau_in_C)
                            Gamma[iter].min_tau_index_in_C++;
                        if (Gamma[iter].alg3[z].tau >= now_max_tau_in_C)
                        {
                            Gamma[iter].max_tau_index_in_C=z;
                            need_update=false;
                            break;
                        }
                    }
                    if(need_update)
                    {
                        //finally, go through all gamma now, put new pair (S,gamma) if needed
                        for (int t = left; t <= right; t++)
                        {
                            double tau_temp = pow(2.0, t);
                            if (tau_temp > Gamma[iter].max_tau)
                                Gamma[iter].alg3.push_back(ALG2(tau_temp));
                        }
                        Gamma[iter].max_tau_index_in_C=Gamma[iter].alg3.size()-1;//max gamma index in C now
                        Gamma[iter].max_tau = Gamma[iter].alg3.back().tau;//the last element always is the max_gamma anyway
                    }
                }
            }
        }

        /****** The running time of each copy is timed separately,
         * and the one that takes the longest is taken. *********/

        for(int a=min_gamma_index_in_C;a<=max_gamma_index_in_C;a++)
        {
            for(int b=Gamma[a].min_tau_index_in_C;b<=Gamma[a].max_tau_index_in_C;b++)
            {

                //int ell=ceil(4.0+2.0*log2(k)+1.0+2.0*log2(G.Set.size()));
                int ell=ceil(3.0+2.0*log2(k)+2.0*log2(G.solution.size()));
                if(ell>Gamma[a].alg3[b].S.size())
                {
                    for(int i=Gamma[a].alg3[b].S.size();i<ell;i++)
                    {
                        Gamma[a].alg3[b].S.push_back(vector<S_class>());
                        for(int j=0;j<h;j++)
                        {
                            Gamma[a].alg3[b].S[i].push_back(S_class());
                        }
                    }
                }
                for(int i=0;i<ell;i++)
                {
                    vector<int> J;
                    for(int j=0;j<h;j++)
                    {
                        ////////////////
                        double cup_mariginal=0.0;
                        /****check every end_vertex v in N(u) whether has been covered by any solution******/
                        for(const auto &v:Edge[u])
                        {
                            bool is_covered=false;
                            for(int x=0;x<i;x++)
                            {
                                if(Gamma[a].alg3[b].S[x][j].node_covered.count(v))
                                {
                                    is_covered=true;
                                    break;
                                }
                            }
                            //if not covered, then we can sum weight as marginal gain
                            if(!is_covered)
                                cup_mariginal+=Groundset[v].weight;
                        }
                        ///////////////
                        if(cup_mariginal>= (  Gamma[a].alg3[b].tau/pow(2.0,i-1.0)  )  )
                        {
                            J.push_back(j);
                        }
                        oracle_times++;
                    }

                    int y=-1;
                    double max_marginal=-999999999.0;
                    for(auto j:J)
                    {
                        bool flag=Gamma[a].alg3[b].S[i][j].fair_upper(u);
                        //if S+{e}\in I
                        if (flag)
                        {
                            oracle_times++;
                            double marginal = Gamma[a].alg3[b].S[i][j].marginal_gain(u);
                            if(marginal>max_marginal)
                            {
                                max_marginal=marginal;
                                y=j;
                            }
                        }
                    }
                    if(y!=-1)
                    {
                        //budget constraints
//                        double u_cost_sum=0.0;
//                        bool feasible=true;
//                        for(int r=0;r<d;r++)
//                        {
//                            if(Gamma[a].alg3[b].S[i][y].s_cost[r]+data->costs[r][u]>B)
//                                feasible=false;
//                            u_cost_sum+=data->costs[r][u];
//                        }

                        //gamma constraints
                        if((max_marginal/Groundset[u].cost)>=Gamma[a].gamma&&Gamma[a].alg3[b].S[i][y].budget_feasible(u,B))
                        {
                            Gamma[a].alg3[b].S[i][y].add_element(max_marginal,u);

                            if(Gamma[a].alg3[b].S[i][y].s_revenue > S_best.s_revenue)
                            {
                                S_best=Gamma[a].alg3[b].S[i][y];
                            }
                        }
                    }
                    if(!J.empty())
                        break;
                }

                // long long temp_time=chrono_timer.elapsed_nano();
                // if(temp_time>max_subroutine_times)
                //     max_subroutine_times=temp_time;
                //cout<<"temp_time "<<temp_time<<endl;
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
    cout<<"MultiplexGreedy ---------end--------- "<<endl<<endl;
    return Result(S_best.s_revenue, S_best.s_cost, S_best.solution.size(), oracle_times, violation);

}
#endif //COVERAGE_MULTIPLEXGREEDY_H
