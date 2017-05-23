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
	vector<vector <T>> query;      // ��������
	vector<vector <int>> annresult; //�����
	void ADCsearch();
	void knnsearch(vector<T> qu,int n);
	index<T> dataindex;
public:
	vector<int>  coarse_class;     //��¼ÿ��������Ӧ�Ĵֲ���
	class cq
	{public:
		int clus; //���ƴֲ���  
		vector<T> dis;//  ���Ե���ֲ��������ĵ���� 
		T Eucdis;//ŷ�Ͼ���
	};
	class result
	{public:
		int resultid;
		T Eucdis;//ŷ�Ͼ���
	};
	vector<vector <int>> PQsearch(int n);   //����nΪmultiple_assignmet���� 
	vector <cq> mul_assignment;    //�洢���ƴֲ�������� 
	void Distance(vector <T>v1, vector <T>v2, cq &d);
	IVFADC_index(index<T> dataindex,vector<vector <T>> query);
};

template<typename T>
vector<vector <int>> IVFADC_index<T>::PQsearch(int n)
{
	for (int i = 0; i < query.size(); i++)
	{
		knnsearch(query[i],n);  //�ҵ�������
		ADCsearch();  // ���ڷǶԳƾ������d���������ҵ������Ե������top n 
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
			else { j--; continue; }      //��֤���Ҵֲ�����������
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
		vector<vector <T>> mquery; //��������
		vector<vector <T>> Distab;     // ��Ų��Ե���m����ÿһ���i����������u����  
		int no, count = 0;
		int topk =1000;//���������1000
		int  *w = new int[dataindex.k2];
		for (int i= 0; i < dataindex.k2; i++) w[i] = 0;
		vector<result> result;
		vector <T> vect;
		IVFADC_index<T>::result re;
		for (int i = 0; i < mul_assignment.size(); i++)   //   ������ڴֲ���   
		{
			vector<T> vec(dataindex.k2);
			Distab.clear();//���Distab��
			mquery.clear(); //���mquery��
			for (int j = 0; j < dataindex.m; j++) {
				Distab.push_back(vec);
				vect.clear();
				for (int t = j*query[0].size() /dataindex. m; t < (j + 1)*query[0].size() / dataindex.m; t++)    vect.push_back(mul_assignment[i].dis[t]);
				mquery.push_back(vect);
			}
			//���ϡ������
			for (int j = 0; j <dataindex. m; j++) {
				for (int t = 0; t < dataindex.inverted_list[mul_assignment[i].clus].size(); t++)
				{
					no = dataindex.inverted_list[mul_assignment[i].clus].at(t).group.at(j);
					if (w[no] == 0) {
						cq d;
						Distance(mquery[j], dataindex.pq_centroids[j].at(no), d);
						Distab[j].at(no) = d.Eucdis; w[no]++;
					}// �ж��Ƿ�����
					if (j == 0) {
						re.Eucdis = Distab[j].at(no);
						re.resultid = dataindex.inverted_list[mul_assignment[i].clus].at(t).ID;
						result.push_back(re);
					}
					else result[t + count].Eucdis += Distab[j].at(no);
				}
				for (int k= 0; k < dataindex.k2; k++) w[k] = 0;//�����ж�
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

