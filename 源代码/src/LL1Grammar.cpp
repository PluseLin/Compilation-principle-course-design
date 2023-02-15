#include "LL1Grammar.h"
using namespace std;

/* �ǳ�Ա���� */

//�ж��Ƿ�Ϊ�ս��
bool is_Vt(std::string str) {
	//Լ���ս����$��ͷ
	return str.length() > 0 && str[0] == '$';
}

//�ж��Ƿ�Ϊ���ս��
bool is_Vn(std::string str) {
	return str.length() > 0 && isalpha(str[0]);
}

//�ָ��ַ���
std::vector<std::string> split(std::string str, char sp) {
	vector<string> res;
	string temp;
	for (char c : str) {
		if (c == sp) {
			res.push_back(temp); temp = "";
		}
		else {
			temp += c;
		}
	}
	if (temp != "") res.push_back(temp);
	return res;
}

/* Production��Ա���� */

std::string Production::str() {
	string res;
	res += (left + " -> ");
	for (string word : right) res += (word + " ");
	return res;
}

Production::~Production(){
    right.clear();
}

/* FirstSet��Ա���� */

//���캯��
FirstSet::FirstSet(Vn left) {
	this->left = left;
}

FirstSet::FirstSet() {

}

FirstSet::FirstSet(const FirstSet& fs) {
	left = fs.left;
	_set.insert(fs._set.begin(), fs._set.end());
}

FirstSet::~FirstSet(){
    _set.clear();
}

//����=�����
FirstSet& FirstSet::operator=(const FirstSet& fs) {
	left = fs.left;
	_set.insert(fs._set.begin(), fs._set.end());
	return *this;
}

//��ӷ��շ�
void FirstSet::addVt(const FirstSet& fs) {
	this->_set.insert(fs._set.begin(), fs._set.end());
}

//��ӷ��շ�
void FirstSet::addVt(Vt vt) {
	this->_set.insert(vt);
}

void FirstSet::addVt(set<Vt>& VtSet) {
	_set.insert(VtSet.begin(), VtSet.end());
}

//����ɾȥĳ�����ս���Ľ��
FirstSet FirstSet::erase(Vt vt) {
	FirstSet fs = *this;
	fs._set.erase(vt);
	return fs;
}

//��First������ָ�����ʣ�һ����eps
bool FirstSet::findword(std::string word) {
	return _set.find(word) != _set.end();
}


/* FollowSet��Ա���� */

//���캯��
FollowSet::FollowSet(Vn left) {
	this->left = left;
}

FollowSet::FollowSet() {

}

FollowSet::FollowSet(const FollowSet& fs) {
	left = fs.left;
	_set.insert(fs._set.begin(), fs._set.end());
}

FollowSet::~FollowSet(){
    _set.clear();
}

//����=�����
FollowSet& FollowSet::operator=(const FollowSet& fs) {
	left = fs.left;
	_set.insert(fs._set.begin(), fs._set.end());
	return *this;
}

//��ӷ��ս��
void FollowSet::addVt(const FirstSet& fs) {
	_set.insert(fs._set.begin(), fs._set.end());
}

void FollowSet::addVt(const FollowSet& fs) {
	_set.insert(fs._set.begin(), fs._set.end());
}

void FollowSet::addVt(std::set<Vt>VtSet, std::string erase) {
	VtSet.erase(erase);
	_set.insert(VtSet.begin(), VtSet.end());
}

/* LL1Grammar��Ա���� */

