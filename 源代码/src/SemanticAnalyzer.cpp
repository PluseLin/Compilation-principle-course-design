#include "SemanticAnalyzer.h"
using namespace std;

/* 非成员函数 */
void getdim(vector<string>& dim, ArrayPart* p_ArrayPart) {
	ArrayPart* p = p_ArrayPart;
	while (p && p->is_array) {
		string value = p->value;
		dim.push_back(value);
		p = p->p_ArrayPart;
	}
}

/* Quat成员变量 */
//返回字符串形式
string Quat::str() {
	return "< " + op + " , " + argv1 + " , " + argv2 + " , " + result + " >";
}

AttrTable::~AttrTable(){
    for(auto pr:a_PtrTable){
        int id=pr.first;
        if(a_IdTable[id]=="AddExpr"){
            auto ptr=(AddExpr*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="AddExpr1"){
            auto ptr=(AddExpr1*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="ArgsList"){
            auto ptr=(ArgsList*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="ArgsList1"){
            auto ptr=(ArgsList1*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Argument"){
            auto ptr=(Argument*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="ArrayDecl"){
            auto ptr=(ArrayDecl*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="ArrayPart"){
            auto ptr=(ArrayPart*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="AssignCode"){
            auto ptr=(AssignCode*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="CallList"){
            auto ptr=(CallList*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Code"){
            auto ptr=(Code*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="CodeBlock"){
            auto ptr=(CodeBlock*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="CodeStr"){
            auto ptr=(CodeStr*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Decl"){
            auto ptr=(Decl*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="ElseCode"){
            auto ptr=(ElseCode*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Expr"){
            auto ptr=(Expr*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="FPara"){
            auto ptr=(FPara*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Factor"){
            auto ptr=(Factor*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Factor1"){
            auto ptr=(Factor1*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="FuncDecl"){
            auto ptr=(FuncDecl*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="IfCode"){
            auto ptr=(IfCode*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="InState"){
            auto ptr=(InState*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="InVarDecl"){
            auto ptr=(InVarDecl*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="MulExpr"){
            auto ptr=(MulExpr*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="MulExpr1"){
            auto ptr=(MulExpr1*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Program"){
            auto ptr=(Program*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="RPara"){
            auto ptr=(RPara*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="RParaList"){
            auto ptr=(RParaList*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="RParaList1"){
            auto ptr=(RParaList1*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="Relop"){
            auto ptr=(Relop*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="RelopExpr"){
            auto ptr=(RelopExpr*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="ReturnCode"){
            auto ptr=(ReturnCode*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="ReturnCode1"){
            auto ptr=(ReturnCode1*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="State"){
            auto ptr=(State*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="StateStr"){
            auto ptr=(StateStr*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="VarDecl"){
            auto ptr=(VarDecl*)pr.second;
            delete ptr;
        }
        if(a_IdTable[id]=="WhileCode"){
            auto ptr=(WhileCode*)pr.second;
            delete ptr;
        }

    }
}

/* SemanticAnalyzer成员变量 */

//纠错用函数
//检查变量数组重命名
bool SemanticAnalyzer::checkVarDecl(string var) {
	for (auto pr : sa_VarTable) {
		if (pr.second.name == var) {
            if (pr.second.field == "current" || pr.second.field == "$public") {
				sa_error = true;
				sa_errorinfo += "语义分析错误：变量" + var + "重命名\n";
				return false;
			}
		}
	}
	return true;
}

//检查变量或数组是否未命名
bool SemanticAnalyzer::checkVarUse(string var) {
	int no = findVarNode(var, "current");
	if (no == -1) {
		sa_error = true;
		sa_errorinfo += "语义错误：变量: " + var + "未声明\n";
		return false;
	}
	return true;
}

//检查数组维数是否不一致
bool SemanticAnalyzer::checkArrayDim(string name,
									 vector<string>& use,
									 vector<int>& decl) {

	if (use.size() != decl.size()) {
		sa_error = true;
		sa_errorinfo += "语义错误：数组:" + name + "的维数出现错误\n";
		return false;
	}
	return true;
}

//检查函数是否未声明
bool SemanticAnalyzer::checkFuncUse(string name) {
	int no = findFuncNode(name);
	if (no == -1) {
		sa_error = true;
		sa_errorinfo += "语义分析错误：函数:" + name + "未声明!\n";
		return false;
	}
	return true;
}

//检查函数返回值是否为void
bool SemanticAnalyzer::checkFuncRet(string name) {
	if (checkFuncUse(name) == false) return false;
	int no = findFuncNode(name);
	FuncNode node = sa_FuncTable[no];
	if (node.type == "$void") return false;
	return true;
}

//检查函数是否重命名
bool SemanticAnalyzer::checkFuncDecl(string name) {
	if (findFuncNode(name) != -1) {
		sa_error = true;
		sa_errorinfo += "语义分析错误：函数:" + name + "重命名\n";
		return false;
	}
	return true;
}

//检查函数参数是否不一致
bool SemanticAnalyzer::checkFuncArgc(string name, int argc) {
	int no = findFuncNode(name);
	FuncNode node = sa_FuncTable[no];
	if (node.argc != argc) {
		sa_error = true;
		sa_errorinfo += "语义分析错误！调用函数:" + name + "参数数量与定义不一致！\n";
		return false;
	}
	return true;
}

//检查函数返回值与变量声明是否一致
bool SemanticAnalyzer::checkFuncType(string name, bool has_ret) {
	bool RetInt = checkFuncRet(name);//返回int时为true
	if (has_ret ^ RetInt) {
		sa_error = true;
		sa_errorinfo += "语义分析错误！函数:" + name + "返回类型与return不一致！";
		return false;
	}
	return true;
}

//构造函数
SemanticAnalyzer::SemanticAnalyzer(Parser& ps) {
    sa_error=false;
    load(ps);
}



//清空函数
void SemanticAnalyzer::clear() {
	sa_AttrTable.a_IdTable.clear();
	sa_AttrTable.a_PtrTable.clear();
	sa_WordList.clear();
	sa_QuatList.clear();
    sa_VarTable.clear();
    sa_FuncTable.clear();
}

//载入函数
void SemanticAnalyzer::load(Parser& ps) {
    ps.ps_Tree->dfs_export(sa_ProdIndex);
    //sa_WordList.swap(ps.ps_Tree->GetWordList());
    vector<Word>& temp=ps.ps_Tree->GetWordList();
    sa_WordList.assign(temp.begin(),temp.end());
}

//导出为txt文件
void SemanticAnalyzer::export_to_txt(const char* dst) {
	ofstream fout(dst);
	vector<string> file;
	
	if (!fout.is_open()) {
		cerr << "Export SemanticAnalyzer file: " << dst << " fail!" << endl;
		exit(-1);
	}

	for (auto q : sa_QuatList) {
		file.push_back(q.str());
	}

	for (auto pr : sa_FuncTable) {
		int pos = pr.second.startpos;
		string name = pr.second.name;
		file[pos] += " //" + name;
	}

	int cnt = 0;
	for (auto line : file) {
		fout << cnt++ << "\t" << line << endl;
	}
	fout.close();
}

bool SemanticAnalyzer::is_error(){
    return this->sa_error;
}

std::string SemanticAnalyzer::geterrorinfo(){
    return this->sa_errorinfo;
}

//执行函数
void SemanticAnalyzer::execute() {
	Analy();
}

//总分析函数
void SemanticAnalyzer::Analy() {
	//约定入口是main函数，且约定main函数无参数，因此在最开头加一条jump 
	addQuat(Quat{ "jal","_","_","main" });
	addQuat(Quat{ "halt","_","_","_" });
	for (auto Prod : sa_ProdIndex) {
		//取左部
		string left = sa_WordList[Prod[0]].value;
		//调用子分析函数
		auto func = sa_SubFuncMap[left];
		(this->*func)(Prod);
		if (sa_error) return;
	}
	//获取main函数入口，若无，跳至末尾
	int Quatcnt = 0;
	bool has_main = false;
	for (auto pr : sa_FuncTable) {
		FuncNode fn = pr.second;
		if (fn.name == "main") {
			has_main = true; break;
		}
	}
	if (!has_main) {
		updateQuat(0, "result", "");
	}
}

void SemanticAnalyzer::addAttr(void* ptr, int index) {
	sa_AttrTable.a_PtrTable[index] = ptr;
	sa_AttrTable.a_IdTable[index] = sa_WordList[index].value;
}

//封装将四元式加入到四元式序列的操作
void SemanticAnalyzer::addQuat(Quat q) {
	sa_QuatList.push_back(q);
}

void SemanticAnalyzer::insertQuat(Quat q, int pos) {
	sa_QuatList.insert(sa_QuatList.begin() + pos, q);
}

//获取新的临时变量
std::string SemanticAnalyzer::NewTemp() {
	return "%T" + to_string(tempcnt++);
}

void SemanticAnalyzer::updateQuat(int Quatpos, string member, string value) {
	if (member == "op") sa_QuatList[Quatpos].op = value;
	else if (member == "argv1") sa_QuatList[Quatpos].argv1 = value;
	else if (member == "argv2") sa_QuatList[Quatpos].argv2 = value;
	else sa_QuatList[Quatpos].result = value;
}

//寻找符合条件的VarNode，并返回其在map的下标
int SemanticAnalyzer::findVarNode(std::string name, std::string field) {
	for (auto pr : sa_VarTable) {
        if (pr.second.name == name && (pr.second.field == field || pr.second.field == "$public")) {
			return pr.first;
		}
	}
	return -1;
}


//寻找符合条件的Funcnode，并返回其在map的下标
int SemanticAnalyzer::findFuncNode(string name) {
	for (auto pr : sa_FuncTable) {
		if (pr.second.name == name)
			return pr.first;
	}
	return -1;
}

//测试函数
void SemanticAnalyzer::test() {
	execute();
	//输出变量表
	if (1) {
		cout << (sa_error ? "语义分析出错！" : "语义分析成功！") << endl;
		if (sa_error) {
			cout << sa_errorinfo << endl;
			return;
		}

	}
	if (1) {
		cout << "函数表:" << endl;
		for (auto pr : sa_FuncTable) {
			cout << pr.first << " ";
			cout << "函数名: " << pr.second.name<<"\t";
			cout << "返回类型: " << pr.second.type<<"\t";
			cout << "参数个数: " << pr.second.argc << "\t";
			cout << "首地址:" << pr.second.startpos << "\t";
			cout << "长度:" << pr.second.Quatcnt << "\t";
			cout << endl;
		}
	}
	//输出函数表
	if (1) {
		cout << "变量表:" << endl;
		for (auto pr : sa_VarTable) {
			cout << pr.first << " ";
			cout << "变量名: " << pr.second.name << "\t";
			cout << "变量类型: " << pr.second.type << "\t";
			cout << "作用域: " << pr.second.field << "\t";
			cout << "第几个局部变量:" << pr.second.Varcnt << "\t";
			//cout << "维数:" << pr.second.width.size() << "\t";
			cout << "各维宽度: ";
			for (auto w : pr.second.width) cout << w << ",";
			cout << '\b'<<" \t";
			cout << endl;
		}
	}
	//输出四元式序列
	if (1) {
		cout << "四元式序列:" << endl;
		int cnt = 0;
		for (auto quat : sa_QuatList) {
			cout << cnt++ << "\t" << quat.str() << endl;
		}
		cout << endl;
	}

	if (1) {
		export_to_txt("Quat.txt");
		cout << "中间代码已导出至同目录下 Quat.txt 处" << endl;
		cout << endl;
	}
}

//子分析函数
void SemanticAnalyzer::sa_AddExpr(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	AddExpr* left = new AddExpr();

	//AddExpr -> MulExpr AddExpr1
	left->p_MulExpr = (MulExpr*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
	left->p_AddExpr1 = (AddExpr1*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	if (left->p_AddExpr1->op != "") {
		left->value = NewTemp();
		left->Quatcnt = left->p_AddExpr1->Quatcnt + left->p_MulExpr->Quatcnt + 1;
		Quat temp{ left->p_AddExpr1->op,left->p_MulExpr->value,left->p_AddExpr1->value,left->value };
		left->startpos = min({ left->p_AddExpr1->startpos,left->p_MulExpr->startpos,int(sa_QuatList.size()) });
		addQuat(temp);
	}
	else {
		left->value = left->p_MulExpr->value;
		left->Quatcnt = left->p_MulExpr->Quatcnt;
		left->startpos = left->p_MulExpr->startpos;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_AddExpr1(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	AddExpr1* left = new AddExpr1();

	//AddExpr1 -> $eps
	if (Prod[1].value == "$eps") {
		left->op = "", left->value = "", left->Quatcnt = 0;
		left->p_MulExpr = nullptr, left->p_AddExpr1 = nullptr;
	}
	//AddExpr1 -> $+ MulExpr AddExpr1
	//AddExpr1 -> $- MulExpr AddExpr1
	else {
		left->p_MulExpr = (MulExpr*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->p_AddExpr1 = (AddExpr1*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
		left->op = Prod[1].value;
		//右部AddExpr1有运算，则此步生成一个四元式
		if (left->p_AddExpr1->op != "") {
			left->value = NewTemp();
			left->Quatcnt = left->p_AddExpr1->Quatcnt + left->p_MulExpr->Quatcnt + 1;
			Quat temp{ left->p_AddExpr1->op,left->p_MulExpr->value,left->p_AddExpr1->value,left->value};
			left->startpos = min({ left->p_AddExpr1->startpos,left->p_MulExpr->startpos,int(sa_QuatList.size()) });
			addQuat(temp);
		}
		else {
			left->value = left->p_MulExpr->value;
			left->Quatcnt = left->p_MulExpr->Quatcnt;
			left->startpos = left->p_MulExpr->startpos;
		}
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_ArgsList(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	ArgsList* left = new ArgsList();

	//ArgsList -> Argument ArgsList1
	left->p_ArgList1 = (ArgsList1*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	left->p_Argument = (Argument*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
	
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_ArgsList1(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	ArgsList1* left = new ArgsList1();

	//ArgsList1 -> $, Argument ArgsList1
	//ArgsList1 -> $eps
	if (Prod[1].value == "$eps") {
		left->p_ArgList1 = nullptr, left->p_Argument = nullptr;
	}
	else {
		left->p_ArgList1 = (ArgsList1*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
		left->p_Argument = (Argument*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Argument(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Argument* left = new Argument();

	//Argument -> $int $id
	left->name = Prod[2].value;
	left->type = "int";
	left->Argcnt = --sa_Argcnt;

	sa_VarTable[ProdIndex[0]] = VarNode{
		left->name,left->type,"current",left->Argcnt,vector<int>()
	};

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_ArrayDecl(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	ArrayDecl* left = new ArrayDecl();

	//ArrayDecl -> $[ $const $] ArrayDecl
	//ArrayDecl -> $eps
	if (Prod[1].value == "$eps") {
		left->p_ArrayDecl = nullptr;
		left->width = -1;
	}
	else {
		left->width = stoi(Prod[2].value);
		left->p_ArrayDecl = (ArrayDecl*)sa_AttrTable.a_PtrTable[ProdIndex[4]];
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_ArrayPart(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	ArrayPart* left = new ArrayPart();

	//ArrayPart -> $eps
	//ArrayPart -> $[ Expr $] ArrayPart
	if (Prod[1].value == "$eps") {
		left->is_array = false;
		left->value = "";
	}
	else {
		left->is_array = true;
		left->p_ArrayPart = (ArrayPart*)sa_AttrTable.a_PtrTable[ProdIndex[4]];
		auto p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->startpos = min(left->p_ArrayPart->startpos, p_Expr->startpos);
		left->Quatcnt = left->p_ArrayPart->Quatcnt + p_Expr->Quatcnt;
		left->value = p_Expr->value;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_AssignCode(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	AssignCode* left = new AssignCode();
	//AssignCode -> $id ArrayPart $= Expr
	left->value = Prod[1].value;
	left->p_ArrayPart = (ArrayPart*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	left->p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[4]];
	if (checkVarUse(left->value) == false) {
		return;
	}
	if (left->p_ArrayPart->is_array == false) {
		Quat temp{ "mov", left->p_Expr->value,"_",left->value };
		left->startpos = min(left->p_Expr->startpos, int(sa_QuatList.size()));
		left->Quatcnt = left->p_Expr->Quatcnt + 1;
		addQuat(temp);
	}
	else {
		//完成纠错时补充此部分内容
		left->p_ArrayPart = (ArrayPart*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		//找全维数并检查
		vector<string> dim;
		getdim(dim, left->p_ArrayPart);

		//依次生成计算位移的公式
		string offset = NewTemp();
		//检查维数
		int no = findVarNode(left->value, "current");
		vector<int>& width = sa_VarTable[no].width;
		if (checkArrayDim(left->value, dim, width)==false)
			return;
		int Quatcnt = 0;
		for (int i = 0; i < dim.size(); i++) {
			if (i == 0) {
				left->startpos = int(sa_QuatList.size());
				addQuat(Quat{ "mov",dim[i],"_",offset });
			}
			else {
				addQuat(Quat{ "*",offset,to_string(width[i]),offset });
				addQuat(Quat{ "+",offset,dim[i],offset});
			}
			Quatcnt += 2;
		}
		//left->value = NewTemp();
		addQuat(Quat{ "*",offset,"4",offset });
		addQuat(Quat{ "store",left->value,offset,left->p_Expr->value });
		left->Quatcnt = left->p_Expr->Quatcnt + Quatcnt + 1;
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_CallList(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	CallList* left = new CallList();

	//CallList -> $( RPara $)
	left->p_RPara = (RPara*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	left->startpos = left->p_RPara->startpos;
	left->Quatcnt = left->p_RPara->Quatcnt;
	left->argc = left->p_RPara->argc;

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Code(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Code* left = new Code();
	//Code -> IfCode
	//Code -> WhileCode
	//Code -> ReturnCode $;
	//Code -> AssignCode $;
	left->p_AnyCode = sa_AttrTable.a_PtrTable[ProdIndex[1]];
	if (Prod[1].value == "IfCode") {
		left->Quatcnt= ((IfCode*)left->p_AnyCode)->Quatcnt;
		left->startpos= ((IfCode*)left->p_AnyCode)->startpos;
	}
	else if (Prod[1].value == "WhileCode") {
		left->Quatcnt = ((WhileCode*)left->p_AnyCode)->Quatcnt;
		left->startpos = ((WhileCode*)left->p_AnyCode)->startpos;
	}
	else if (Prod[1].value == "ReturnCode") {
		left->Quatcnt = ((ReturnCode*)left->p_AnyCode)->Quatcnt;
		left->startpos= ((ReturnCode*)left->p_AnyCode)->startpos;
	}
	else {//AssignCode
		left->Quatcnt = ((AssignCode*)left->p_AnyCode)->Quatcnt;
		left->startpos = ((AssignCode*)left->p_AnyCode)->startpos;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_CodeBlock(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	CodeBlock* left = new CodeBlock();
	//CodeBlock -> ${ InState CodeStr $}
	//处理InState
	
	//处理CodeStr
	left->p_CodeStr = sa_AttrTable.a_PtrTable[ProdIndex[3]];
	CodeStr* p_CodeStr = (CodeStr*)left->p_CodeStr;
	/*p_CodeStr->startpos = cur_startpos;*/
	
	cur_startpos += p_CodeStr->Quatcnt;
	left->Quatcnt = p_CodeStr->Quatcnt;
	left->startpos = p_CodeStr->startpos;
	addAttr(left, ProdIndex[0]);
	
}

void SemanticAnalyzer::sa_CodeStr(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	CodeStr* left = new CodeStr();

	//CodeStr -> Code CodeStr
	//CodeStr -> $eps
	if (Prod[1].value == "$eps") {
		left->Quatcnt = 0;
		left->startpos = 0x7fffffff;
		left->p_CodeStr = nullptr;
	}
	else {
		left->p_CodeStr = (CodeStr*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->p_Code = (Code*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
		left->Quatcnt = left->p_Code->Quatcnt + left->p_CodeStr->Quatcnt;
		left->startpos = min(left->p_Code->startpos,left->p_CodeStr->startpos);
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Decl(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Decl* left = new Decl();
	void* decl= sa_AttrTable.a_PtrTable[ProdIndex[1]];
	left->declare = decl;


	//Decl -> VarDecl
	//Decl -> FuncDecl
	if (Prod[1].value == "VarDecl") {
		left->declType = left->declType;
		if (((VarDecl*)left->declare)->p_ArrayDecl == nullptr)
			left->declType = VAR;
		else
			left->declType = ARRAY;
	}
	else if (Prod[1].value == "FuncDecl") {
		left->declType = FUNC;
	}
	//else {
	//	left->declType = ARRAY;
	//}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_ElseCode(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	ElseCode* left = new ElseCode();

	//ElseCode -> $else CodeBlock
	//ElseCode -> $eps
	if (Prod[1].id == "$else") {
		left->has_else = true;
		left->p_ElseCB = sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->Quatcnt = ((CodeBlock*)left->p_ElseCB)->Quatcnt;
		left->startpos= ((CodeBlock*)left->p_ElseCB)->startpos;
	}
	else {
		left->has_else = false;
		left->p_ElseCB = nullptr;
		left->Quatcnt = 0;
		left->startpos = 0x7fffffff;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Expr(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Expr* left = new Expr();
	//Expr -> AddExpr RelopExpr

	auto p_AddExpr = (AddExpr*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
	auto p_RelopExpr = (RelopExpr*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	//若RelopExpr的op非空，则需要生成一个四元式
	if (p_RelopExpr->op != "") {
		left->value = NewTemp();
		left->Quatcnt = p_AddExpr->Quatcnt + p_RelopExpr->Quatcnt + 1;
		Quat temp{ p_RelopExpr->op,p_AddExpr->value,p_RelopExpr->value,left->value };
		left->startpos = min({ p_AddExpr->startpos,p_RelopExpr->startpos,int(sa_QuatList.size()) });
		addQuat(temp);
	}
	else {
		left->Quatcnt = p_AddExpr->Quatcnt;
		left->value = p_AddExpr->value;
		left->startpos = p_AddExpr->startpos;
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_FPara(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	FPara* left = new FPara();

	//FPara -> ArgsList
	//FPara -> $void
	//FPara -> $eps
	if (Prod[1].value == "ArgsList") {
		left->argc = -sa_Argcnt;
	}
	else {
		left->argc = 0;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Factor(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Factor* left = new Factor();

	//Factor -> $const
	//Factor -> $( Expr $)
	//Factor -> $id Factor1
	if (Prod[1].id == "$const") {
		left->p_Factor1 = nullptr;
		left->value = Prod[1].value;
		left->type = VAR;
		left->startpos = 0x7fffffff;
	}
	else if (Prod[1].id == "$(") {
		left->p_Factor1 = nullptr;
		auto p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->value = p_Expr->value;
		left->Quatcnt = p_Expr->Quatcnt;
		left->startpos = p_Expr->startpos;
		left->type = VAR;
	}
	else {
		left->p_Factor1 = (Factor1*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		string name = Prod[1].value;
		if (left->p_Factor1->type != FUNC) {
			if (checkVarUse(Prod[1].value) == false)
				return;
		}
		else {
			if (checkFuncUse(name) == false)
				return;
			if (checkFuncRet(name) == false) {
				sa_error = true;
				sa_errorinfo += "语义分析错误：函数:" + name + 
					"的类型是void，不能用于表达式\n";
				return;
			}
		}
		if (left->p_Factor1->type == VAR) {
			
			left->value = Prod[1].value;
		}
		//实现函数引用和数组时补全
		else if(left->p_Factor1->type==FUNC) {
			//完成纠错时补充
			//Factor1 -> CallList;
			auto p_CallList = (CallList*)left->p_Factor1->p_Any;
			if (checkFuncArgc(Prod[1].value, p_CallList->argc) == false)
				return;
			addQuat(Quat{ "jal","_","_",Prod[1].value });
			left->startpos = min(left->p_Factor1->startpos, int(sa_QuatList.size()));
			left->Quatcnt = left->p_Factor1->Quatcnt + 1;
			left->value = "EAX";
		}
		else {
			//Factor -> $id Factor1 -> $id ArrayPart
			auto p_ArrayPart = (ArrayPart*)left->p_Factor1->p_Any;
			left->value = Prod[1].value;
			int no = findVarNode(left->value, "current");
			//找全维数并检查
			vector<string> dim;
			getdim(dim, p_ArrayPart);

			//依次生成计算位移的公式
			string offset = NewTemp();
			//取出各维宽度
			vector<int>& width = sa_VarTable[no].width;
			if (checkArrayDim(left->value, dim, width) == false)
				return;
			int Quatcnt = 0;
			for (int i = 0; i < dim.size(); i++) {
				if (i == 0) {
					left->startpos = int(sa_QuatList.size());
					addQuat(Quat{ "mov",dim[i],"_",offset });
				}
				else {
					addQuat(Quat{ "*",offset,to_string(width[i]),offset });
					addQuat(Quat{ "+",offset,dim[i],offset });
				}
				Quatcnt += 2;
			}
			left->value = NewTemp();
			addQuat(Quat{ "*",offset,"4",offset });
			addQuat(Quat{ "load",Prod[1].value,offset,left->value });
			left->Quatcnt = Quatcnt + 1;
		}
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Factor1(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Factor1* left = new Factor1();

	left->p_Any = sa_AttrTable.a_PtrTable[ProdIndex[1]];
	//Factor1 -> CallList
	//Factor1 -> ArrayPart
	if (Prod[1].value == "CallList") {
		left->type = FUNC;
		left->startpos = ((CallList*)left->p_Any)->startpos;
		left->Quatcnt = ((CallList*)left->p_Any)->Quatcnt;
	}
	else {
		if (((ArrayPart*)left->p_Any)->is_array == false) {
			left->type = VAR;
		}
		else {
			left->type = ARRAY;
			//实现数组时补全
		}
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_FuncDecl(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	FuncDecl* left = new FuncDecl();

	//FuncDecl -> $( FPara $) CodeBlock
	//处理FPara
	FPara* p_FPara = (FPara*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	left->argc = p_FPara->argc;
	//
	left->p_CodeBlock = (CodeBlock*)sa_AttrTable.a_PtrTable[ProdIndex[4]];
	
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_IfCode(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	IfCode* left = new IfCode();


	//IfCode -> $if $( Expr $) CodeBlock ElseCode
	left->p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
	left->p_IfCB= sa_AttrTable.a_PtrTable[ProdIndex[5]];
	left->p_ElseCode= (ElseCode*)sa_AttrTable.a_PtrTable[ProdIndex[6]];
	left->startpos = left->p_Expr->startpos;
	left->Quatcnt = left->p_Expr->Quatcnt + ((CodeBlock*)left->p_IfCB)->Quatcnt
		+ left->p_ElseCode->Quatcnt + 2;
	//插入三条jump
	//前两条，对应if条件的jump，一条跳入IfCode，另一条跳入ElseCode
	//后一条在if中跳出else
	//本实验未实现and or not逻辑运算，因此不需要真假链，只需要判断Expr数值是否为1即可
	Quat JumpIf{ "beq",left->p_Expr->value,"1","+2" };
	insertQuat(JumpIf, left->p_Expr->startpos + left->p_Expr->Quatcnt);
	int adder = (left->p_ElseCode->has_else) ? 2 : 1;
	Quat JumpElse{ "j","_","_","+" + to_string(((CodeBlock*)left->p_IfCB)->Quatcnt+adder) };
	insertQuat(JumpElse, left->p_Expr->startpos + left->p_Expr->Quatcnt + 1);
	if (left->p_ElseCode->has_else) {
		Quat JumpOut{ "j","_","_","+" + to_string(left->p_ElseCode->Quatcnt+1) };
		insertQuat(JumpOut, left->p_Expr->startpos + left->p_Expr->Quatcnt + ((CodeBlock*)left->p_IfCB)->Quatcnt + 2);
		left->Quatcnt++;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_InState(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	InState* left = new InState();

	//InState -> $eps
	if (Prod[1].value == "$eps") {
		left->p_InState = nullptr;
	}
	//InState -> InVarDecl $; InState
	else {
		left->p_InState = (InState*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_InVarDecl(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	InVarDecl* left = new InVarDecl();

	//InVarDecl -> $int $id ArrayDecl
	ArrayDecl* ap = (ArrayDecl*)(sa_AttrTable.a_PtrTable[ProdIndex[3]]);
	left->name = sa_WordList[ProdIndex[2]].value;
	left->p_ArrayDecl = ap;
	if (checkVarDecl(left->name) == false) {
		return;
	}
	if (left->p_ArrayDecl->p_ArrayDecl!=nullptr) {
		left->type = ARRAY;
	}
	else {
		left->type = VAR;
	}
	left->VarNum = ++sa_Varcnt;
    sa_VarTable[ProdIndex[0]] = VarNode{
		left->name,left->type,"current",left->VarNum,vector<int>()
	};
	ArrayDecl* p = left->p_ArrayDecl;
	int cnt = 1;
	while (p && p->p_ArrayDecl) {
		cnt *= p->width;
		sa_VarTable[ProdIndex[0]].width.push_back(p->width);
		p = p->p_ArrayDecl;
	}
	sa_Varcnt += cnt - 1;
	//sa_VarTable[ProdIndex[0]].Varcnt = left->VarNum;
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_MulExpr(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	MulExpr* left = new MulExpr();

	//MulExpr -> Factor MulExpr1
	left->p_Factor = (Factor*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
	left->p_MulExpr1 = (MulExpr1*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	//MulExpr1有op，要生成四元式
	if (left->p_MulExpr1->op != "") {
		left->Quatcnt = left->p_MulExpr1->Quatcnt + 1;
		left->value = NewTemp();
		Quat temp{ left->p_MulExpr1->op,left->p_Factor->value,left->p_MulExpr1->value,left->value };
		left->startpos = min({ left->p_Factor->startpos,left->p_MulExpr1->startpos,int(sa_QuatList.size()) });
		addQuat(temp);
	}
	else {
		left->Quatcnt = left->p_Factor->Quatcnt;
		left->value = left->p_Factor->value;
		left->startpos = left->p_Factor->startpos;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_MulExpr1(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	MulExpr1* left = new MulExpr1();

	//MulExpr1 -> $* Factor MulExpr1
	//MulExpr1 -> $/ Factor MulExpr1
	//MulExpr1 -> $eps
	if (Prod[1].id == "$eps") {
		left->p_Factor = nullptr;left->p_MulExpr1 = nullptr;
		left->op = "";left->value = "";left->Quatcnt = 0;
	}
	else {
		left->p_Factor = (Factor*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->p_MulExpr1 = (MulExpr1*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
		MulExpr1* p_MulExpr1 = left->p_MulExpr1;
		left->op = Prod[1].value;
		//后面的MulExpr的op非空，则应该生成四元式
		//用Factor和MulExpr1生成
		if (p_MulExpr1->op != "") {
			string argv1 = left->p_Factor->value;
			string argv2 = p_MulExpr1->value;
			string result = NewTemp();
			left->value = result;	//记下此MulExpr1所使用的值
			left->Quatcnt = p_MulExpr1->Quatcnt + left->p_Factor->Quatcnt + 1;//产生四元式数多1
			left->startpos = min({ left->p_Factor->startpos,p_MulExpr1->startpos,int(sa_QuatList.size()) });
			addQuat(Quat{ p_MulExpr1->op,argv1,argv2,result });
		}
		//后面的MulExpr为空，不生成四元式
		else {
			left->value = left->p_Factor->value;
			left->Quatcnt = left->p_Factor->Quatcnt;
			left->startpos = left->p_Factor->startpos;
		}
	}


	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Program(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Program* left = new Program();

	//Program -> StateStr

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_RPara(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	RPara* left = new RPara();

	//RPara -> RParaList
	//RPara -> $eps
	if (Prod[1].value == "$eps") {
		left->p_RParaList = nullptr;
	}
	else {
		left->p_RParaList =(RParaList*) sa_AttrTable.a_PtrTable[ProdIndex[1]];
		left->startpos = left->p_RParaList->startpos;
		left->Quatcnt = left->p_RParaList->Quatcnt;
		left->argc = left->p_RParaList->argc;
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_RParaList(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	RParaList* left = new RParaList();

	//RParaList -> Expr RParaList1
	left->p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
	left->p_RParaList1 = (RParaList1*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	left->Quatcnt = ((Expr*)left->p_Expr)->Quatcnt + left->p_RParaList1->Quatcnt + 1;
	left->startpos = min({ ((Expr*)left->p_Expr)->startpos, left->p_RParaList1->startpos
						 ,int(sa_QuatList.size()) });
	left->argc = left->p_RParaList1->argc + 1;
	addQuat(Quat{ "push", ((Expr*)left->p_Expr)->value, "_", "_" });
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_RParaList1(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	RParaList1* left = new RParaList1();

	//RParaList1 -> $, Expr RParaList1
	//RParaList1 -> $eps
	if (Prod[1].value == "$eps") {
		left->p_Expr = nullptr; left->p_RParaList1 = nullptr;
		left->argc = 0;
	}
	else {
		left->p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->p_RParaList1 = (RParaList1*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
		//入栈
		left->Quatcnt = ((Expr*)left->p_Expr)->Quatcnt + left->p_RParaList1->Quatcnt + 1;
		left->startpos = min({ ((Expr*)left->p_Expr)->startpos, left->p_RParaList1->startpos
							 ,int(sa_QuatList.size()) });
		left->argc = left->p_RParaList1->argc + 1;
		addQuat(Quat{ "push", ((Expr*)left->p_Expr)->value, "_", "_" });
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_Relop(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	Relop* left = new Relop();
	//Relop -> $<
	left->op = Prod[1].value;
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_RelopExpr(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	RelopExpr* left = new RelopExpr();

	//RelopExpr -> Relop AddExpr
	//RelopExpr -> $eps
	if (Prod[1].value == "$eps") {
		left->op = "", left->p_AddExpr = nullptr, left->value = "";
		left->Quatcnt = 0;
	}
	else {
		auto p_Relop = (Relop*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
		left->op = p_Relop->op;
		left->p_AddExpr = (AddExpr*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
		left->value = left->p_AddExpr->value;
		left->Quatcnt = left->p_AddExpr->Quatcnt;
		left->startpos = left->p_AddExpr->startpos;
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_ReturnCode(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	ReturnCode* left = new ReturnCode();

	//ReturnCode -> $return ReturnCode1
	auto p_ReturnCode1 = (ReturnCode1*)sa_AttrTable.a_PtrTable[ProdIndex[2]];
	left->Quatcnt = p_ReturnCode1->Quatcnt;
	left->startpos = p_ReturnCode1->startpos;
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_ReturnCode1(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	ReturnCode1* left = new ReturnCode1();
	//ReturnCode1 -> Expr
	//ReturnCode1 -> $eps
	if (Prod[1].value == "$eps") {
		left->Quatcnt = 1, left->value = "";
		left->startpos = sa_QuatList.size();
		addQuat(Quat{ "jret","_","_","_" });
	}
	else {
		auto p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
		left->Quatcnt = p_Expr->Quatcnt+2, left->value = p_Expr->value;
		left->startpos = min(p_Expr->startpos, int(sa_QuatList.size()));
		addQuat(Quat{ "mov",left->value,"_","EAX" });
		addQuat(Quat{ "jret","_","_","_" });
		sa_ReturnInt = true;
	}
	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_State(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	State* left = new State();

	//State -> int $id Decl
	//State -> void $id FuncDecl
	if (Prod[1].value == "int") {
		//取出Decl结构体
		Decl* d = (Decl*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
		//填充State结构体相对应内容
		left->DeclType = d->declType;
		left->name = Prod[2].value;
		left->ValueType = "$int";
		//如果声明是函数，需要填充函数特有内容
		if (d->declType == FUNC) {
			FuncDecl* p_FuncDecl = (FuncDecl*) d->declare;
			left->argc = p_FuncDecl->argc;
			CodeBlock* p_CodeBlock = (CodeBlock*)p_FuncDecl->p_CodeBlock;
			left->startpos = p_CodeBlock->startpos;
			left->Quatcnt = p_CodeBlock->Quatcnt;
		}
	}
	else {
		auto p_FuncDecl = (FuncDecl*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
		CodeBlock* p_CodeBlock = (CodeBlock*)p_FuncDecl->p_CodeBlock;
		left->DeclType = FUNC;
		left->name = Prod[2].value;
		left->ValueType = "$void";
		left->argc = p_FuncDecl->argc;
		left->startpos = p_CodeBlock->startpos;
		left->Quatcnt = p_CodeBlock->Quatcnt;
	}

	if (left->DeclType == FUNC) {
		if (checkFuncDecl(left->name) == false)
			return;
		sa_FuncTable[ProdIndex[0]] = FuncNode();
		FuncNode& fnode = sa_FuncTable[ProdIndex[0]];
		fnode.name = left->name;
		fnode.type = left->ValueType;
		fnode.argc = left->argc;
		fnode.startpos = left->startpos;
		fnode.Quatcnt = left->Quatcnt;
		if (checkFuncType(fnode.name, sa_ReturnInt) == false)
			return;
		sa_ReturnInt = false;
		//遍历变量表，将field中的current改为left->name
		for (auto p = sa_VarTable.begin(); p != sa_VarTable.end(); p++) {
			if ((p->second).field == "current") {
				(p->second).field = left->name;
			}
		}
		sa_Varcnt = sa_Argcnt = 0;
        //检查函数调用有无错误
        /*
        if (checkFuncUse(name) == false)
            return;
        if (checkFuncRet(name) == false) {
            sa_error = true;
            sa_errorinfo += "语义分析错误：函数:" + name +
                "的类型是void，不能用于表达式\n";
            return;
        }
        */
	}
	else {
		//State -> int $id Decl 
		//Decl-> Vardecl 
        //全局变量
		auto p_Decl = (Decl*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
		int VarNum = ++sa_PublicCnt;
		sa_VarTable[ProdIndex[2]] = VarNode{
            left->name,left->DeclType,"$public",VarNum,vector<int>()
		};
		if (p_Decl->declType == ARRAY) {
			int cnt = 1;
			ArrayDecl* decl = ((VarDecl*)(p_Decl->declare))->p_ArrayDecl;
			ArrayDecl* p = decl;
			while (p && p->p_ArrayDecl) {
				cnt *= p->width;
				sa_VarTable[ProdIndex[2]].width.push_back(p->width);
				p = p->p_ArrayDecl;
			}
			sa_Varcnt += cnt - 1;
		}
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_StateStr(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	StateStr* left = new StateStr();

	//StateStr -> State StateStr
	//StateStr -> $eps

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_VarDecl(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	VarDecl* left = new VarDecl();

	//VarDecl -> ArrayDecl $;
	if (Prod[1].value == "ArrayDecl") {
		left->p_ArrayDecl = (ArrayDecl*)sa_AttrTable.a_PtrTable[ProdIndex[1]];
	}
	else {
		left->p_ArrayDecl = nullptr;
	}

	addAttr(left, ProdIndex[0]);
}

void SemanticAnalyzer::sa_WhileCode(vector<int>& ProdIndex) {
	vector<Word> Prod;
	for (auto index : ProdIndex) {
		Prod.push_back(sa_WordList[index]);
	}
	WhileCode* left = new WhileCode();

	//WhileCode -> $while $( Expr $) CodeBlock
	left->p_Expr = (Expr*)sa_AttrTable.a_PtrTable[ProdIndex[3]];
	left->p_CodeBlock = sa_AttrTable.a_PtrTable[ProdIndex[5]];

	CodeBlock* p_CodeBlock = (CodeBlock*)left->p_CodeBlock;
	left->startpos = left->p_Expr->startpos;
	left->Quatcnt = left->p_Expr->Quatcnt + p_CodeBlock->Quatcnt + 3;

	//要加三条语句
	//其中CB前面加两条，一个是跳入CB，一个是跳出CB
	//CB后面加两条，回到Expr判断
	Quat JumpIn{ "beq",left->p_Expr->value,"1","+2" };
	Quat JumpOut{ "j","_","_","+" + to_string(p_CodeBlock->Quatcnt + 2) };
	Quat jumpBack{ "j","_","_","-" + to_string(p_CodeBlock->Quatcnt + left->p_Expr->Quatcnt + 2) };
	insertQuat(JumpIn, left->p_Expr->startpos + left->p_Expr->Quatcnt);
	insertQuat(JumpOut, left->p_Expr->startpos + left->p_Expr->Quatcnt + 1);
	insertQuat(jumpBack, p_CodeBlock->Quatcnt + p_CodeBlock->startpos + 2);

	addAttr(left, ProdIndex[0]);
}



