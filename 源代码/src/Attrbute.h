#include<iostream>
#include<string>
#include<vector>

struct VarNode {
	std::string name;//名称
	std::string type;//类型
	std::string field;//作用域
	int	Varcnt = -1;//第几个局部变量，若为函数参数则为小于0，否则大于0
	std::vector<int> width;//各维上限（非数组变量为空）
    VarNode(std::string name,std::string type,std::string field,int cnt,std::vector<int> width):
        name(name),type(type),field(field),Varcnt(cnt){
        this->width.swap(width);
    }
    VarNode(){}
    ~VarNode(){
        width.clear();
    }
};


struct FuncNode {
	std::string name;//名称
	std::string type;//返回类型
	int argc = 0;//参数数量
	int startpos = 0x7fffffff;//四元式起始位置
	int Quatcnt = 0;//四元式个数
};

//非终符RParaList1对应的结构体
//RParaList1 -> $, Expr RParaList1
//RParaList1 -> $eps
struct RParaList1 {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	int argc = 0;
	void* p_Expr = nullptr;
	RParaList1* p_RParaList1 = nullptr;
};

//非终符RParaList对应的结构体
//RParaList -> Expr RParaList1
struct RParaList {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	int argc = 0;
	void* p_Expr = nullptr;
	RParaList1* p_RParaList1 = nullptr;
};

//非终符RPara对应的结构体
//RPara -> RParaList
//RPara -> $eps
struct RPara {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	int argc = 0;
	RParaList* p_RParaList = nullptr;
};

//非终符ArrayPart对应的结构体
//ArrayPart -> $eps
//ArrayPart -> $[ Expr $] ArrayPart
struct ArrayPart {
	bool is_array = false;//有些不是array，也会涉及到此结构体
	std::string value;//当前维数位移变量

	int startpos = 0;
	int Quatcnt = 0x7fffffff;
	
	ArrayPart* p_ArrayPart = nullptr;
	
};

//非终符CallList对应的结构体
//CallList -> $( RPara $)
struct CallList {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	int argc = 0;
	RPara* p_RPara = nullptr;;
};
//非终符Factor1对应的结构体

//Factor1 -> CallList
//Factor1 -> ArrayPart
struct Factor1 {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	std::string type;//种类，调用/数组/空
	void* p_Any = nullptr;//CallList/ArrayPart
};

//非终符Factor对应的结构体
//Factor -> $const
//Factor -> $( Expr $)
//Factor -> $id Factor1
struct Factor {
	std::string value;//此Factor占用的值或变量
	std::string type;//类型（函数引用或数值）
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	Factor1* p_Factor1 = nullptr;
	//void* p_Expr
};

//非终符MulExpr1对应的结构体
//MulExpr1 -> $* Factor MulExpr1
//MulExpr1 -> $/ Factor MulExpr1
//MulExpr1 -> $eps
struct MulExpr1 {
	std::string op;//使用的运算符
	std::string value;//使用的数值/中间变量
	int startpos = 0x7fffffff;
	int Quatcnt = 0;//四元式计数

	Factor* p_Factor = nullptr;//对应的Factor
	MulExpr1* p_MulExpr1 = nullptr;//下一个MulExpr1
};

//非终符MulExpr对应的结构体
//MulExpr -> Factor MulExpr1
struct MulExpr {
	std::string value;//使用的数值/变量
	int Quatcnt = 0;//四元式计数
	int startpos = 0x7fffffff;//四元式起始位置

	Factor* p_Factor = nullptr;
	MulExpr1* p_MulExpr1 = nullptr;
};

//非终符AddExpr1对应的结构体
//AddExpr1 -> $+ MulExpr AddExpr1
//AddExpr1 -> $- MulExpr AddExpr1
//AddExpr1 -> $eps
struct AddExpr1 {
	std::string op;//使用的运算符
	std::string value;//使用的数值/中间变量
	int startpos = 0x7fffffff;
	int Quatcnt = 0;//四元式计数


	MulExpr* p_MulExpr=nullptr;
	AddExpr1* p_AddExpr1=nullptr;
};

//非终符AddExpr对应的结构体
//AddExpr -> MulExpr AddExpr1
struct AddExpr {
	std::string value;//使用的数值/中间变量
	int Quatcnt = 0;//四元式计数
	int startpos = 0x7fffffff;

	MulExpr* p_MulExpr = nullptr;
	AddExpr1* p_AddExpr1 = nullptr;
};
//非终符Relop对应的结构体
//Relop -> $<
//Relop -> $<=
//Relop -> $>
//Relop -> $>=
//Relop -> $==
//Relop -> $!=

struct Relop {
	std::string op;//使用的比较符
};
//非终符RelopExpr对应的结构体
//RelopExpr -> Relop AddExpr
//RelopExpr -> $eps
struct RelopExpr {
	std::string op;
	std::string value;
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	AddExpr* p_AddExpr = nullptr;
};

//非终符Expr对应的结构体
//Expr -> AddExpr RelopExpr
struct Expr {
	std::string value;//表达式值（常量或临时变量）
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
};

