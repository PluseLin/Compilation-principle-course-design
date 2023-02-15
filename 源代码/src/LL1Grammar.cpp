#include "LL1Grammar.h"
using namespace std;

/* 非成员函数 */

//判断是否为终结符
bool is_Vt(std::string str) {
	//约定终结符以$开头
	return str.length() > 0 && str[0] == '$';
}

//判断是否为非终结符
bool is_Vn(std::string str) {
	return str.length() > 0 && isalpha(str[0]);
}

//分割字符串
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

/* Production成员函数 */

std::string Production::str() {
	string res;
	res += (left + " -> ");
	for (string word : right) res += (word + " ");
	return res;
}

Production::~Production(){
    right.clear();
}

/* FirstSet成员函数 */

//构造函数
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

//重载=运算符
FirstSet& FirstSet::operator=(const FirstSet& fs) {
	left = fs.left;
	_set.insert(fs._set.begin(), fs._set.end());
	return *this;
}

//添加非终符
void FirstSet::addVt(const FirstSet& fs) {
	this->_set.insert(fs._set.begin(), fs._set.end());
}

//添加非终符
void FirstSet::addVt(Vt vt) {
	this->_set.insert(vt);
}

void FirstSet::addVt(set<Vt>& VtSet) {
	_set.insert(VtSet.begin(), VtSet.end());
}

//返回删去某个非终结符的结果
FirstSet FirstSet::erase(Vt vt) {
	FirstSet fs = *this;
	fs._set.erase(vt);
	return fs;
}

//找First中有无指定单词，一般是eps
bool FirstSet::findword(std::string word) {
	return _set.find(word) != _set.end();
}


/* FollowSet成员函数 */

//构造函数
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

//重载=运算符
FollowSet& FollowSet::operator=(const FollowSet& fs) {
	left = fs.left;
	_set.insert(fs._set.begin(), fs._set.end());
	return *this;
}

//添加非终结符
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

/* LL1Grammar成员函数 */

