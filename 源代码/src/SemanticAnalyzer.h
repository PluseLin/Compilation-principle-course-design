#pragma once
#include<initializer_list>
#include"Parser.h"
#include"Attrbute.h"

#define ARRAY "int[]"
#define VAR	"int"
#define FUNC "function"

//辅助函数，生成各维位移
void getdim(std::vector<std::string>& dim, ArrayPart* p_ArrayPart);


//四元式结构体
struct Quat {
	std::string op;
	std::string argv1;
	std::string argv2;
	std::string result;
	Quat(std::string op, std::string argv1, std::string argv2, std::string result)
		:op(op), argv1(argv1), argv2(argv2), result(result){}
	Quat(){}
	std::string str();
};

//存放属性结构体的表
struct AttrTable {
	int cnt = 0;
	std::map<int, void*> a_PtrTable;
	std::map<int, std::string> a_IdTable;
    ~AttrTable();
};

//语义分析器
class SemanticAnalyzer{
	friend class AsmGenerator;
private:
	//产生式序列（下标表示） Input
	std::vector<std::vector<int> > sa_ProdIndex;
	//单词序列 Input
	std::vector<Word> sa_WordList;
	//中间代码 Output
	std::vector<Quat> sa_QuatList;
	//属性结构体表
	AttrTable sa_AttrTable;

	//变量名表
	std::map<int, VarNode> sa_VarTable;
	
	//函数名表
	std::map<int, FuncNode> sa_FuncTable;


	//当前语句块四元式首地址，初始为1
	//0是跳入main函数
	int cur_startpos = 2;
	
	//当前语义分析使用的临时变量编号
	//使用为%T+编号
	int tempcnt = 1;
	
	//当前局部变量统计，一个函数声明块被翻译完后还原此值
	//本次实验中，暂不支持标准C语言中的声明嵌套
	int sa_Varcnt = 0;
	int sa_PublicCnt = 0;
	//当前函数参数统计，一个函数参数列表翻译完后还原此值
	//为了与实参区分，形参计数从-1开始，这也有利于栈帧中的处理
	//本次实验中，暂不支持标准C语言中的实参形参重名
	int sa_Argcnt = 0;
	//错误信息
    bool sa_error=false;
	std::string sa_errorinfo;
	//寻找符合条件的VarNode，并返回其在map的下标
	int findVarNode(std::string name, std::string field);
	//寻找符合条件的Funcnode，并返回其在map的下标
	int findFuncNode(std::string name);

	typedef void (SemanticAnalyzer::* SubFunc)(std::vector<int>&);

	//记录是否返回int
	bool sa_ReturnInt = false;


private:
	std::map<std::string, SubFunc> sa_SubFuncMap{
		{"AddExpr",&SemanticAnalyzer::sa_AddExpr},
		{"AddExpr1",&SemanticAnalyzer::sa_AddExpr1},
		{"ArgsList",&SemanticAnalyzer::sa_ArgsList},
		{"ArgsList1",&SemanticAnalyzer::sa_ArgsList1},
		{"Argument",&SemanticAnalyzer::sa_Argument},
		{"ArrayDecl",&SemanticAnalyzer::sa_ArrayDecl},
		{"ArrayPart",&SemanticAnalyzer::sa_ArrayPart},
		//{"ArrayPart1",&SemanticAnalyzer::sa_ArrayPart1},
		{"AssignCode",&SemanticAnalyzer::sa_AssignCode},
		{"CallList",&SemanticAnalyzer::sa_CallList},
		{"Code",&SemanticAnalyzer::sa_Code},
		{"CodeBlock",&SemanticAnalyzer::sa_CodeBlock},
		{"CodeStr",&SemanticAnalyzer::sa_CodeStr},
		{"Decl",&SemanticAnalyzer::sa_Decl},
		{"ElseCode",&SemanticAnalyzer::sa_ElseCode},
		{"Expr",&SemanticAnalyzer::sa_Expr},
		{"FPara",&SemanticAnalyzer::sa_FPara},
		{"Factor",&SemanticAnalyzer::sa_Factor},
		{"Factor1",&SemanticAnalyzer::sa_Factor1},
		{"FuncDecl",&SemanticAnalyzer::sa_FuncDecl},
		{"IfCode",&SemanticAnalyzer::sa_IfCode},
		{"InState",&SemanticAnalyzer::sa_InState},
		{"InVarDecl",&SemanticAnalyzer::sa_InVarDecl},
		{"MulExpr",&SemanticAnalyzer::sa_MulExpr},
		{"MulExpr1",&SemanticAnalyzer::sa_MulExpr1},
		{"Program",&SemanticAnalyzer::sa_Program},
		{"RPara",&SemanticAnalyzer::sa_RPara},
		{"RParaList",&SemanticAnalyzer::sa_RParaList},
		{"RParaList1",&SemanticAnalyzer::sa_RParaList1},
		{"Relop",&SemanticAnalyzer::sa_Relop},
		{"RelopExpr",&SemanticAnalyzer::sa_RelopExpr},
		{"ReturnCode",&SemanticAnalyzer::sa_ReturnCode},
		{"ReturnCode1",&SemanticAnalyzer::sa_ReturnCode1},
		{"State",&SemanticAnalyzer::sa_State},
		{"StateStr",&SemanticAnalyzer::sa_StateStr},
		{"VarDecl",&SemanticAnalyzer::sa_VarDecl},
		{"WhileCode",&SemanticAnalyzer::sa_WhileCode},
	};

private:
	//总分析程序，根据不同的左部调用不同的子分析函数
	void Analy();

