#pragma once
#include<iostream>  
#include<vector>  
#include<map>  
#include<cstdlib>  
#include<algorithm>  
#include<fstream>  
#include<string>  
#include<time.h>  //for srand  
#include<limits.h> //for INT_MIN INT_MAX  
using namespace std;
template<typename T>
class KMEANS
{
private:
	vector< vector<T> > dataSet;           // dataSet：训练数据，即所有点的集合，是一个多维的矩阵
	int colLen, rowLen;                    //colLen:向量维度;rowLen：向量个数 
	int k;                                 //k类 
	void initClusterAssment();              //初始化数据点所属
	double distEclud(vector<T> &v1, vector<T> &v2); //计算欧氏距离
public:
	class MinMax
	{public:
		T Min;
		T Max;
		MinMax(T min, T max) { Min = min; Max = max; };
	};
	class Node
	{public :
		int minIndex; //每个数据点的近邻类
		double minDist;
	};
	vector<vector<T>> centroids;
	vector<Node>clusterAssment;
	KMEANS(int k, vector< vector<T> > dataSet);
	void randCent();      //随机生成k个点 
	void kmeans(vector< vector<T> > & centroid,vector<int> & coarse_class);  	  //计算k个中心点以及每个点的归类
	MinMax getMinMax(int idx);
	void setCentroids(MinMax &tminmax, int idx);  //设置中心点 
};

template<typename T>
void KMEANS<T>::initClusterAssment()
{
	Node node;
	node.minIndex = -1;
	node.minDist = -1;
	for (int i = 0; i<rowLen; i++)
	{
		clusterAssment.push_back(node);
	}
};

template<typename T>
 void KMEANS<T>::kmeans(vector< vector<T> > & centroid,vector<int> & coarse_class)
 {
	initClusterAssment();
	bool clusterChanged = true;
	//the termination condition can also be the loops less than some number such as 1000  
	int w = 0;
	while (clusterChanged)
	{
		w++;
		cout << w << endl;
		clusterChanged = false;
		//step one : find the nearest centroid of each point  
		for (int i = 0; i<rowLen; i++)
		{
			int minIndex = -1;
			double minDist = INT_MAX;
			for (int j = 0; j<k; j++)
			{
					double distJI = distEclud(dataSet[i], centroids[j]);
					if (distJI < minDist)
					{
						minDist = distJI;
						minIndex = j;
					}
			}
			if (clusterAssment[i].minIndex != minIndex)
			{
				clusterChanged = true;
				clusterAssment[i].minIndex = minIndex;
				clusterAssment[i].minDist = minDist;
			}
		}
		//step two : 更新中心点  
		//cout << "update the centroids:" << endl;
		for (int cent = 0; cent<k; cent++)
		{
			vector<T> vec(colLen, 0);
			int cnt = 0;
			for (int i = 0; i<rowLen; i++)
			{
				if (clusterAssment[i].minIndex == cent)
				{
					++cnt;
					//sum of two vectors  
					for (int j = 0; j<colLen; j++)
					{
						vec[j] += dataSet[i].at(j);
					}
				}
			}
			//mean of the vector and update the centroids[cent]  
			for (int j = 0; j<colLen; j++)
			{
				if (cnt != 0 ) centroids[cent].at(j) = vec[j] / cnt;
			}
		}//for  
	}//while 
	centroid = centroids;
	for(int i=0;i<dataSet.size();i++)	coarse_class[i]= clusterAssment[i].minIndex;
	dataSet.swap(vector< vector<T> >());
};

template<typename T>
KMEANS<T>::KMEANS(int k, vector< vector<T> > dataSet)
{
	this->k = k;
	this->dataSet = dataSet;
	this->colLen = dataSet[0].size();
	this->rowLen = dataSet.size();
};

template<typename T>
void KMEANS<T>::setCentroids(MinMax &tminmax, int idx)
{
	T rangeIdx = tminmax.Max - tminmax.Min;
	for (int i = 0; i<k; i++)
	{
		/* generate float data between 0 and 1 */
		centroids[i].at(idx) = tminmax.Min + rangeIdx *  (rand() / (double)RAND_MAX);
	}
}

//get the min and max value of the idx column  
template<typename T>
typename KMEANS<T>::MinMax KMEANS<T>::getMinMax(int idx)
{
	T min, max;
	dataSet[0].at(idx) > dataSet[1].at(idx) ? (max = dataSet[0].at(idx), min = dataSet[1].at(idx)) : (max = dataSet[1].at(idx), min = dataSet[0].at(idx));

	for (int i = 2; i<rowLen; i++)
	{
		if (dataSet[i].at(idx) < min)   min = dataSet[i].at(idx);
		else if (dataSet[i].at(idx) > max) max = dataSet[i].at(idx);
		else continue;
	}
	MinMax tminmax(min, max);
	return tminmax;
}

template<typename T>
void KMEANS<T>::randCent()
{
	//init centroids  
	vector<T> vec(colLen, 0);
	for (int i = 0; i<k; i++)
	{
		centroids.push_back(vec);
	}
	//set values by column  
	srand(time(NULL));
	for (int j = 0; j<colLen; j++)
	{
		MinMax tminmax = getMinMax(j);
		setCentroids(tminmax, j);
	}
}

template<typename T>
double KMEANS<T>::distEclud(vector<T> &v1, vector<T> &v2)
{
	T sum = 0;
	int size = v1.size();
	for (int i = 0; i<size; i++)
	{
		sum += (v1[i] - v2[i])*(v1[i] - v2[i]);
	}
	return sum;
}
