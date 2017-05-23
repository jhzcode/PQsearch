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
	//���ע�ʹ���Ϊͨ��ѵ�������������ṹ��ʱ��ϳ�,�ѽ����й��Ľṹ��������ļ���ֱ�Ӵ��ļ���ȡ����
	/*vector<vector<float>> dataset;
	int k1, k2, m;
	load_from_file(dataset, "siftsmall_base.fvecs");
	cout <<"������ֲڷ��������������,������������ÿһ���Ϊ2����������" << endl;
	cin >> k1 >> m >> k2;
	index<float> dataSet(dataset,k1,k2,m);
	dataset.swap(vector<vector<float>>());  // �ͷ��ڴ�ռ�
	dataSet.buildindex();
	dataSet.save_index("dataindex4.txt");*/
	long long start, finish;
	long long total;
	string filename = "dataindex3.txt";    //��Ӧ����Ϊk1=1024,m=8,k3=256
	//string filename = "dataindex1.txt";  //��Ӧ����Ϊk1=1024,m=4,k3=256
	//string filename = "dataindex2.txt";  //��Ӧ����Ϊk1=256,m=8,k3=256
	vector<vector<float>> query;
	vector<vector <int >> result;
	index<float> data;
	load_index_file(data, filename);                        //��filename��Ӧ�ļ��ж�ȡ�����ṹ
	load_from_file(query, "siftsmall_query.fvecs");
	IVFADC_index<float> IVF(data,query);
	/*��ȡ����ʱ��*/
	start = GetTickCount();
	result=IVF.PQsearch(16);//�����ӿ� w=16
	finish = GetTickCount();
	total = finish - start;
	cout << total <<"ms"<< endl;
	//����Ĵ���Ϊ����recall@R
	float count[100];
	vector <vector <int>> correct;
	load_from_file(correct, "siftsmall_groundtruth.ivecs");
	for (int i = 0; i < query.size(); i++)
	{
		count[i] = 0;
		for (int R = 0; R < 100; j++)   //Rֵ��ͨ���ı����R��ʵ��
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
	//Ϊ�ý��ͣ���ڿ���̨������getchar()
	getchar();
	return 0;
}
