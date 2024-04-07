#include "generate_data.h"
#include "utilityfunction.h"
#include "time.h"
#include "FairKnapStream.h"
#include "SmkStream.h"
#include "StreamingHeuristic.h"
#include "MultiplexGreedy.h"
int main(int argc,char *argv[]) {

    //generate_data_with_lable();
    //return 0;
    //generate_data();
    string::size_type pos1, pos2,posend;
    pos1=cost_text.find_last_of("/");
    pos2=cost_text.rfind("/",pos1-1);

    //posend=cost_text.find_last_not_of("/");
    string name1=cost_text.substr(pos2+1,pos1-pos2-1);
    //string name2=cost_text.substr(pos1+1,posend);
    //string result_name=name1+"_"+name2;
    string result_name="engine_"+name1;

    read();
    cal_similarity();


    time_t nowtime;
    struct tm* p;;
    time(&nowtime);
    p = localtime(&nowtime);
    string outtext="./result/image_result_normalize"+to_string((int)ave_num)+"_"+result_name+"_"+to_string(p->tm_mon+1)+"."+to_string(p->tm_mday)+"_"+to_string(p->tm_hour)+"_"+to_string(p->tm_min)+"_"+to_string(p->tm_sec)+".txt";

    vector<Result> heuristic_result;
    vector<Result> heuristic_random_result;
    vector<Result> fairknapstream_result;
    vector<Result> multiplexgreedy_result;
    vector<Result> smkstream_result;



    double eps=0.1;

//    vector<int> upper_temp={50,50,50};
//    vector<int> lower_temp={15,25,35};


    double B_start=10.0;
    double B_end=100.0;
    double B_step=10.0;

    for(double B=B_start;B<=B_end;B+=B_step)
    {
        //cout<<"Group\tupper bound\tlower bond"<<endl;
        for (int i = 0; i < group_num; ++i) {
            double upper_temp=ceil(1.2*B*(double)group_node_num[i]/node_num);
            double lower_temp=ceil(0.8*B*(double)group_node_num[i]/node_num);
            group_upper[i]=upper_temp;
            group_lower[i]=lower_temp;

         //   cout<<i<<"\t"<<upper_temp<<"\t"<<lower_temp<<endl;
        }


        fairknapstream_result.emplace_back(FairKnapStream(eps,B));
        heuristic_random_result.emplace_back(StreamingHeuristicRandom(eps,B));
        multiplexgreedy_result.emplace_back(MultiplexGreedy(B,eps));
        smkstream_result.emplace_back(SmkStream(B,eps));
    }

    ofstream out(outtext);
    out<<"eps: "<<eps<<endl;
    out<<"Budget: "<<endl;
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
