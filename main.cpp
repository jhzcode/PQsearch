#include<iostream> 
#include<vector> 
#include<string> 
#include <Windows.h>  
#include "load_file.h"
#include "index.h"
#include"IVFADC_index.h"
using namespace std;  
int main()
{
	//这段注释代码为通过训练集产生索引结构，时间较长,已将运行过的结构保存成了文件，直接从文件读取即可
	/*vector<vector<float>> dataset;
	int k1, k2, m;
	load_from_file(dataset, "siftsmall_base.fvecs");
	cout <<"请输入粗糙分类个数，分组数,分组聚类个数。每一项均为2的整数次幂" << endl;
	cin >> k1 >> m >> k2;
	index<float> dataSet(dataset,k1,k2,m);
	dataset.swap(vector<vector<float>>());  // 释放内存空间
	dataSet.buildindex();
	dataSet.save_index("dataindex4.txt");*/
	long long start, finish;
	long long total;
	string filename = "dataindex3.txt";    //对应参数为k1=1024,m=8,k3=256
	//string filename = "dataindex1.txt";  //对应参数为k1=1024,m=4,k3=256
	//string filename = "dataindex2.txt";  //对应参数为k1=256,m=8,k3=256
	vector<vector<float>> query;
	vector<vector <int >> result;
	index<float> data;
	load_index_file(data, filename);                        //从filename对应文件中读取索引结构
	load_from_file(query, "siftsmall_query.fvecs");
	IVFADC_index<float> IVF(data,query);
	/*获取搜索时间*/
	start = GetTickCount();
	result=IVF.PQsearch(16);//搜索接口 w=16
	finish = GetTickCount();
	total = finish - start;
	cout << total <<"ms"<< endl;
	//下面的代码为计算recall@R
	float count[100];
	vector <vector <int>> correct;
	load_from_file(correct, "siftsmall_groundtruth.ivecs");
	for (int i = 0; i < query.size(); i++)
	{
		count[i] = 0;
		for (int R = 0; R < 100; j++)   //R值可通过改变变量R来实现
		{
				if (result[i].at(R) == correct[i].at(0))
				{
					count[i]++; break;
				}	
		}	
	}
	float sum = 0;
	for (int i = 0; i < 100;i++)
	{
		sum += count[i];
	}
	cout << sum / 100 << endl;
	//为让结果停留在控制台，用了getchar()
	getchar();
	return 0;
}
