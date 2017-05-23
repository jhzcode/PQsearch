#pragma once 
#include<iostream>
#include<fstream>
#include<vector>  
#include<map>  
#include<cstdlib>  
#include<algorithm>  
#include<fstream>  
#include"k_means.h" 
#include<string>   
using namespace std;
template<typename T>
class index
{
private:
	vector <vector <T>> dataSet;     //�洢ԭʼ���ݣ����Ҽ�¼��������ÿ������������
	void groupq();        //�������� 
public:
	vector<int>  coarse_class;     //��¼ÿ��������Ӧ�Ĵֲ���
	index(vector <vector <T>> data,int k1,int k2,int m);
	index() {};
	void buildindex();
	void save_index(string filename);
	class mNode
	{
	public :
		int ID; //the ID of each node  
		vector<int> group;//  ÿ����mά���� 
	};
	vector<vector <mNode>> inverted_list;         //���������ļ�  ��¼ѵ�������id+��������� 
	vector<vector <T>>   cluster;                 //������
	vector<vector<vector <T>>>  pq_centroids;     //ϸ����mά     
	int k1 ,k2;         //��ϸ���������� 
	int m ;   //��m��
};
template<typename T>
index<T>::index(vector <vector <T>>  data,int k1,int k2,int m)
{
	this->dataSet = data;
	this->k1 = k1;
	this->k2 = k2;
	this->m = m;
};
template<typename T>
void index<T>::buildindex()
{
	inverted_list.resize(k1);
	coarse_class.resize(dataSet.size());
	KMEANS<T> kms(k1, dataSet);
	kms.randCent();     //�������k���������� 
	kms.kmeans(cluster, coarse_class);       //������
	//�洢����
	for (int i = 0; i < dataSet.size(); i++)
	{
		for (int j = 0; j < dataSet[0].size(); j++)  dataSet[i].at(j)-=cluster[coarse_class[i]].at(j);
	}
	groupq();
    coarse_class.swap(vector<int>());
};

template<typename T>
void index<T>::groupq()
{
	vector<mNode> node;
	node.resize(dataSet.size());
	vector<vector <T>>   pqcluster;
	vector<int> pq_class;
	pq_class.resize(dataSet.size());
	vector <vector<T>> pq_data;    // ѵ������������
	for (int i = 0; i < m; i++)
	{
		vector <T> x;
		pq_data.swap(vector <vector<T>>());
		pqcluster.swap(vector <vector<T>>());
		for (int j = 0; j < dataSet.size(); j++)
		{
			x.swap(vector <T>());
			for (int t = i*dataSet[0].size() / m; t < (i + 1)*dataSet[0].size() / m; t++) x.push_back(dataSet[j].at(t));
			pq_data.push_back(x);
		}
		KMEANS<T> kms(k2, pq_data);
		kms.randCent();     //�������k���������� 
	    kms.kmeans(pqcluster,pq_class);
		pq_centroids.push_back(pqcluster);
		for (int j = 0; j < dataSet.size(); j++)
		{
			node[j].ID = j;
			node[j].group.push_back(pq_class[j]);
		}
	}
	for (int j=0; j<dataSet.size(); j++) inverted_list[coarse_class[j]].push_back(node[j]);
};

template<typename T>
void index<T>::save_index(string filename)
{
	ofstream outfile;
	outfile.open(filename);
	outfile << dataSet[0].size()<<" "<<m << " " << k1 << " " << k2 ;
	for (int i = 0; i < k1; i++)
	{
		outfile << " " << inverted_list[i].size();
		for (int j = 0; j < inverted_list[i].size(); j++)
		{
			outfile << " " << inverted_list[i].at(j).ID ;
			for (int k = 0; k<m; k++) outfile << " " << inverted_list[i].at(j).group[k];
		}
	}
	for (int i = 0; i < k1; i++)
	{
		for (int j = 0; j <dataSet[0].size(); j++)
		{
			outfile << " " << cluster[i].at(j);
		}
	}
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < k2; j++)
		{
			for (int k = 0; k<dataSet[0].size()/m; k++) outfile << " " << pq_centroids[i].at(j).at(k);
		}
	}
};
