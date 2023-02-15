#include<iostream>
#include<string>
#include<vector>

struct VarNode {
	std::string name;//����
	std::string type;//����
	std::string field;//������
	int	Varcnt = -1;//�ڼ����ֲ���������Ϊ����������ΪС��0���������0
	std::vector<int> width;//��ά���ޣ����������Ϊ�գ�
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
	std::string name;//����
	std::string type;//��������
	int argc = 0;//��������
	int startpos = 0x7fffffff;//��Ԫʽ��ʼλ��
	int Quatcnt = 0;//��Ԫʽ����
};

//���շ�RParaList1��Ӧ�Ľṹ��
//RParaList1 -> $, Expr RParaList1
//RParaList1 -> $eps
struct RParaList1 {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	int argc = 0;
	void* p_Expr = nullptr;
	RParaList1* p_RParaList1 = nullptr;
};

//���շ�RParaList��Ӧ�Ľṹ��
//RParaList -> Expr RParaList1
struct RParaList {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	int argc = 0;
	void* p_Expr = nullptr;
	RParaList1* p_RParaList1 = nullptr;
};

//���շ�RPara��Ӧ�Ľṹ��
//RPara -> RParaList
//RPara -> $eps
struct RPara {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	int argc = 0;
	RParaList* p_RParaList = nullptr;
};

//���շ�ArrayPart��Ӧ�Ľṹ��
//ArrayPart -> $eps
//ArrayPart -> $[ Expr $] ArrayPart
struct ArrayPart {
	bool is_array = false;//��Щ����array��Ҳ���漰���˽ṹ��
	std::string value;//��ǰά��λ�Ʊ���

	int startpos = 0;
	int Quatcnt = 0x7fffffff;
	
	ArrayPart* p_ArrayPart = nullptr;
	
};

//���շ�CallList��Ӧ�Ľṹ��
//CallList -> $( RPara $)
struct CallList {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	int argc = 0;
	RPara* p_RPara = nullptr;;
};
//���շ�Factor1��Ӧ�Ľṹ��

//Factor1 -> CallList
//Factor1 -> ArrayPart
struct Factor1 {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	std::string type;//���࣬����/����/��
	void* p_Any = nullptr;//CallList/ArrayPart
};

//���շ�Factor��Ӧ�Ľṹ��
//Factor -> $const
//Factor -> $( Expr $)
//Factor -> $id Factor1
struct Factor {
	std::string value;//��Factorռ�õ�ֵ�����
	std::string type;//���ͣ��������û���ֵ��
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	Factor1* p_Factor1 = nullptr;
	//void* p_Expr
};

//���շ�MulExpr1��Ӧ�Ľṹ��
//MulExpr1 -> $* Factor MulExpr1
//MulExpr1 -> $/ Factor MulExpr1
//MulExpr1 -> $eps
struct MulExpr1 {
	std::string op;//ʹ�õ������
	std::string value;//ʹ�õ���ֵ/�м����
	int startpos = 0x7fffffff;
	int Quatcnt = 0;//��Ԫʽ����

	Factor* p_Factor = nullptr;//��Ӧ��Factor
	MulExpr1* p_MulExpr1 = nullptr;//��һ��MulExpr1
};

//���շ�MulExpr��Ӧ�Ľṹ��
//MulExpr -> Factor MulExpr1
struct MulExpr {
	std::string value;//ʹ�õ���ֵ/����
	int Quatcnt = 0;//��Ԫʽ����
	int startpos = 0x7fffffff;//��Ԫʽ��ʼλ��

	Factor* p_Factor = nullptr;
	MulExpr1* p_MulExpr1 = nullptr;
};

//���շ�AddExpr1��Ӧ�Ľṹ��
//AddExpr1 -> $+ MulExpr AddExpr1
//AddExpr1 -> $- MulExpr AddExpr1
//AddExpr1 -> $eps
struct AddExpr1 {
	std::string op;//ʹ�õ������
	std::string value;//ʹ�õ���ֵ/�м����
	int startpos = 0x7fffffff;
	int Quatcnt = 0;//��Ԫʽ����


	MulExpr* p_MulExpr=nullptr;
	AddExpr1* p_AddExpr1=nullptr;
};

//���շ�AddExpr��Ӧ�Ľṹ��
//AddExpr -> MulExpr AddExpr1
struct AddExpr {
	std::string value;//ʹ�õ���ֵ/�м����
	int Quatcnt = 0;//��Ԫʽ����
	int startpos = 0x7fffffff;

	MulExpr* p_MulExpr = nullptr;
	AddExpr1* p_AddExpr1 = nullptr;
};
//���շ�Relop��Ӧ�Ľṹ��
//Relop -> $<
//Relop -> $<=
//Relop -> $>
//Relop -> $>=
//Relop -> $==
//Relop -> $!=

struct Relop {
	std::string op;//ʹ�õıȽϷ�
};
//���շ�RelopExpr��Ӧ�Ľṹ��
//RelopExpr -> Relop AddExpr
//RelopExpr -> $eps
struct RelopExpr {
	std::string op;
	std::string value;
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	AddExpr* p_AddExpr = nullptr;
};

//���շ�Expr��Ӧ�Ľṹ��
//Expr -> AddExpr RelopExpr
struct Expr {
	std::string value;//���ʽֵ����������ʱ������
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
};

//���շ�ElseCode��Ӧ�Ľṹ��
//ElseCode -> $else CodeBlock
//ElseCode -> $eps
struct ElseCode {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
	bool has_else = false;
	void* p_ElseCB = nullptr;
};

