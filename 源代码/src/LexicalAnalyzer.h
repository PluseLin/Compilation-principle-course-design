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

//�ʷ������������ʱ�ʾ
struct Word {
	std::string		id;		//��ʶ
	std::string		value;	//ֵ
	int				line;	//��������
	Word() :id(""), value(""), line(0) {}
	Word(std::string id, std::string value,int line) 
		:id(id), value(value),line(line) {}
	Word(std::string id, std::string value, int line,int no)
		:id(id), value(value), line(line)  {}
	std::string str();
	bool operator< (const Word& word)const;
	Word& operator=(const Word& word);
};
//ɨ��char* ����
int ArrayFind(const char** array, int size, std::string goal);


class LexicalAnalyzer
{
private:
	std::string lx_fp;	//Դ�ļ���ַ
	std::vector<Word> lx_Wordist;//�ʷ��������
	
	std::string lx_errorinfo;		//������Ϣ
	bool lx_error;					//�����־
private:
	void addWord(std::string id, std::string value,int line);	//��ӵ��ʷ��������

	std::string readfile();					//��ȡ
	void scan(std::string& text);			//ɨ��
	void addError(int line,char ch);		//��ӱ�����Ϣ		

public:
	//���캯��
	LexicalAnalyzer(const char* fp);
    ~LexicalAnalyzer();
	//����ѡ������Դ�ļ�
	void reload(const char* fp);
	//ִ�к���
	void execute();
	//��պ���
	void clear();
	//�ж��Ƿ��д�
	bool is_error();
	std::vector<Word>& GetWordList();
	//����������Ϣ
	std::string geterrorinfo();
	//������
	void test();

};