//�����ķ�����ʼ������ʽ���շ����շ�����
void LL1Grammar::ImportGrammar(const char* fp) {
	ifstream fin(fp);
	if (!fin.is_open()) {
		cerr << "Open LL1Grammar File " << fp << " fail!";
		exit(-1);
	}
	string line;
	//��ȡ����ʽ����ʼ������ʽ���շ����Ϻͷ��շ�����
	while (!fin.eof()) {
		getline(fin, line);
		if (line.length() == 0 || line[0] == '#') continue;
		vector<string> parts = split(line);
		Production p;
		//����ʽ��ʽΪ A -> $id ��һ��Ϊ���շ����ڶ������࣬������Ϊ�ս��
		for (int i = 0; i < parts.size(); i++) {
			if (i == 1) continue;
			if (i == 0) {
				p.left = parts[i];
			}
			else if (i >= 2) {
				p.right.push_back(parts[i]);
			}
			if (is_Vt(parts[i]))	g_VtSet.insert(parts[i]);
			else 					g_VnSet.insert(parts[i]);
		}
		g_ProductionList.push_back(p);
	}
	
	//�����ʼ����
	if (g_ProductionList.size() > 0)
		StartVn = g_ProductionList[0].left;
	//���ú�First���Ϻ�Follow����
	for (Vn vn : g_VnSet) {
		g_FirstMap.insert({ vn,FirstSet(vn) });
		g_FollowMap.insert({ vn,FollowSet(vn) });
	}
	//��¼λ��
	string prevVn = "";
	int count = 0;
	for (Production p : g_ProductionList) {
		if (prevVn != p.left) {
			g_ProdIndex[p.left] = { count,count + 1 };
			prevVn = p.left;
		}
		else {
			g_ProdIndex[p.left].second += 1;
		}
		count++;
	}
	fin.close();
}

//ȡ����Ϊvn�����в���ʽ
std::vector<Production> LL1Grammar::GetProduction(Vn vn) {
	vector<Production> res;
	auto begin = g_ProductionList.begin()+g_ProdIndex[vn].first;
	auto end = g_ProductionList.begin() + g_ProdIndex[vn].second;
	res.assign(begin, end);
	return res;
}

//�������������ڳ�ʼ��First
void LL1Grammar::dfs_GetFirst(Vn vn, std::map<Vn, bool>& record) {
	//��eps�ı�־
	bool has_eps = false;
	//�������꣬��������
	if (record[vn] == true) return;
	//ȡ�����в���ʽ
	vector<Production> list = GetProduction(vn);
	//һһ����
	for (Production p : list) {
		//������ʽ�Ҳ�
		for (string word : p.right) {
			//������Vt������涼���ÿ��ˣ�ֱ������
			if (is_Vt(word)) {
				g_FirstMap[vn].addVt(word);
				break;
			}
			//��Vn������Ҫ���ҳ���First������eps���������
			else {
				if (record[word] == false) dfs_GetFirst(word, record);
				//��ȥeps�����
				set<Vt> temp = g_FirstMap[word]._set;
				temp.erase(eps);
				g_FirstMap[vn].addVt(temp);
				has_eps = g_FirstMap[word].findword(eps);
				//����eps��break
				if (!has_eps) break;
			}
		}
	}
	//�鿴�Ƿ���eps
	if (has_eps) g_FirstMap[vn].addVt(eps);
	//��¼Ϊ����
	record[vn] = true;
}

//�������������ڳ�ʼ��Follow
void LL1Grammar::dfs_GetFollow(Vn vn,map<Vn,set<Vn> >& Need,map<Vn, bool>& record) {
	set<Vn>& tempNeed = Need[vn];
	for (Vn eachvn : tempNeed) {
		if (!record[eachvn]) dfs_GetFollow(eachvn, Need, record);
		g_FollowMap[vn].addVt(g_FollowMap[eachvn]);
	}
	record[vn] = true;
}

//��ʼ��First
void LL1Grammar::InitFirst() {
	/*����ĳ����ʽ��������ʽΪA -> a1 a2 a3��ÿ��aiΪ�շ�����շ�
	* ����a1��FIRST�����룬���a1��FIRST��$eps�ټ���a2
	* ���a1��vt����ܺð졣��ȻҪ��a1���������û���������a1
	* ���Ϲ�����dfsʵ��
	*/
	map<Vn, bool> record;//��¼�Ƿ�����first
	for (Vn vn : g_VnSet) record[vn] = false;
	for (Vn vn : g_VnSet) {
		dfs_GetFirst(vn, record);
	}
}

