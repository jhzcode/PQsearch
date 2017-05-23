#include<stdio.h>
#include<stdlib.h>
#include<iostream> 
#include<fstream>
#include"index.h"

using namespace std;
template<typename T>
void load_from_file(vector<vector<T>> &data, string filename)
{
	ifstream fin;
	fin.open(filename, ios::binary);
	if (!fin.good()) {
		cout << "file bad!\n";
		return ;
	}
	int vcc; int vrr;
	fin.read((char*)&vcc, 4);//get dimension.
	cout << "dimension=" << vcc << endl;
	fin.seekg(0, ios::end);
	size_t size = fin.tellg();
	cout << "size=" << size << endl;
	fin.seekg(0, ios::beg);
	if (size % (4 + vcc * 4) != 0) {
		cout << "file size error!\n";
		return ;
	}
	vrr = size / (4 + vcc * 4);
	cout << "vecNum=" << vrr << endl;
	//because file is big,and speed is import,so read one tenth once a time.
	int m = 20;//needs m|100,0000
	int num = vrr / m;//actually read 10,0000 vec once a time.
	size_t nbytes = (4 + vcc * 4)*num;
	char *buffer = new char[nbytes];
	int i = 0, offset = 0;
	vector<T>  database;
	while (!fin.eof() && i < m) {
		fin.read(buffer, nbytes);
		T *m_features = new T[vcc*vrr/m];
		for (int n = 0; n < num; n++) {
			memcpy(m_features + n*vcc, buffer + 4 + n*(4 + vcc * sizeof(T)), vcc * sizeof(T));
		}
		for (int i = 0; i < vrr/m; i++)
		{
			database.clear();
			for (int j = i*vcc; j < (i + 1)*vcc; j++)
			{
				database.push_back(m_features[j]);
			}
			data.push_back(database);
		}
		delete m_features;
		i++;
	}
	fin.close();
	delete[]buffer;
	buffer = NULL;
};

template<typename T>
void load_index_file(index<T> &dataindex, string filename)
{
	index <T>::mNode Node;
	vector <index <T>::mNode> Nodes;         //倒排索引文件  记录训练集点的id+分组聚类结果 
	vector <T> cluster;             //类中心
	vector<vector <T>> pq_cen;
	ifstream fin;
	fin.open(filename);
	string word;
	fin >> word;
    int	dim = atoi(word.c_str());
	fin >> word;
	dataindex.m = atoi(word.c_str());
	fin >> word;
	dataindex.k1 = atoi(word.c_str());
	fin >> word;
	dataindex.k2 = atoi(word.c_str());
		//载入inverted_list
		for (int w=0;w < dataindex.k1;w++)
		{
			fin >> word;
			int num = atoi(word.c_str());//每一粗糙类包含向量个数
			for (int i = 0; i<num; i++)
			{
				fin >> word;
				Node.group.clear();
				Node.ID = atoi(word.c_str());
				for (int j = 0; j < dataindex.m; j++)
				{
					fin >> word; Node.group.push_back(atoi(word.c_str()));
				}
				Nodes.push_back(Node);
			}
			dataindex.inverted_list.push_back(Nodes);
			Nodes.clear();
		}
		//载入cluster
		for(int w=0;w<dataindex.k1;w++)
		{
			for (int i = 0; i < dim; i++)
			{
				fin >> word;
				cluster.push_back(atof(word.c_str()));
			}
			dataindex.cluster.push_back(cluster);
			cluster.clear();
		}
		for (int w = 0; w < dataindex.m; w++)
		{
			for (int i = 0; i < dataindex.k2; i++)
			{
				for (int j = 0; j < dataindex.cluster[0].size() / dataindex.m; j++)
				{
					fin >> word;
					cluster.push_back(atof(word.c_str()));
				}
				pq_cen.push_back(cluster);
				cluster.clear();
			}
			dataindex.pq_centroids.push_back(pq_cen);
			pq_cen.clear();
		}
	}
#pragma once