//导入文法，初始化产生式，终符非终符集合
void LL1Grammar::ImportGrammar(const char* fp) {
	ifstream fin(fp);
	if (!fin.is_open()) {
		cerr << "Open LL1Grammar File " << fp << " fail!";
		exit(-1);
	}
	string line;
	//读取产生式并初始化产生式，终符集合和非终符集合
	while (!fin.eof()) {
		getline(fin, line);
		if (line.length() == 0 || line[0] == '#') continue;
		vector<string> parts = split(line);
		Production p;
		//产生式格式为 A -> $id 第一个为非终符，第二个多余，第三个为终结符
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
	
	//标记起始符号
	if (g_ProductionList.size() > 0)
		StartVn = g_ProductionList[0].left;
	//设置好First集合和Follow集合
	for (Vn vn : g_VnSet) {
		g_FirstMap.insert({ vn,FirstSet(vn) });
		g_FollowMap.insert({ vn,FollowSet(vn) });
	}
	//记录位置
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

//取出左部为vn的所有产生式
std::vector<Production> LL1Grammar::GetProduction(Vn vn) {
	vector<Production> res;
	auto begin = g_ProductionList.begin()+g_ProdIndex[vn].first;
	auto end = g_ProductionList.begin() + g_ProdIndex[vn].second;
	res.assign(begin, end);
	return res;
}

//辅助函数，用于初始化First
void LL1Grammar::dfs_GetFirst(Vn vn, std::map<Vn, bool>& record) {
	//有eps的标志
	bool has_eps = false;
	//若已求完，不必再求
	if (record[vn] == true) return;
	//取出所有产生式
	vector<Production> list = GetProduction(vn);
	//一一遍历
	for (Production p : list) {
		//看产生式右部
		for (string word : p.right) {
			//遍历到Vt，则后面都不用看了，直接跳出
			if (is_Vt(word)) {
				g_FirstMap[vn].addVt(word);
				break;
			}
			//是Vn，则需要先找出其First，若有eps则继续遍历
			else {
				if (record[word] == false) dfs_GetFirst(word, record);
				//除去eps后加入
				set<Vt> temp = g_FirstMap[word]._set;
				temp.erase(eps);
				g_FirstMap[vn].addVt(temp);
				has_eps = g_FirstMap[word].findword(eps);
				//若无eps则break
				if (!has_eps) break;
			}
		}
	}
	//查看是否有eps
	if (has_eps) g_FirstMap[vn].addVt(eps);
	//记录为求完
	record[vn] = true;
}

//辅助函数，用于初始化Follow
void LL1Grammar::dfs_GetFollow(Vn vn,map<Vn,set<Vn> >& Need,map<Vn, bool>& record) {
	set<Vn>& tempNeed = Need[vn];
	for (Vn eachvn : tempNeed) {
		if (!record[eachvn]) dfs_GetFollow(eachvn, Need, record);
		g_FollowMap[vn].addVt(g_FollowMap[eachvn]);
	}
	record[vn] = true;
}

//初始化First
void LL1Grammar::InitFirst() {
	/*对于某产生式，设其形式为A -> a1 a2 a3，每个ai为终符或非终符
	* 先求a1的FIRST并加入，如果a1的FIRST有$eps再加入a2
	* 如果a1是vt，则很好办。不然要看a1有无求出，没求出则先求a1
	* 以上过程用dfs实现
	*/
	map<Vn, bool> record;//记录是否求完first
	for (Vn vn : g_VnSet) record[vn] = false;
	for (Vn vn : g_VnSet) {
		dfs_GetFirst(vn, record);
	}
}

//初始化Follow
void LL1Grammar::InitFollow() {
	/*对于终结符的Follow，需要遍历所有产生式才能求得
	* 第一步：遍历所有产生式，遇到形如 a -> Ab（A是非终符，b是句型）的时候要将
	* First(b)除去eps加入到Follow（A）
	* 如果遇到了a -> A或者是a ->Ab但是b->+ eps就要把Follow(a)加入到Follow（A)
	* 但有可能Follow(a)在后续的遍历过程中还要添加，因此要延后处理。可以称之为是
	* 一种依赖关系，我们将这种依赖关系用map记下来
	* 第二步：根据第一步求得的依赖关系用dfs的方式填充
	*/
	//用于记录依赖关系 map[A]={B,C}表示A需要B，C
	//即填完B和C才能填A
	map<Vn, set<Vn> > Need;
	for (Vn vn : g_VnSet) Need[vn] = set<Vn>();
	
	g_FollowMap[StartVn].addVt(set<Vt>{endc});

	//第一步，遍历每个产生式
	for (Production p : g_ProductionList) {
		Vn left = p.left;
		for (auto ptr = p.right.begin(); ptr != p.right.end(); ptr++) {
			//如果是Vn，则要求出后面的First减去eps加入到Follow。
			//当然如果有eps要先标记下来
			bool has_eps = false;//是否需要记录依赖的标志。
			if (is_Vn(*ptr)) {
				vector<string> words(ptr + 1, p.right.end());
				auto temp = First(words);
				if (find(temp.begin(), temp.end(), eps) != temp.end()) has_eps = true;
				g_FollowMap[*ptr].addVt(temp, "$eps");
				//如果遍历到末尾了，或者后部分的first中有eps，则需要记录依赖关系
				if ((ptr == p.right.end() - 1 || has_eps) && *ptr != left) {
					Need[*ptr].insert(left);
				}
			}
		}
	}
	//第二步 用依赖关系填充Follow
	map<Vn, bool> record;
	for (Vn vn : g_VnSet) record[vn] = false;
	for (Vn vn : g_VnSet)
		dfs_GetFollow(vn, Need, record);
}

//初始化PredTable
void LL1Grammar::InitPredTable() {
	for (Vn vn : g_VnSet) {
		for (Vt vt : g_VtSet) {
			g_PredTable[{vn, vt}] = -1;
		}
	}
	//遍历每一个产生式
	for (int i = 0; i < g_ProductionList.size(); i++) {
		auto p = g_ProductionList[i];
		//如果产生式右部不是eps
		//对于每一个产生式的右部p，求其First
		//任意 a 属于 First(p)，Table[left][a]=p的index
		auto RightFirst = First(p);
		for (Vt vt : RightFirst) {
			g_PredTable[{p.left, vt}] = i;
		}
		//如果次产生式的first含有eps，需要求follow
		//任意 a 属于 Follow(left) Table[left][a]=p的index
		if (RightFirst.find(eps) != RightFirst.end()) {
			auto RightFollow = Follow(p.left);
			for (Vt vt : RightFollow) {
				g_PredTable[{p.left, vt}] = i;
			}
		}
	}
}

//First Follow 接口
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

//预测分析表接口
Production LL1Grammar::PredProduction(Vn StackTop, Vt InputWord) {
	int index = g_PredTable[{StackTop, InputWord}];
	//下标为-1则表示未找到，返回空产生式，判断时用左部判断
	if (index == -1) {
		return Production();
	}
	return g_ProductionList[index];
}

//构造函数
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

//初始化
void LL1Grammar::Init(const char* fp) {
	ImportGrammar(fp);
	InitFirst();
	InitFollow();
	InitPredTable();
}

bool LL1Grammar::is_LL1() {	
	//验证同一个非终符的产生式右部的First是否有冲突
	//验证存在 A -> $eps时，First和Follow是否冲突
	bool flag = true;
	map<Vt, bool> mp;
	for (Vn vn : g_VnSet) {
		for (Vt vt : g_VtSet) mp[vt] = false;
		auto prodlist = GetProduction(vn);
		for (Production each : prodlist) {
			set<Vt> ProdFirst = First(each);
			for (Vt vt : ProdFirst) {
				if (mp[vt] == true) {
					cout << "检查到 " << vn << " 处，产生式冲突！" << endl;
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
					cout << "检查到 " << vn << " 处，First与Follow冲突！" << endl;
					flag = false;
				}
			}
		}
	}
	return flag;
}

//测试用
void LL1Grammar::test() {
	//输出终符集合
	if (1) {
		cout << "终符集合，数量:" << g_VtSet.size() << endl;
		for (Vt vt : g_VtSet) {
			cout << vt << " ";
		}
		cout << endl << endl;
	}
	//输出非终符集合
	if (1) {
		cout << "非终符集合，数量:" << g_VnSet.size() << endl;
		for (Vn vn : g_VnSet) {
			cout << vn << " , ";
		}
		cout << endl << endl;
	}
	//输出产生式
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
	//检查是否为LL1
	if (1) {
		if (is_LL1())
			cout << "验证完毕，是LL1文法！" << endl;
	}
	//输出First与Follow
	if (1) {
		cout << "First集合和Follow集合：" << endl;
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
	//输出预测分析表，文件输出（放在同目录下，名为"PredTable.csv"）
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
		cout << "预测分析表输出到 .\\Predtable.csv 目录下！" << endl;
	}
}

