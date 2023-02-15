#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<algorithm>

extern const char* KeyWord[];
extern const char* Symbol[];
extern const char* EndSymbol;
extern const char* Identity_Name;
extern const char* Constant_Name;
extern const char* eps;
extern const char* endc;

//词法分析单个单词表示
struct Word {
	std::string		id;		//标识
	std::string		value;	//值
	int				line;	//出现行数
	Word() :id(""), value(""), line(0) {}
	Word(std::string id, std::string value,int line) 
		:id(id), value(value),line(line) {}
	Word(std::string id, std::string value, int line,int no)
		:id(id), value(value), line(line)  {}
	std::string str();
	bool operator< (const Word& word)const;
	Word& operator=(const Word& word);
};
//扫描char* 数组
int ArrayFind(const char** array, int size, std::string goal);


class LexicalAnalyzer
{
private:
	std::string lx_fp;	//源文件地址
	std::vector<Word> lx_Wordist;//词法分析结果
	
	std::string lx_errorinfo;		//报错信息
	bool lx_error;					//报错标志
private:
	void addWord(std::string id, std::string value,int line);	//添加到词法分析结果

	std::string readfile();					//读取
	void scan(std::string& text);			//扫描
	void addError(int line,char ch);		//添加报错信息		

public:
	//构造函数
	LexicalAnalyzer(const char* fp);
    ~LexicalAnalyzer();
	//重新选定读入源文件
	void reload(const char* fp);
	//执行函数
	void execute();
	//清空函数
	void clear();
	//判断是否有错
	bool is_error();
	std::vector<Word>& GetWordList();
	//导出错误信息
	std::string geterrorinfo();
	//测试用
	void test();

};

