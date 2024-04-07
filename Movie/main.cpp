#include "utilityfunction.h"
#include <stdlib.h>
#include "time.h"
#include "FairKnapStream.h"
#include "SmkStream.h"
#include "StreamingHeuristic.h"
#include "MultiplexGreedy.h"
int main(int argc,char *argv[]) {

    //double B=atof(argv[1]);
    //double eps=atof(argv[2]);
//    double eps=atof(argv[1]);
//   / cout<<"eps: "<<eps<<endl;

    string filename = "movies_1793.txt";
    dataset *da = new dataset(filename);

    time_t nowtime;
    struct tm* p;;
    time(&nowtime);
    p = localtime(&nowtime);
    string outtext="./result/movie_result_"+to_string((int)ave_num)+"_"+to_string(p->tm_mon+1)+"."+to_string(p->tm_mday)+"_"+to_string(p->tm_hour)+"_"+to_string(p->tm_min)+"_"+to_string(p->tm_sec)+".txt";

    lambda_f=1.0;

    //test value
   // cout<<f_u(1468,0)<<endl;
    //cout<<da->costs[0][1468]<<endl;
   // cout<<da->costs[1][1468]<<endl;
    /*
    S_class t1;
    t1.solution={1161, 1159 ,979, 883 ,797, 772 ,750, 315, 291, 265 ,190, 189, 185 ,177 ,176 ,173 ,172 ,169};
    cout<<t1.f_S()<<endl;
    double sum_cost1=0.0;
    for(int i=0;i<t1.solution.size();i++)
        sum_cost1+=da->costs[0][t1.solution[i]];
    double sum_cost2=0.0;
    for(int i=0;i<t1.solution.size();i++)
        sum_cost2+=da->costs[1][t1.solution[i]];
    cout<<sum_cost1<<"\t"<<sum_cost2<<endl;

//    S_class t2;
//    t2.solution={50, 324, 772};
//    cout<<t2.f_S()+t2.marginal( 316)<<endl;

//*/

    //test
    /*
    int node_num=da->index.size();
    cout<<"f(u)"<<endl;
    double sum_density=0.0;
    for(int iter=0;iter<node_num;iter++)
    {
        cout<<f_u(iter,node_num)<<endl;
    }
    cout<<"f(u)/cost"<<endl;
    for(int iter=0;iter<node_num;iter++)
    {
        cout<<f_u(iter,node_num)/da->rate_costs[iter]<<endl;
        sum_density+=f_u(iter,node_num)/da->rate_costs[iter];
    }
    sum_density/=node_num;
    cout<<"ave density:"<<sum_density<<endl;
    //*/

    vector<Result> heuristic_result;
    vector<Result> heuristic_random_result;
    vector<Result> fairknapstream_result;
    vector<Result> multiplexgreedy_result;
    vector<Result> smkstream_result;



    /////experiments for d-knapsack
    /*
    for(double B=B_start;B<=B_end;B+=B_step)
    {
        dknapsack_result.push_back(DKnapsack(da,B,eps));
        onepass_result.push_back(onepass(da,B,eps));
        twopass_result.push_back(twopass(da,B,eps));

       //ksystemdknapsack_result.push_back(KsystemDknapsack(da,B,eps));
       //densitysearchsgs_result.push_back(DensitySearchSgs(eps,da,B));
        //multirandom_result.push_back(multi_random(eps,da,m));

        /*** d_knapsack but run for k-system+d-knapsack ***/
       // dknapsack_k_result.push_back(DKnapsack_K(da,B,eps));
    //}
    //*/

    /////experiments for k-system+d-knapsack
    //*
//    vector<int> t={7,2,3,4,5};
//    unordered_set<int> tt={7,2,3,4,5};
//    cout<<f_S(t)<<endl;
//    cout<<f_S(tt);

//    vector<int> upper_temp={30,40,50};
//    vector<int> lower_temp={5,10,15};
//    group_upper=upper_temp;
//    group_lower=lower_temp;

    group_upper.resize(group_num);
    group_lower.resize(group_num);

    double B_start=10.0;
    double B_end=100.0;
    double B_step=10.0;

    double eps=0.1;
    //for(genres_limit=1;genres_limit<=10;genres_limit++)
    for(double B=B_start;B<=B_end;B+=B_step)
    {
        //cout<<"Group\tupper bound\tlower bound"<<endl;
        for (int i = 0; i < group_num; ++i) {
            double upper_temp=ceil(1.2*B*(double)group_node_num[i]/node_num);
            double lower_temp=ceil(0.8*B*(double)group_node_num[i]/node_num);
            group_upper[i]=upper_temp;
            group_lower[i]=lower_temp;

            //cout<<i<<"\t"<<upper_temp<<"\t"<<lower_temp<<endl;
        }


        fairknapstream_result.emplace_back(FairKnapStream(eps,B));
        heuristic_random_result.emplace_back(StreamingHeuristicRandom(eps,B));
        multiplexgreedy_result.emplace_back(MultiplexGreedy(B,eps));
        smkstream_result.emplace_back(SmkStream(B,eps));
    }//*/

    ofstream out(outtext);
//    if(B_wrong)
//        out<<"Budget is wrong !"<<endl;
    out<<"eps: "<<eps<<" lambda_f:"<<lambda_f<<" lambda:"<<da->lambda<<endl;
    out<<"B: "<<endl;
    for(double B=B_start;B<=B_end;B+=B_step)
    {
        out<<B<<"\t";
    }
    out<<endl;

    out<<"FairKnapStream "<<endl;
    out<<"utility: "<<endl;
    for(auto p:fairknapstream_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"number of violation: "<<endl;
    for(auto p:fairknapstream_result)
    {
        out<<p.violation_num<<"\t";
    }
    out<<endl;

    out<<"RandomStream "<<endl;
    out<<"utility: "<<endl;
    for(auto p:heuristic_random_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"number of violation: "<<endl;
    for(auto p:heuristic_random_result)
    {
        out<<p.violation_num<<"\t";
    }
    out<<endl;


    out<<"MultiplexGreedy "<<endl;
    out<<"utility: "<<endl;
    for(auto p:multiplexgreedy_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"number of violation: "<<endl;
    for(auto p:multiplexgreedy_result)
    {
        out<<p.violation_num<<"\t";
    }
    out<<endl;

    out<<"SmkStream "<<endl;
    out<<"utility: "<<endl;
    for(auto p:smkstream_result)
    {
        out<<p.revenue<<"\t";
    }
    out<<endl;
    out<<"number of violation: "<<endl;
    for(auto p:smkstream_result)
    {
        out<<p.violation_num<<"\t";
    }
    out<<endl;


    return 0;
}
