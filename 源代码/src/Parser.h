#pragma once
#include "LexicalAnalyzer.h"
#include "LL1Grammar.h"
#include<queue>
#include<stack>

//语法分析树
//语法分析树不仅要记录使用的产生式
//还要记录使用的终结符对应的单词编号
class ParserTree {
private:
	//存放单词信息
	//对于非终结符,word.id="$Vn" word.value=vn，word.line=-1;
	std::vector<Word> t_WordList;
	//树枝，map实现
	std::map<int, std::vector<int> > t_Branch;
	//根节点
	int rootindex = 1;
	void dfs(std::vector<std::vector<Word> >& ParserResult, int fa);
	void dfs(std::vector<std::vector<int> >& ParserResult, int fa);
public:
    ~ParserTree();
	//用序号取出单词，通常用于取栈顶
	Word GetWord(int index);
	//添加单词，index为序号，word为单词的Word结构体
	void addWord(Word& word);
	//添加树枝，要提供父亲儿子的序号
	void addBranch(int i_fa, int i_chi);
	//修改语法树的某一单词内容
	void UpdateWord(int index, Word w);

	//dfs的方式输出产生式下标序号（给语义分析用）
	void dfs_export(std::vector<std::vector<int> >& ParserResult);
	//用dfs的方式输出使用的产生式
	void dfs_export(std::vector<std::vector<Word> >& ParserResult);
	//用bfs的方式输出使用的产生式
	void bfs_export(std::vector<std::vector<std::vector<Word> > >& ParserResult);

	//导出单词序列
	std::vector<Word>& GetWordList();

    //导出树枝
    std::map<int,std::vector<int> >& GerBranchs(){
        return this->t_Branch;
    }
};

class Parser{
private:
	//LL1文法类，内含分析表接口
    LL1Grammar* ps_Grammar=nullptr;
	//分析栈
	//分析栈内存放的是单词的下标，取得时候要从ps_Tree中取出
	std::stack<int> ps_AnalyStack;
	//输入单词序列
	std::vector<Word> ps_WordList;
	//报错信息
	std::string ps_errorinfo;
	bool ps_error;
	//分析日志
	std::vector<std::string> ps_log;
private:
	//因为对栈的操作中还要涉及下标内容，因此这里稍微封装一下
	//execute函数中直接调用这些函数，不使用成员变量。
	void StackPush(int index, Word w);
	Word StackTop();
	//这里加入参数word的原因是需要修改语法树中此单词的内容
	void StackPop(Word w);
	//不需要改动时，不加参数
	void StackPop();
	//添加产生式
	void addProd(Production p,int& index);
public:
    bool is_error();

    std::string geterrorinfo();

    std::vector<std::string>& GetLog();

	//语法分析树
    ParserTree* ps_Tree=nullptr;
	//构造函数
	Parser(const char* fp, LexicalAnalyzer& la);
    //析构函数
    ~Parser();
	//清空函数
	void clear();
	//执行函数
	void execute();
	//测试用
	void test();

};

