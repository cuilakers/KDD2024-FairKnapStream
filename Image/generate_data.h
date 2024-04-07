//
// Created by CLAKERS on 2020/8/22.
//

#ifndef IMAGE_GENERATE_DATA_H
#define IMAGE_GENERATE_DATA_H
#include <iostream>
#include "fstream"
#include "vector"
#include "random"
#include "set"
using namespace std;
void generate_data_with_lable()
{
    vector<vector<int>> feature;
    vector<int> lable;
    int n=10000;
    int m=3072;
    feature.resize(n, vector<int>(m));

    //vector<vector<double>> similarity;
    //similarity.resize(n, vector<double>(n));

    //ifstream in1("feature.txt");
    set<int> random_num;
    ifstream in1("cifar.txt");
    int i=0;
    int templable=-1;
    while(!in1.eof())
    {
        in1>>templable;
        if (in1.fail())
            break;
        lable.push_back(templable);
        for(int j=0;j<m;j++)
            in1>>feature[i][j];

        //if(templable<3)
        //    random_num.insert(i);

        i++;
    }
    in1.close();
    cout<<"feature Lines: "<<i<<endl;

    vector<double> contrast(n,0.0);
    ifstream in2("contrast.txt");
    i=0;
    while(!in2.eof())
    {
        in2>>contrast[i];
        if (in2.fail())
            break;
        i++;
    }
    in2.close();

    default_random_engine e(2);
    uniform_int_distribution<int> u(0, 9999);


    //* sample 600 images
    int rn;
    while (random_num.size() < 300)
    {
        rn = u(e);
        if(lable[rn]<3)
            random_num.insert(rn);
    }
    //*/
    ofstream out1("rd_feature.txt");
    ofstream out2("rd_contrast.txt");
    for(auto iter:random_num)
    {
        out1 << lable[iter] << "\t";
        for(int k=0;k<feature[iter].size();k++) {
            out1 << feature[iter][k] << "\t";
        }
        out1<<endl;
        out2<<contrast[iter]<<endl;
    }
    out1.close();
    out2.close();

    /*
    cout<<feature[0][3071]<<endl;
    cout<<feature[1][0]<<endl;
    cout<<feature[9998][0]<<endl;
    cout<<feature[9999][0]<<endl;
    cout<<feature[9998][3071]<<endl;
    cout<<feature[9999][3071]<<endl;
*/
}
void generate_data()
{
    vector<vector<int>> feature;
    int n=10000;
    int m=3072;
    feature.resize(n, vector<int>(m));

    vector<vector<double>> similarity;
    //similarity.resize(n, vector<double>(n));

    ifstream in1("feature.txt");
    int i=0;
    while(!in1.eof())
    {
        for(int j=0;j<m;j++)
            in1>>feature[i][j];
        if (in1.fail())
            break;
        i++;
    }
    in1.close();

    vector<double> contrast(n,0.0);
    ifstream in2("contrast.txt");
    i=0;
    while(!in2.eof())
    {
        in2>>contrast[i];
        if (in2.fail())
            break;
        i++;
    }
    in2.close();

    default_random_engine e(2);
    uniform_int_distribution<int> u(0, 9999);
    set<int> random_num;
    int rn;
    while (random_num.size() < 500)
    {
        rn = u(e);
        random_num.insert(rn);
    }
    ofstream out1("rd_feature.txt");
    ofstream out2("rd_contrast.txt");
    for(auto iter:random_num)
    {
        for(int k=0;k<feature[iter].size();k++)
            out1<<feature[iter][k]<<"\t";
        out1<<endl;
        out2<<contrast[iter]<<endl;
    }
    out1.close();
    out2.close();

    /*
    cout<<feature[0][3071]<<endl;
    cout<<feature[1][0]<<endl;
    cout<<feature[9998][0]<<endl;
    cout<<feature[9999][0]<<endl;
    cout<<feature[9998][3071]<<endl;
    cout<<feature[9999][3071]<<endl;
*/
}

#endif //IMAGE_GENERATE_DATA_H
