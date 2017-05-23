#include<iostream>  
#include<math.h>
#include<vector>  
#include<map>  
#include<cstdlib>  
#include<algorithm>  
#include<fstream>  
#include "k_means.h"
#include<string>   
#include "index.h"
using namespace std;
template<typename T>
class IVFADC_index 
{
private:
	vector<vector <T>> query;      // 测试数据
	vector<vector <int>> annresult; //结果集
	void ADCsearch();
	void knnsearch(vector<T> qu,int n);
	index<T> dataindex;
public:
	vector<int>  coarse_class;     //记录每个向量对应的粗糙类
	class cq
	{public:
		int clus; //近似粗糙类  
		vector<T> dis;//  测试点离粗糙量化中心点距离 
		T Eucdis;//欧氏距离
	};
	class result
	{public:
		int resultid;
		T Eucdis;//欧氏距离
	};
	vector<vector <int>> PQsearch(int n);   //其中n为multiple_assignmet个数 
	vector <cq> mul_assignment;    //存储近似粗糙类与距离 
	void Distance(vector <T>v1, vector <T>v2, cq &d);
	IVFADC_index(index<T> dataindex,vector<vector <T>> query);
};

template<typename T>
vector<vector <int>> IVFADC_index<T>::PQsearch(int n)
{
	for (int i = 0; i < query.size(); i++)
	{
		knnsearch(query[i],n);  //找到相似类
		ADCsearch();  // 基于非对称距离计算d，并排序找到力测试点最近的top n 
	}
	return annresult;
};

	template<typename T>
    IVFADC_index<T>::IVFADC_index(index<T> dataindex,vector<vector< T > > query)
	{
		this->dataindex = dataindex;
		this->query = query;
	};

	template<typename T>
	void IVFADC_index<T>::knnsearch(vector<T> qu,int n)
	{
		int size = dataindex.cluster.size();
		mul_assignment.clear();
		cq *r;
		r = new cq[size];
			for (int j = 0; j<size; j++)
			{
				Distance(qu, dataindex.cluster[j],r[j]);
				r[j].clus = j;
			}
		sort(r, r + size ,cmp<T>());
		for (int j = 0; j < n; j++)
		{
			if (dataindex.inverted_list[r[j].clus].size() != 0)  mul_assignment.push_back(r[j]);      
			else { j--; continue; }      //保证所找粗糙类内有数据
		}
	};

	template<typename T>
    void  IVFADC_index<T>::Distance(vector <T> v1, vector <T>v2,cq &d)
	{
		T sum = 0;
		for (int i = 0; i<v1.size(); i++)
		{
			d.dis.push_back (v1.at(i) - v2.at(i));
			sum += d.dis.at(i)*d.dis.at(i);
		}
		d.Eucdis = sum;
	};

    template<typename T>
	void IVFADC_index<T>::ADCsearch()
	{
		vector<vector <T>> mquery; //余量分组
		vector<vector <T>> Distab;     // 存放测试点与m组中每一组第i个量化中心u距离  
		int no, count = 0;
		int topk =1000;//查找最近邻1000
		int  *w = new int[dataindex.k2];
		for (int i= 0; i < dataindex.k2; i++) w[i] = 0;
		vector<result> result;
		vector <T> vect;
		IVFADC_index<T>::result re;
		for (int i = 0; i < mul_assignment.size(); i++)   //   多个近邻粗糙类   
		{
			vector<T> vec(dataindex.k2);
			Distab.clear();//清空Distab表
			mquery.clear(); //清空mquery表
			for (int j = 0; j < dataindex.m; j++) {
				Distab.push_back(vec);
				vect.clear();
				for (int t = j*query[0].size() /dataindex. m; t < (j + 1)*query[0].size() / dataindex.m; t++)    vect.push_back(mul_assignment[i].dis[t]);
				mquery.push_back(vect);
			}
			//解决稀疏量化
			for (int j = 0; j <dataindex. m; j++) {
				for (int t = 0; t < dataindex.inverted_list[mul_assignment[i].clus].size(); t++)
				{
					no = dataindex.inverted_list[mul_assignment[i].clus].at(t).group.at(j);
					if (w[no] == 0) {
						cq d;
						Distance(mquery[j], dataindex.pq_centroids[j].at(no), d);
						Distab[j].at(no) = d.Eucdis; w[no]++;
					}// 判断是否计算过
					if (j == 0) {
						re.Eucdis = Distab[j].at(no);
						re.resultid = dataindex.inverted_list[mul_assignment[i].clus].at(t).ID;
						result.push_back(re);
					}
					else result[t + count].Eucdis += Distab[j].at(no);
				}
				for (int k= 0; k < dataindex.k2; k++) w[k] = 0;//重置判断
			}
			count += dataindex.inverted_list[mul_assignment[i].clus].size();
		}
		sort(result.begin(), result.end(), cmpresult<T>());
		vector <int> singleresult;
		for (int i = 0; i < topk&&i<result.size(); i++)   singleresult.push_back(result[i].resultid);
		annresult.push_back(singleresult);
	};
	template<typename T>
	class cmp {
	public :
		bool operator()(typename IVFADC_index<T>::cq a, typename IVFADC_index<T>::cq b)
		{
			return a.Eucdis < b.Eucdis;
		};
	};
	
	template<typename T>
	class cmpresult
	{
	public :
		bool operator()(typename IVFADC_index<T>::result a, typename IVFADC_index<T>::result b)
		{
			return a.Eucdis < b.Eucdis;
		};
	};

