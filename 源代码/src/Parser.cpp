#include "Parser.h"
using namespace std;
/*�ǳ�Ա����*/


/*ParserTree��Ա����*/
ParserTree::~ParserTree(){
    t_WordList.clear();
    t_Branch.clear();
}

//�����ȡ�����ʣ�ͨ������ȡջ��
Word ParserTree::GetWord(int index) {
	//����±꣬��ֹ���
	if (index < 0 || index >= t_WordList.size())
		return Word("", "", -1);
	return t_WordList[index];
}
//��ӵ��ʣ�indexΪ��ţ�wordΪ���ʵ�Word�ṹ��
void ParserTree::addWord(Word& word) {
	t_WordList.push_back(word);
}
//�����֦��Ҫ�ṩ���׶��ӵ����
void ParserTree::addBranch(int i_fa, int i_chi) {
	if (t_Branch.find(i_fa) == t_Branch.end())
		t_Branch[i_fa] = vector<int>();
	t_Branch[i_fa].push_back(i_chi);
}
//�޸��﷨����ĳһ��������
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
	//���ж��ӣ����ȴ���1
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
	//���ж��ӣ����ȴ���1
	if (Words.size() > 1)
		ParserResult.push_back(Words);
}

vector<Word>& ParserTree::GetWordList() {
	return t_WordList;
}

//��dfs�ķ�ʽ���ʹ�õĲ���ʽ
//< $Vn, Factor > -> < $const, 0 >
//����ʽ�У���һ��Ϊ�󲿣�����Ϊ�Ҳ�
//����word����Ϊ���ս��,$id,$const��ʹ��word.value������ʹ��word.id
void ParserTree::dfs_export(vector<vector<Word> >& ParserResult) {
	ParserResult.clear();
	dfs(ParserResult, rootindex);
}

//dfs�ķ�ʽ�������ʽ�±���ţ�����������ã�
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

/*Parser��Ա����*/

//��װ�ķ���ջ��������
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
	//�ȳ�ջ
	ps_AnalyStack.pop();
	//������ջ
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

//���캯��
Parser::Parser(const char* fp, LexicalAnalyzer& la) {
    ps_Tree=new ParserTree();
    ps_Grammar=new LL1Grammar();
    ps_Grammar->Init(fp);
	ps_error = false;
    //ps_WordList.swap(la.GetWordList());
    vector<Word>& temp=la.GetWordList();
    ps_WordList.assign(temp.begin(),temp.end());
}

//��������
Parser::~Parser(){
    delete ps_Tree;
    delete ps_Grammar;
    ps_WordList.clear();
    ps_log.clear();
}

//��պ���
void Parser::clear() {
	ps_WordList.clear();
	ps_errorinfo = "";
	ps_error = false;
	while (!ps_AnalyStack.empty()) ps_AnalyStack.pop();
}

//ִ�к���
void Parser::execute() {
	int index = 0;//�±�0��ʼ����
	auto Inputptr = ps_WordList.begin();
	int loopcnt = 1;
	//���ȣ�ջ������$# �� ��ʼ����
	StackPush(index++, Word(endc, "#", -1));
    StackPush(index++, Word("$Vn", ps_Grammar->StartVn, -1));
	//ѭ��
	while (!ps_AnalyStack.empty() && Inputptr != ps_WordList.end()) {
		//ȡ��ջ��
		Word top = StackTop();
		/*������� endc="$#"
		* 1. top.id==endc && Inputptr->id==endc �����ɹ�
		* 2. top.id=epc ��ջ��continue
		* 3. top.id==Inputptr->id && top.id!=endc ��ջ����һ������
		* 4. is_Vn(top.id) ����ʹ���ĸ�����ʽ����ջ����error
		*/
		if (top.id == endc && Inputptr->id == endc) {
			string info = "��" + to_string(loopcnt) + "����ջ��Ϊ��" + top.value +
				"������Ϊ��" + Inputptr->value + "�������ɹ�";
			ps_log.push_back(info);
			return;
		}
		else if (top.id == eps) {
			string info = "��" + to_string(loopcnt) + "����ջ��Ϊ��" + top.value +
				"������Ϊ��" + Inputptr->value + ", ����";
			ps_log.push_back(info);
			StackPop();
		}
		else if (top.id == Inputptr->id && top.id != endc) {
			string info = "��" + to_string(loopcnt) + "����ջ��Ϊ��" + top.value +
				"������Ϊ��" + Inputptr->value + "����ջ";
			StackPop(*Inputptr);
			Inputptr++;
			ps_log.push_back(info);
		}
		else if(top.id=="$Vn") {
			//ȡ������ʽ
            Production prod = ps_Grammar->PredProduction(top.value, Inputptr->id);
			//Լ����Ϊ�յ�ʱ���Ǵ������
			if (prod.left == "") {
				ps_error = true;
				//ps_errorinfo = string("�﷨���󣺳����ڵ�") + to_string(Inputptr->line) +
				//	"�е���" + Inputptr->value + "����";
				ps_errorinfo = string("�﷨���󣺳����ڵ���") + Inputptr->value + "����";
				return;
			}
			string info = "��" + to_string(loopcnt) + "����ջ��Ϊ��" + top.value +
				"������Ϊ��" + Inputptr->value + "��ʹ�ò���ʽ��" + prod.str();
			addProd(prod, index);
			ps_log.push_back(info);
		}
		else {
			ps_error = true;
			ps_errorinfo = string("�﷨���󣺳����ڵ���") + Inputptr->value + "����";
			return;
		}
		loopcnt++;
	}
}

//������
void Parser::test() {
	execute();
	if (ps_error) {
		cout << "����ʧ��" << endl;
		cout << ps_errorinfo << endl;
	}
	else {
		cout << "�����ɹ�" << endl;
	}
	//�����־
	if (0) {
		cout << "ִ�в���" << endl;
		for (string line : ps_log) {
			cout << line << endl;
		}
	}
	//���ʹ�õĲ���ʽdfs
	if (1) {
		vector<vector<Word> > ParserResult;
        ps_Tree->dfs_export(ParserResult);
		cout << "dfsչʾʹ�õĲ���ʽ" << endl;
		for (auto list : ParserResult) {
			for (auto ptr = list.begin(); ptr != list.end(); ptr++) {
				cout << ptr->str() << " ";
				if (ptr == list.begin())
					cout << "-> ";
			}
			cout << endl;
		}
	}
	//���ʹ�õĲ���ʽbfs
	if (1) {
		vector<vector<vector<Word> > > ParserResult;
        ps_Tree->bfs_export(ParserResult);
		cout << "bfsչʾʹ�õĲ���ʽ" << endl;
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
	//�������ʽ���dfs
	if (0) {
		vector<vector<int> > ParserResult;
        ps_Tree->dfs_export(ParserResult);
		cout << "dfsչʾ����ʽ��ű�ʾ" << endl;
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