//��ʼ��Follow
void LL1Grammar::InitFollow() {
	/*�����ս����Follow����Ҫ�������в���ʽ�������
	* ��һ�����������в���ʽ���������� a -> Ab��A�Ƿ��շ���b�Ǿ��ͣ���ʱ��Ҫ��
	* First(b)��ȥeps���뵽Follow��A��
	* ���������a -> A������a ->Ab����b->+ eps��Ҫ��Follow(a)���뵽Follow��A)
	* ���п���Follow(a)�ں����ı��������л�Ҫ��ӣ����Ҫ�Ӻ������Գ�֮Ϊ��
	* һ��������ϵ�����ǽ�����������ϵ��map������
	* �ڶ��������ݵ�һ����õ�������ϵ��dfs�ķ�ʽ���
	*/
	//���ڼ�¼������ϵ map[A]={B,C}��ʾA��ҪB��C
	//������B��C������A
	map<Vn, set<Vn> > Need;
	for (Vn vn : g_VnSet) Need[vn] = set<Vn>();
	
	g_FollowMap[StartVn].addVt(set<Vt>{endc});

	//��һ��������ÿ������ʽ
	for (Production p : g_ProductionList) {
		Vn left = p.left;
		for (auto ptr = p.right.begin(); ptr != p.right.end(); ptr++) {
			//�����Vn����Ҫ��������First��ȥeps���뵽Follow��
			//��Ȼ�����epsҪ�ȱ������
			bool has_eps = false;//�Ƿ���Ҫ��¼�����ı�־��
			if (is_Vn(*ptr)) {
				vector<string> words(ptr + 1, p.right.end());
				auto temp = First(words);
				if (find(temp.begin(), temp.end(), eps) != temp.end()) has_eps = true;
				g_FollowMap[*ptr].addVt(temp, "$eps");
				//���������ĩβ�ˣ����ߺ󲿷ֵ�first����eps������Ҫ��¼������ϵ
				if ((ptr == p.right.end() - 1 || has_eps) && *ptr != left) {
					Need[*ptr].insert(left);
				}
			}
		}
	}
	//�ڶ��� ��������ϵ���Follow
	map<Vn, bool> record;
	for (Vn vn : g_VnSet) record[vn] = false;
	for (Vn vn : g_VnSet)
		dfs_GetFollow(vn, Need, record);
}

//��ʼ��PredTable
void LL1Grammar::InitPredTable() {
	for (Vn vn : g_VnSet) {
		for (Vt vt : g_VtSet) {
			g_PredTable[{vn, vt}] = -1;
		}
	}
	//����ÿһ������ʽ
	for (int i = 0; i < g_ProductionList.size(); i++) {
		auto p = g_ProductionList[i];
		//�������ʽ�Ҳ�����eps
		//����ÿһ������ʽ���Ҳ�p������First
		//���� a ���� First(p)��Table[left][a]=p��index
		auto RightFirst = First(p);
		for (Vt vt : RightFirst) {
			g_PredTable[{p.left, vt}] = i;
		}
		//����β���ʽ��first����eps����Ҫ��follow
		//���� a ���� Follow(left) Table[left][a]=p��index
		if (RightFirst.find(eps) != RightFirst.end()) {
			auto RightFollow = Follow(p.left);
			for (Vt vt : RightFollow) {
				g_PredTable[{p.left, vt}] = i;
			}
		}
	}
}

//First Follow �ӿ�
std::set<Vt> LL1Grammar::First(string word) {
	set<Vt> res;
	if (is_Vt(word)) res.insert(word);
	else {
		res = g_FirstMap[word]._set;
	}
	return res;
}

std::set<Vt> LL1Grammar::First(vector<string>& list) {
	set<Vt> res;
	bool has_eps = false;
	for (string word : list) {
		set<Vt>temp = First(word);
		has_eps = find(temp.begin(), temp.end(), eps) != temp.end();
		temp.erase(eps);
		res.insert(temp.begin(), temp.end());
		if (!has_eps) break;
	}
	if (has_eps) res.insert(eps);
	return res;
}

std::set<Vt> LL1Grammar::First(Production& p) {
	set<Vt> res;
	res = First(p.right);
	return res;
}

std::set<Vt> LL1Grammar::Follow(Vn vn) {
	set<Vt> res;
	auto temp = g_FollowMap[vn]._set;
	res.insert(temp.begin(), temp.end());
	return res;
}

//Ԥ�������ӿ�
Production LL1Grammar::PredProduction(Vn StackTop, Vt InputWord) {
	int index = g_PredTable[{StackTop, InputWord}];
	//�±�Ϊ-1���ʾδ�ҵ������ؿղ���ʽ���ж�ʱ�����ж�
	if (index == -1) {
		return Production();
	}
	return g_ProductionList[index];
}

