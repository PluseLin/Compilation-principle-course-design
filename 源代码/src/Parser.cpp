#include "Parser.h"
using namespace std;
/*非成员函数*/


/*ParserTree成员函数*/
ParserTree::~ParserTree(){
    t_WordList.clear();
    t_Branch.clear();
}

//用序号取出单词，通常用于取栈顶
Word ParserTree::GetWord(int index) {
	//检查下标，防止溢出
	if (index < 0 || index >= t_WordList.size())
		return Word("", "", -1);
	return t_WordList[index];
}
//添加单词，index为序号，word为单词的Word结构体
void ParserTree::addWord(Word& word) {
	t_WordList.push_back(word);
}
//添加树枝，要提供父亲儿子的序号
void ParserTree::addBranch(int i_fa, int i_chi) {
	if (t_Branch.find(i_fa) == t_Branch.end())
		t_Branch[i_fa] = vector<int>();
	t_Branch[i_fa].push_back(i_chi);
}
//修改语法树的某一单词内容
void ParserTree::UpdateWord(int index, Word w) {
	if (index < 0 || index >= t_WordList.size())
		return;
	t_WordList[index] = w;
}

void ParserTree::dfs(vector<vector<int> >& ParserResult, int fa) {
	auto children = t_Branch[fa];
	vector<int> temp;
	temp.push_back(fa);
	//dfs
	/*
	for (int child : children) {
		dfs(ParserResult, child);
	}
	*/
	for (auto p = children.rbegin(); p != children.rend(); p++) {
		dfs(ParserResult, *p);
		temp.push_back(*p);
	}
	//若有儿子，长度大于1
	if (temp.size() > 1)
		ParserResult.push_back(temp);
}

void ParserTree::dfs(vector<vector<Word> >& ParserResult, int fa) {
	auto children = t_Branch[fa];
	vector<Word> Words;
	Words.push_back(t_WordList[fa]);
	//dfs
	for (auto p = children.rbegin(); p != children.rend(); p++) {
		dfs(ParserResult, *p);
		Words.push_back(t_WordList[*p]);
	}
	//若有儿子，长度大于1
	if (Words.size() > 1)
		ParserResult.push_back(Words);
}

vector<Word>& ParserTree::GetWordList() {
	return t_WordList;
}

//用dfs的方式输出使用的产生式
//< $Vn, Factor > -> < $const, 0 >
//产生式中，第一个为左部，后面为右部
//对于word，若为非终结符,$id,$const，使用word.value，否则使用word.id
void ParserTree::dfs_export(vector<vector<Word> >& ParserResult) {
	ParserResult.clear();
	dfs(ParserResult, rootindex);
}

//dfs的方式输出产生式下标序号（给语义分析用）
void ParserTree::dfs_export(std::vector<std::vector<int> >& ParserResult) {
	ParserResult.clear();
	dfs(ParserResult, rootindex);
}

void ParserTree::bfs_export(vector<vector<vector<Word> > >& ParserResult) {
	ParserResult.clear();
	struct info {
		int index;
		int level;
	};
	queue<info> Q;
	Q.push(info{ rootindex,1 });
	while (!Q.empty()) {
		info front = Q.front();
		Q.pop();
		vector<Word> temp;
		temp.push_back(t_WordList[front.index]);
		for (auto p = t_Branch[front.index].rbegin(); p != t_Branch[front.index].rend(); p++) {
			Q.push(info{ *p,front.level + 1 });
			temp.push_back(t_WordList[*p]);
		}
		if (temp.size() > 1) {
			if (front.level + 1 >= ParserResult.size())
				ParserResult.push_back(vector<vector<Word> >());
			ParserResult.back().push_back(temp);
		}
	}
}

/*Parser成员函数*/

//封装的分析栈操作函数
void Parser::StackPush(int index, Word w) {
    ps_Tree->addWord(w);
	ps_AnalyStack.push(index);
}

Word Parser::StackTop() {
	int top = ps_AnalyStack.top();
    return ps_Tree->GetWord(top);
}

void Parser::StackPop(Word w) {
	int top = ps_AnalyStack.top();
    ps_Tree->UpdateWord(top, w);
	ps_AnalyStack.pop();
}

void Parser::StackPop() {
	ps_AnalyStack.pop();
}

void Parser::addProd(Production p,int& index) {
	int i_fa = ps_AnalyStack.top();
	//先出栈
	ps_AnalyStack.pop();
	//逆序入栈
	for (auto ptr = p.right.rbegin(); ptr != p.right.rend(); ptr++) {
        ps_Tree->addBranch(i_fa, index);
		if (is_Vn(*ptr)) {
			StackPush(index++, Word("$Vn", *ptr, -1));
		}
		else {
			StackPush(index++, Word(*ptr, *ptr, -1));
		}
	}
}

//构造函数
Parser::Parser(const char* fp, LexicalAnalyzer& la) {
    ps_Tree=new ParserTree();
    ps_Grammar=new LL1Grammar();
    ps_Grammar->Init(fp);
	ps_error = false;
    //ps_WordList.swap(la.GetWordList());
    vector<Word>& temp=la.GetWordList();
    ps_WordList.assign(temp.begin(),temp.end());
}

