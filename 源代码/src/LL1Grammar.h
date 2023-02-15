#pragma once
#include<iostream>
#include<set>
#include<map>
#include<stack>
#include<string>
#include<vector>
#include"LexicalAnalyzer.h"

typedef std::string Vt;	//�ս��
typedef std::string Vn;	//���ս��

bool is_Vt(std::string str);//�ж��Ƿ�Ϊ�ս��
bool is_Vn(std::string str);//�ж��Ƿ�Ϊ���ս��

//�ָ��ַ���
std::vector<std::string> split(std::string str, char sp=' ');

struct Production {
	//��
	std::string					left;
	//�Ҳ�
	std::vector<std::string>	right;
	Production(){}
    ~Production();
	std::string str();
};

//���ս��First
struct FirstSet {
	//���շ�
	Vn left;
	//�շ�����
	std::set<Vt> _set;
	//���캯��
	FirstSet();
	FirstSet(Vn left);
	FirstSet(const FirstSet& fs);
    //��������
    ~FirstSet();
	//����=�����
	FirstSet& operator=(const FirstSet& fs);

	//��ӷ��շ�
	void addVt(const FirstSet& fs);			
	void addVt(Vt vt);
	void addVt(std::set<Vt>& VtSet);
	//����ɾȥĳ�����ս���Ľ��
	FirstSet erase(Vt vt);
	//��First������ָ�����ʣ�һ����eps
	bool findword(std::string word="$eps");
};

struct FollowSet {
	//���շ�
	Vn left;
	//�շ�����
	std::set<Vt> _set;
	//���캯��
	FollowSet();
	FollowSet(Vn left);
	FollowSet(const FollowSet& fs);
    //��������
    ~FollowSet();
	//����=�����
	FollowSet& operator=(const FollowSet& fs);
	//��ӷ��ս��
	void addVt(std::set<Vt> VtSet, std::string erase = "$eps");
	void addVt(const FirstSet& fs);
	void addVt(const FollowSet& fs);
};

class LL1Grammar{
	//friend class Parser;
private:
	//���в���ʽ
	std::vector<Production>	g_ProductionList;
	//��¼ͬһ���շ��Ĳ���ʽλ�ã�ӳ��Ϊһ�����䣨���ұգ�
	//���뱣֤ͬһ���ս�������в���ʽ���ļ����������ģ�������
	std::map < Vn, std::pair<int,int> > g_ProdIndex;
	//�ս������
	std::set<Vt> g_VtSet;
	//���ս������
	std::set<Vn> g_VnSet;
	//first����
	std::map<Vn,FirstSet> g_FirstMap;
	//follow����
	std::map<Vn,FollowSet> g_FollowMap;
	//Ԥ�������,��ΪVn,Vt�ԣ�ֵΪ����ʽ��š�
	std::map<std::pair<Vn, Vt>, int> g_PredTable;
private:
	//ȡ����Ϊvn�����в���ʽ
	std::vector<Production> GetProduction(Vn vn);
	void dfs_GetFirst(Vn vn, std::map<Vn, bool>& record);
	void dfs_GetFollow(Vn vn, std::map<Vn, std::set<Vn> >& Need,std::map<Vn,bool>& record);

	void ImportGrammar(const char* fp);//�����ķ�����ʼ������ʽ���շ����շ�����
	void InitFirst();//��ʼ��First
	void InitFollow();//��ʼ��Follow
	void InitPredTable();//��ʼ��Ԥ�������
public:
    //First Follow �ӿ�
	std::set<Vt> First(std::string word);	//�շ�/���շ�
	std::set<Vt> First(std::vector<std::string>& list);
	//���ͣ���ɾȥָ������
	std::set<Vt> First(Production& p);//����ʽ���Ҳ���
	std::set<Vt> Follow(Vn vn);
public:
    std::set<Vn>& GetVnSet(){
        return g_VnSet;
    }
    std::set<Vt>& GetVtSet(){
        return g_VtSet;
    }

    std::vector<Production>& GetProdList(){
        return this->g_ProductionList;
    }

	//��ʼ���շ�
	Vn StartVn;
	//Ԥ�������ӿ�
	Production PredProduction(Vn StackTop, Vt InputWord);
	//���캯��
	LL1Grammar();
    ~LL1Grammar();
	//��ʼ��
	void Init(const char* fp);

	//��֤�ķ��Ƿ�ΪLL1�ķ�
	bool is_LL1();

	//������
	void test();
};