//非终符ElseCode对应的结构体
//ElseCode -> $else CodeBlock
//ElseCode -> $eps
struct ElseCode {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	bool has_else = false;
	void* p_ElseCB = nullptr;
};

//非终符IfCode对应的结构体
//IfCode -> $if $( Expr $) CodeBlock ElseCode
struct IfCode {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;

	Expr* p_Expr = nullptr;
	void* p_IfCB = nullptr;
	ElseCode* p_ElseCode = nullptr;

};

//非终符WhileCode对应的结构体
//WhileCode -> $while $( Expr $) CodeBlock
struct WhileCode {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;

	Expr* p_Expr = nullptr;
	void* p_CodeBlock = nullptr;
};

//非终符ReturnCode1对应的结构体
//ReturnCode1 -> Expr
//ReturnCode1 -> $eps
struct ReturnCode1 {
	std::string value;//Expr的value
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
};

//非终符ReturnCode对应的结构体
//ReturnCode -> $return ReturnCode1
struct ReturnCode {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
};

//非终符AssignCode对应的结构体
//AssignCode -> $id ArrayPart $= Expr
struct AssignCode {
	std::string value;
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	ArrayPart* p_ArrayPart = nullptr;
	Expr* p_Expr = nullptr;
};

//非终符Code对应的结构体
//Code -> IfCode
//Code -> WhileCode
//Code -> ReturnCode $;
//Code -> AssignCode $;
struct Code {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	void* p_AnyCode = nullptr;
};

//非终符CodeStr对应的结构体
//CodeStr -> Code CodeStr
//CodeStr -> $eps
struct CodeStr {
	int startpos = 0x7fffffff;//起始地址
	int Quatcnt = 0;//长度
	Code* p_Code = nullptr;
	CodeStr* p_CodeStr = nullptr;
};

//非终符ArrayDecl对应的结构体
//ArrayDecl -> $[ $const $] ArrayDecl
//ArrayDecl -> $eps
struct ArrayDecl {
	int width = 0;//宽度
	ArrayDecl* p_ArrayDecl = nullptr;
};

//非终符InVarDecl对应的结构体
//InVarDecl -> $int $id ArrayDecl
struct InVarDecl {
	std::string type;//变量种类，int或数组
	std::string name;//变量名字
	int VarNum = -1;//指明第几个局部变量
	//ArrayDecl
	ArrayDecl* p_ArrayDecl = nullptr;
};

//非终符InState对应的结构体
//InState -> $eps
//InState -> InVarDecl $; InState
struct InState {
	void* p_InState = nullptr;//指向下一个InState
	
};
//非终符CodeBlock对应的结构体
//CodeBlock -> ${ InState CodeStr $}

struct CodeBlock {
	//InState部分，按声明顺序列出
	void* p_InState = nullptr;
	//CodeStr部分
	void* p_CodeStr = nullptr;
	//长度和首地址
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
};

//非终符Argument对应的结构体
//Argument -> $int $id
struct Argument {
	std::string name;	//形参名字
	std::string type;	//形参类型
	int Argcnt = -1;	//指明第几个参数
};
//非终符ArgsList1对应的结构体
//ArgsList1 -> $, Argument ArgsList1
//ArgsList1 -> $eps

struct ArgsList1 {
	Argument* p_Argument = nullptr;
	ArgsList1* p_ArgList1 = nullptr;
};

//非终符ArgsList对应的结构体
//ArgsList -> Argument ArgsList1
struct ArgsList {
	Argument* p_Argument = nullptr;
	ArgsList1* p_ArgList1 = nullptr;
};
//非终符FPara对应的结构体
//FPara -> ArgsList
//FPara -> $void
//FPara -> $eps
struct FPara {
	int argc;//参数个数
};


//非终符FuncDecl对应的结构体
//FuncDecl -> $( FPara $) CodeBlock

struct FuncDecl {
	//FPara部分
	int argc = 0;
	FPara* p_FPara = nullptr;
	//CodeBlock部分
	CodeBlock* p_CodeBlock = nullptr;
};

//非终符VarDecl对应的结构体
//VarDecl -> ArrayDecl $;
struct VarDecl {
	ArrayDecl* p_ArrayDecl = nullptr;
};

//非终符Decl对应的结构体
//Decl -> VarDecl
//Decl -> FuncDecl
struct Decl {
	void* declare = nullptr;
	std::string declType;
};

//非终符State对应的结构体
//State -> $int $id Decl
//State -> $void $id FuncDecl
struct State {
	std::string name;//名称
	std::string ValueType;//声明类型（int void）
	std::string DeclType;//变量/函数/数组
	int argc = 0;//参数个数（仅用于函数）
	int startpos = 0x7fffffff;//起始地址（仅用于函数）
	int Quatcnt = 0;//长度（仅用于函数）
	
};

//非终符StateStr对应的结构体
//StateStr -> State StateStr
//StateStr -> $eps
struct StateStr {

};
//非终符Program对应的结构体
//Program -> StateStr

struct Program {

};

