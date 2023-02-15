#pragma once
#include<iostream>
#include<set>
#include<map>
#include<stack>
#include<string>
#include<vector>
#include"LexicalAnalyzer.h"

typedef std::string Vt;	//终结符
typedef std::string Vn;	//非终结符

bool is_Vt(std::string str);//判断是否为终结符
bool is_Vn(std::string str);//判断是否为非终结符

//分割字符串
std::vector<std::string> split(std::string str, char sp=' ');

struct Production {
	//左部
	std::string					left;
	//右部
	std::vector<std::string>	right;
	Production(){}
    ~Production();
	std::string str();
};

//非终结符First
struct FirstSet {
	//非终符
	Vn left;
	//终符集合
	std::set<Vt> _set;
	//构造函数
	FirstSet();
	FirstSet(Vn left);
	FirstSet(const FirstSet& fs);
    //析构函数
    ~FirstSet();
	//重载=运算符
	FirstSet& operator=(const FirstSet& fs);

	//添加非终符
	void addVt(const FirstSet& fs);			
	void addVt(Vt vt);
	void addVt(std::set<Vt>& VtSet);
	//返回删去某个非终结符的结果
	FirstSet erase(Vt vt);
	//找First中有无指定单词，一般是eps
	bool findword(std::string word="$eps");
};

struct FollowSet {
	//非终符
	Vn left;
	//终符集合
	std::set<Vt> _set;
	//构造函数
	FollowSet();
	FollowSet(Vn left);
	FollowSet(const FollowSet& fs);
    //析构函数
    ~FollowSet();
	//重载=运算符
	FollowSet& operator=(const FollowSet& fs);
	//添加非终结符
	void addVt(std::set<Vt> VtSet, std::string erase = "$eps");
	void addVt(const FirstSet& fs);
	void addVt(const FollowSet& fs);
};

class LL1Grammar{
	//friend class Parser;
private:
	//所有产生式
	std::vector<Production>	g_ProductionList;
	//记录同一个终符的产生式位置，映射为一个区间（左开右闭）
	//必须保证同一个终结符的所有产生式在文件中是连续的！！！！
	std::map < Vn, std::pair<int,int> > g_ProdIndex;
	//终结符集合
	std::set<Vt> g_VtSet;
	//非终结符集合
	std::set<Vn> g_VnSet;
	//first集合
	std::map<Vn,FirstSet> g_FirstMap;
	//follow集合
	std::map<Vn,FollowSet> g_FollowMap;
	//预测分析表,键为Vn,Vt对，值为产生式序号。
	std::map<std::pair<Vn, Vt>, int> g_PredTable;
private:
	//取出左部为vn的所有产生式
	std::vector<Production> GetProduction(Vn vn);
	void dfs_GetFirst(Vn vn, std::map<Vn, bool>& record);
	void dfs_GetFollow(Vn vn, std::map<Vn, std::set<Vn> >& Need,std::map<Vn,bool>& record);

	void ImportGrammar(const char* fp);//导入文法，初始化产生式，终符非终符集合
	void InitFirst();//初始化First
	void InitFollow();//初始化Follow
	void InitPredTable();//初始化预测分析表
public:
    //First Follow 接口
	std::set<Vt> First(std::string word);	//终符/非终符
	std::set<Vt> First(std::vector<std::string>& list);
	//句型，可删去指定单词
	std::set<Vt> First(Production& p);//产生式（右部）
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

	//起始非终符
	Vn StartVn;
	//预测分析表接口
	Production PredProduction(Vn StackTop, Vt InputWord);
	//构造函数
	LL1Grammar();
    ~LL1Grammar();
	//初始化
	void Init(const char* fp);

	//验证文法是否为LL1文法
	bool is_LL1();

	//测试用
	void test();
};