//���캯��
LL1Grammar::LL1Grammar() {

}

LL1Grammar::~LL1Grammar(){
    g_ProductionList.clear();
    g_ProdIndex.clear();
    g_VtSet.clear();
    g_VnSet.clear();
    g_FirstMap.clear();
    g_FollowMap.clear();
    g_PredTable.clear();
}

//��ʼ��
void LL1Grammar::Init(const char* fp) {
	ImportGrammar(fp);
	InitFirst();
	InitFollow();
	InitPredTable();
}

bool LL1Grammar::is_LL1() {	
	//��֤ͬһ�����շ��Ĳ���ʽ�Ҳ���First�Ƿ��г�ͻ
	//��֤���� A -> $epsʱ��First��Follow�Ƿ��ͻ
	bool flag = true;
	map<Vt, bool> mp;
	for (Vn vn : g_VnSet) {
		for (Vt vt : g_VtSet) mp[vt] = false;
		auto prodlist = GetProduction(vn);
		for (Production each : prodlist) {
			set<Vt> ProdFirst = First(each);
			for (Vt vt : ProdFirst) {
				if (mp[vt] == true) {
					cout << "��鵽 " << vn << " ��������ʽ��ͻ��" << endl;
					flag=false;
				}
				mp[vt] = true;
			}
			if (each.right[0] == eps) {
				set<Vt> VnFirst = First(vn);
				set<Vt> VnFollow = Follow(vn);
				set<Vt> Union;
				Union.insert(VnFirst.begin(), VnFirst.end());
				Union.insert(VnFollow.begin(), VnFollow.end());
				if (Union.size() != VnFirst.size() + VnFollow.size()) {
					cout << "��鵽 " << vn << " ����First��Follow��ͻ��" << endl;
					flag = false;
				}
			}
		}
	}
	return flag;
}

//������
void LL1Grammar::test() {
	//����շ�����
	if (1) {
		cout << "�շ����ϣ�����:" << g_VtSet.size() << endl;
		for (Vt vt : g_VtSet) {
			cout << vt << " ";
		}
		cout << endl << endl;
	}
	//������շ�����
	if (1) {
		cout << "���շ����ϣ�����:" << g_VnSet.size() << endl;
		for (Vn vn : g_VnSet) {
			cout << vn << " , ";
		}
		cout << endl << endl;
	}
	//�������ʽ
	if (0) {
		for (Vn vn : g_VnSet) {
			auto list = GetProduction(vn);
			for (Production p : list) {
				cout << p.left << " -> ";
				for (string str : p.right) {
					cout << str << " ";
				}
				cout << endl;
			}
			cout << endl;
		}
	}
	//����Ƿ�ΪLL1
	if (1) {
		if (is_LL1())
			cout << "��֤��ϣ���LL1�ķ���" << endl;
	}
	//���First��Follow
	if (1) {
		cout << "First���Ϻ�Follow���ϣ�" << endl;
		for (Vn vn : g_VnSet) {
			auto temp1 = First(vn);
			auto temp2 = Follow(vn);
			cout <<"First "<< vn << " : ";
			for (string word : temp1) {
				cout << word << " ";
			}
			cout << endl;
			cout << "Follow " << vn << " : ";
			for (string word : temp2) {
				cout << word << " ";
			}
			
			cout << endl;
		}
		cout << endl;
	}
	//���Ԥ��������ļ����������ͬĿ¼�£���Ϊ"PredTable.csv"��
	if (1) {
		ofstream fout("Predtable.csv");
		if (!fout.is_open()) {
			cerr << "Open Predtable.csv Fail!" << endl;
			exit(-1);
		}
		fout << " ,";
		for (Vt vt : g_VtSet) {
			fout << vt << ",";
		}
		fout << endl;
		for (Vn vn : g_VnSet) {
			fout << vn << ",";
			for (Vt vt : g_VtSet) {
				auto p = PredProduction(vn, vt);
				if (p.left == "") {
					fout << "/,"; continue;
				}
				fout << p.left << " -> ";
				for (string word : p.right) {
					fout << word << " ";
				}
				fout << ",";
			}
			fout << endl;
		}
		fout.close();
		cout << "Ԥ������������ .\\Predtable.csv Ŀ¼�£�" << endl;
	}
}