	//封装加入属性表的操作
	void addAttr(void* ptr, int index);

	//封装将四元式加入到四元式序列的操作
	void addQuat(Quat q);
	void insertQuat(Quat q, int pos);
	//获取新的临时变量
	std::string NewTemp();
	//修改Quat的某一值
	void updateQuat(int Quatpos, std::string member,std::string result);

	//针对不同左部的子分析函数，内部再拆分
	//形式 void xxx(std::vector<int>& ProdIndex);
	void sa_AddExpr(std::vector<int>& ProdIndex);
	void sa_AddExpr1(std::vector<int>& ProdIndex);
	void sa_ArgsList(std::vector<int>& ProdIndex);
	void sa_ArgsList1(std::vector<int>& ProdIndex);
	void sa_Argument(std::vector<int>& ProdIndex);
	void sa_ArrayDecl(std::vector<int>& ProdIndex);
	void sa_ArrayPart(std::vector<int>& ProdIndex);
	//void sa_ArrayPart1(std::vector<int>& ProdIndex);
	void sa_AssignCode(std::vector<int>& ProdIndex);
	void sa_CallList(std::vector<int>& ProdIndex);
	void sa_Code(std::vector<int>& ProdIndex);
	void sa_CodeBlock(std::vector<int>& ProdIndex);
	void sa_CodeStr(std::vector<int>& ProdIndex);
	void sa_Decl(std::vector<int>& ProdIndex);
	void sa_ElseCode(std::vector<int>& ProdIndex);
	void sa_Expr(std::vector<int>& ProdIndex);
	void sa_FPara(std::vector<int>& ProdIndex);
	void sa_Factor(std::vector<int>& ProdIndex);
	void sa_Factor1(std::vector<int>& ProdIndex);
	void sa_FuncDecl(std::vector<int>& ProdIndex);
	void sa_IfCode(std::vector<int>& ProdIndex);
	void sa_InState(std::vector<int>& ProdIndex);
	void sa_InVarDecl(std::vector<int>& ProdIndex);
	void sa_MulExpr(std::vector<int>& ProdIndex);
	void sa_MulExpr1(std::vector<int>& ProdIndex);
	void sa_Program(std::vector<int>& ProdIndex);
	void sa_RPara(std::vector<int>& ProdIndex);
	void sa_RParaList(std::vector<int>& ProdIndex);
	void sa_RParaList1(std::vector<int>& ProdIndex);
	void sa_Relop(std::vector<int>& ProdIndex);
	void sa_RelopExpr(std::vector<int>& ProdIndex);
	void sa_ReturnCode(std::vector<int>& ProdIndex);
	void sa_ReturnCode1(std::vector<int>& ProdIndex);
	void sa_State(std::vector<int>& ProdIndex);
	void sa_StateStr(std::vector<int>& ProdIndex);
	void sa_VarDecl(std::vector<int>& ProdIndex);
	void sa_WhileCode(std::vector<int>& ProdIndex);

private:
	//纠错用
	//检查变量是否重命名
	bool checkVarDecl(std::string var);
	//检查变量是否未命名
	bool checkVarUse(std::string var);
	//检查数组维数是否不一致
	bool checkArrayDim(std::string name,
					   std::vector<std::string>& use,
					   std::vector<int>& decl);
	//检查函数是否未声明
	bool checkFuncUse(std::string name);
	//检查函数返回值是否为void
	bool checkFuncRet(std::string name);
	//检查函数是否重命名
	bool checkFuncDecl(std::string name);
	//检查函数参数是否不一致
	bool checkFuncArgc(std::string name, int argc);
	//检查函数返回值与变量声明是否一致
	bool checkFuncType(std::string name, bool has_ret);
public:
    bool is_error();

    std::string geterrorinfo();

	//构造函数
	SemanticAnalyzer(Parser& ps);
	//载入函数
	void load(Parser& ps);
	//执行函数
	void execute();
	//清空函数
	void clear();
	//导出为txt文件
	void export_to_txt(const char* dst);
	//测试用
	void test();

    std::vector<Quat> GetQuatList(){
        return this->sa_QuatList;
    }

    std::map<int, FuncNode> GetFuncTable(){
        return this->sa_FuncTable;
    }

    std::map<int, VarNode> GetVarTable(){
        return this->sa_VarTable;
    }
};