//析构函数
Parser::~Parser(){
    delete ps_Tree;
    delete ps_Grammar;
    ps_WordList.clear();
    ps_log.clear();
}

//清空函数
void Parser::clear() {
	ps_WordList.clear();
	ps_errorinfo = "";
	ps_error = false;
	while (!ps_AnalyStack.empty()) ps_AnalyStack.pop();
}

//执行函数
void Parser::execute() {
	int index = 0;//下标0开始计数
	auto Inputptr = ps_WordList.begin();
	int loopcnt = 1;
	//首先，栈顶加入$# 和 起始符号
	StackPush(index++, Word(endc, "#", -1));
    StackPush(index++, Word("$Vn", ps_Grammar->StartVn, -1));
	//循环
	while (!ps_AnalyStack.empty() && Inputptr != ps_WordList.end()) {
		//取出栈顶
		Word top = StackTop();
		/*四种情况 endc="$#"
		* 1. top.id==endc && Inputptr->id==endc 分析成功
		* 2. top.id=epc 出栈，continue
		* 3. top.id==Inputptr->id && top.id!=endc 出栈，下一个输入
		* 4. is_Vn(top.id) 决定使用哪个产生式，入栈。或error
		*/
		if (top.id == endc && Inputptr->id == endc) {
			string info = "第" + to_string(loopcnt) + "步，栈顶为：" + top.value +
				"，输入为：" + Inputptr->value + "，分析成功";
			ps_log.push_back(info);
			return;
		}
		else if (top.id == eps) {
			string info = "第" + to_string(loopcnt) + "步，栈顶为：" + top.value +
				"，输入为：" + Inputptr->value + ", 跳过";
			ps_log.push_back(info);
			StackPop();
		}
		else if (top.id == Inputptr->id && top.id != endc) {
			string info = "第" + to_string(loopcnt) + "步，栈顶为：" + top.value +
				"，输入为：" + Inputptr->value + "，出栈";
			StackPop(*Inputptr);
			Inputptr++;
			ps_log.push_back(info);
		}
		else if(top.id=="$Vn") {
			//取出产生式
            Production prod = ps_Grammar->PredProduction(top.value, Inputptr->id);
			//约定左部为空的时候，是错误情况
			if (prod.left == "") {
				ps_error = true;
				//ps_errorinfo = string("语法错误：出现在第") + to_string(Inputptr->line) +
				//	"行单词" + Inputptr->value + "附近";
				ps_errorinfo = string("语法错误：出现在单词") + Inputptr->value + "附近";
				return;
			}
			string info = "第" + to_string(loopcnt) + "步，栈顶为：" + top.value +
				"，输入为：" + Inputptr->value + "，使用产生式：" + prod.str();
			addProd(prod, index);
			ps_log.push_back(info);
		}
		else {
			ps_error = true;
			ps_errorinfo = string("语法错误：出现在单词") + Inputptr->value + "附近";
			return;
		}
		loopcnt++;
	}
}

//测试用
void Parser::test() {
	execute();
	if (ps_error) {
		cout << "分析失败" << endl;
		cout << ps_errorinfo << endl;
	}
	else {
		cout << "分析成功" << endl;
	}
	//输出日志
	if (0) {
		cout << "执行步骤" << endl;
		for (string line : ps_log) {
			cout << line << endl;
		}
	}
	//输出使用的产生式dfs
	if (1) {
		vector<vector<Word> > ParserResult;
        ps_Tree->dfs_export(ParserResult);
		cout << "dfs展示使用的产生式" << endl;
		for (auto list : ParserResult) {
			for (auto ptr = list.begin(); ptr != list.end(); ptr++) {
				cout << ptr->str() << " ";
				if (ptr == list.begin())
					cout << "-> ";
			}
			cout << endl;
		}
	}
	//输出使用的产生式bfs
	if (1) {
		vector<vector<vector<Word> > > ParserResult;
        ps_Tree->bfs_export(ParserResult);
		cout << "bfs展示使用的产生式" << endl;
		int level = 1;
		for (auto list : ParserResult) {
			cout << "level " << level << ":\n";
			for (auto prod : list) {
				for (auto ptr = prod.begin(); ptr != prod.end(); ptr++) {
					cout << ptr->str() << " ";
					if (ptr == prod.begin()) cout << "-> ";
				}
				cout << endl;
			}
			level++;
		}
	}
	//输出产生式序号dfs
	if (0) {
		vector<vector<int> > ParserResult;
        ps_Tree->dfs_export(ParserResult);
		cout << "dfs展示产生式序号表示" << endl;
		for (auto prod : ParserResult) {
			for (auto p = prod.begin(); p != prod.end(); p++) {
				cout << *p << " ";
				if (p == prod.begin())
					cout << "-> ";
			}
			cout << endl;
		}
		int index = 1;
		while (true) {
            Word w = ps_Tree->GetWord(index);
			if (w.id == "") break;
			cout << index << " : " << w.str() << endl;
			index++;
		}
	}
}

bool Parser::is_error(){
    return this->ps_error;
}

string Parser::geterrorinfo(){
    return this->ps_errorinfo;
}

vector<string>& Parser::GetLog(){
    return this->ps_log;
}
