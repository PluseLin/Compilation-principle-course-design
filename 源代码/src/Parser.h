#pragma once
#include "LexicalAnalyzer.h"
#include "LL1Grammar.h"
#include<queue>
#include<stack>

//�﷨������
//�﷨����������Ҫ��¼ʹ�õĲ���ʽ
//��Ҫ��¼ʹ�õ��ս����Ӧ�ĵ��ʱ��
class ParserTree {
private:
	//��ŵ�����Ϣ
	//���ڷ��ս��,word.id="$Vn" word.value=vn��word.line=-1;
	std::vector<Word> t_WordList;
	//��֦��mapʵ��
	std::map<int, std::vector<int> > t_Branch;
	//���ڵ�
	int rootindex = 1;
	void dfs(std::vector<std::vector<Word> >& ParserResult, int fa);
	void dfs(std::vector<std::vector<int> >& ParserResult, int fa);
public:
    ~ParserTree();
	//�����ȡ�����ʣ�ͨ������ȡջ��
	Word GetWord(int index);
	//��ӵ��ʣ�indexΪ��ţ�wordΪ���ʵ�Word�ṹ��
	void addWord(Word& word);
	//�����֦��Ҫ�ṩ���׶��ӵ����
	void addBranch(int i_fa, int i_chi);
	//�޸��﷨����ĳһ��������
	void UpdateWord(int index, Word w);

	//dfs�ķ�ʽ�������ʽ�±���ţ�����������ã�
	void dfs_export(std::vector<std::vector<int> >& ParserResult);
	//��dfs�ķ�ʽ���ʹ�õĲ���ʽ
	void dfs_export(std::vector<std::vector<Word> >& ParserResult);
	//��bfs�ķ�ʽ���ʹ�õĲ���ʽ
	void bfs_export(std::vector<std::vector<std::vector<Word> > >& ParserResult);

	//������������
	std::vector<Word>& GetWordList();

    //������֦
    std::map<int,std::vector<int> >& GerBranchs(){
        return this->t_Branch;
    }
};

class Parser{
private:
	//LL1�ķ��࣬�ں�������ӿ�
    LL1Grammar* ps_Grammar=nullptr;
	//����ջ
	//����ջ�ڴ�ŵ��ǵ��ʵ��±꣬ȡ��ʱ��Ҫ��ps_Tree��ȡ��
	std::stack<int> ps_AnalyStack;
	//���뵥������
	std::vector<Word> ps_WordList;
	//������Ϣ
	std::string ps_errorinfo;
	bool ps_error;
	//������־
	std::vector<std::string> ps_log;
private:
	//��Ϊ��ջ�Ĳ����л�Ҫ�漰�±����ݣ����������΢��װһ��
	//execute������ֱ�ӵ�����Щ��������ʹ�ó�Ա������
	void StackPush(int index, Word w);
	Word StackTop();
	//����������word��ԭ������Ҫ�޸��﷨���д˵��ʵ�����
	void StackPop(Word w);
	//����Ҫ�Ķ�ʱ�����Ӳ���
	void StackPop();
	//��Ӳ���ʽ
	void addProd(Production p,int& index);
public:
    bool is_error();

    std::string geterrorinfo();

    std::vector<std::string>& GetLog();

	//�﷨������
    ParserTree* ps_Tree=nullptr;
	//���캯��
	Parser(const char* fp, LexicalAnalyzer& la);
    //��������
    ~Parser();
	//��պ���
	void clear();
	//ִ�к���
	void execute();
	//������
	void test();

};