//���շ�IfCode��Ӧ�Ľṹ��
//IfCode -> $if $( Expr $) CodeBlock ElseCode
struct IfCode {
	int startpos = 0x7fffffff;
	int Quatcnt = 0;

	Expr* p_Expr = nullptr;
	void* p_IfCB = nullptr;
	ElseCode* p_ElseCode = nullptr;

};

//���շ�WhileCode��Ӧ�Ľṹ��
//WhileCode -> $while $( Expr $) CodeBlock
struct WhileCode {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;

	Expr* p_Expr = nullptr;
	void* p_CodeBlock = nullptr;
};

//���շ�ReturnCode1��Ӧ�Ľṹ��
//ReturnCode1 -> Expr
//ReturnCode1 -> $eps
struct ReturnCode1 {
	std::string value;//Expr��value
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
};

//���շ�ReturnCode��Ӧ�Ľṹ��
//ReturnCode -> $return ReturnCode1
struct ReturnCode {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
};

//���շ�AssignCode��Ӧ�Ľṹ��
//AssignCode -> $id ArrayPart $= Expr
struct AssignCode {
	std::string value;
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	ArrayPart* p_ArrayPart = nullptr;
	Expr* p_Expr = nullptr;
};

//���շ�Code��Ӧ�Ľṹ��
//Code -> IfCode
//Code -> WhileCode
//Code -> ReturnCode $;
//Code -> AssignCode $;
struct Code {
	int Quatcnt = 0;
	int startpos = 0x7fffffff;
	void* p_AnyCode = nullptr;
};

//���շ�CodeStr��Ӧ�Ľṹ��
//CodeStr -> Code CodeStr
//CodeStr -> $eps
struct CodeStr {
	int startpos = 0x7fffffff;//��ʼ��ַ
	int Quatcnt = 0;//����
	Code* p_Code = nullptr;
	CodeStr* p_CodeStr = nullptr;
};

//���շ�ArrayDecl��Ӧ�Ľṹ��
//ArrayDecl -> $[ $const $] ArrayDecl
//ArrayDecl -> $eps
struct ArrayDecl {
	int width = 0;//���
	ArrayDecl* p_ArrayDecl = nullptr;
};

//���շ�InVarDecl��Ӧ�Ľṹ��
//InVarDecl -> $int $id ArrayDecl
struct InVarDecl {
	std::string type;//�������࣬int������
	std::string name;//��������
	int VarNum = -1;//ָ���ڼ����ֲ�����
	//ArrayDecl
	ArrayDecl* p_ArrayDecl = nullptr;
};

//���շ�InState��Ӧ�Ľṹ��
//InState -> $eps
//InState -> InVarDecl $; InState
struct InState {
	void* p_InState = nullptr;//ָ����һ��InState
	
};
//���շ�CodeBlock��Ӧ�Ľṹ��
//CodeBlock -> ${ InState CodeStr $}

struct CodeBlock {
	//InState���֣�������˳���г�
	void* p_InState = nullptr;
	//CodeStr����
	void* p_CodeStr = nullptr;
	//���Ⱥ��׵�ַ
	int startpos = 0x7fffffff;
	int Quatcnt = 0;
};

//���շ�Argument��Ӧ�Ľṹ��
//Argument -> $int $id
struct Argument {
	std::string name;	//�β�����
	std::string type;	//�β�����
	int Argcnt = -1;	//ָ���ڼ�������
};
//���շ�ArgsList1��Ӧ�Ľṹ��
//ArgsList1 -> $, Argument ArgsList1
//ArgsList1 -> $eps

struct ArgsList1 {
	Argument* p_Argument = nullptr;
	ArgsList1* p_ArgList1 = nullptr;
};

//���շ�ArgsList��Ӧ�Ľṹ��
//ArgsList -> Argument ArgsList1
struct ArgsList {
	Argument* p_Argument = nullptr;
	ArgsList1* p_ArgList1 = nullptr;
};
//���շ�FPara��Ӧ�Ľṹ��
//FPara -> ArgsList
//FPara -> $void
//FPara -> $eps
struct FPara {
	int argc;//��������
};


//���շ�FuncDecl��Ӧ�Ľṹ��
//FuncDecl -> $( FPara $) CodeBlock

struct FuncDecl {
	//FPara����
	int argc = 0;
	FPara* p_FPara = nullptr;
	//CodeBlock����
	CodeBlock* p_CodeBlock = nullptr;
};

//���շ�VarDecl��Ӧ�Ľṹ��
//VarDecl -> ArrayDecl $;
struct VarDecl {
	ArrayDecl* p_ArrayDecl = nullptr;
};

//���շ�Decl��Ӧ�Ľṹ��
//Decl -> VarDecl
//Decl -> FuncDecl
struct Decl {
	void* declare = nullptr;
	std::string declType;
};

//���շ�State��Ӧ�Ľṹ��
//State -> $int $id Decl
//State -> $void $id FuncDecl
struct State {
	std::string name;//����
	std::string ValueType;//�������ͣ�int void��
	std::string DeclType;//����/����/����
	int argc = 0;//���������������ں�����
	int startpos = 0x7fffffff;//��ʼ��ַ�������ں�����
	int Quatcnt = 0;//���ȣ������ں�����
	
};

//���շ�StateStr��Ӧ�Ľṹ��
//StateStr -> State StateStr
//StateStr -> $eps
struct StateStr {

};
//���շ�Program��Ӧ�Ľṹ��
//Program -> StateStr

struct Program {

};

