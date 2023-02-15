#pragma once
#include<initializer_list>
#include"Parser.h"
#include"Attrbute.h"

#define ARRAY "int[]"
#define VAR	"int"
#define FUNC "function"

//�������������ɸ�άλ��
void getdim(std::vector<std::string>& dim, ArrayPart* p_ArrayPart);


//��Ԫʽ�ṹ��
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

//������Խṹ��ı�
struct AttrTable {
	int cnt = 0;
	std::map<int, void*> a_PtrTable;
	std::map<int, std::string> a_IdTable;
    ~AttrTable();
};

//���������
class SemanticAnalyzer{
	friend class AsmGenerator;
private:
	//����ʽ���У��±��ʾ�� Input
	std::vector<std::vector<int> > sa_ProdIndex;
	//�������� Input
	std::vector<Word> sa_WordList;
	//�м���� Output
	std::vector<Quat> sa_QuatList;
	//���Խṹ���
	AttrTable sa_AttrTable;

	//��������
	std::map<int, VarNode> sa_VarTable;
	
	//��������
	std::map<int, FuncNode> sa_FuncTable;


	//��ǰ������Ԫʽ�׵�ַ����ʼΪ1
	//0������main����
	int cur_startpos = 2;
	
	//��ǰ�������ʹ�õ���ʱ�������
	//ʹ��Ϊ%T+���
	int tempcnt = 1;
	
	//��ǰ�ֲ�����ͳ�ƣ�һ�����������鱻�������ԭ��ֵ
	//����ʵ���У��ݲ�֧�ֱ�׼C�����е�����Ƕ��
	int sa_Varcnt = 0;
	int sa_PublicCnt = 0;
	//��ǰ��������ͳ�ƣ�һ�����������б������ԭ��ֵ
	//Ϊ����ʵ�����֣��βμ�����-1��ʼ����Ҳ������ջ֡�еĴ���
	//����ʵ���У��ݲ�֧�ֱ�׼C�����е�ʵ���β�����
	int sa_Argcnt = 0;
	//������Ϣ
    bool sa_error=false;
	std::string sa_errorinfo;
	//Ѱ�ҷ���������VarNode������������map���±�
	int findVarNode(std::string name, std::string field);
	//Ѱ�ҷ���������Funcnode������������map���±�
	int findFuncNode(std::string name);

	typedef void (SemanticAnalyzer::* SubFunc)(std::vector<int>&);

	//��¼�Ƿ񷵻�int
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
	//�ܷ������򣬸��ݲ�ͬ���󲿵��ò�ͬ���ӷ�������
	void Analy();

	//��װ�������Ա�Ĳ���
	void addAttr(void* ptr, int index);

	//��װ����Ԫʽ���뵽��Ԫʽ���еĲ���
	void addQuat(Quat q);
	void insertQuat(Quat q, int pos);
	//��ȡ�µ���ʱ����
	std::string NewTemp();
	//�޸�Quat��ĳһֵ
	void updateQuat(int Quatpos, std::string member,std::string result);

	//��Բ�ͬ�󲿵��ӷ����������ڲ��ٲ��
	//��ʽ void xxx(std::vector<int>& ProdIndex);
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
	//������
	//�������Ƿ�������
	bool checkVarDecl(std::string var);
	//�������Ƿ�δ����
	bool checkVarUse(std::string var);
	//�������ά���Ƿ�һ��
	bool checkArrayDim(std::string name,
					   std::vector<std::string>& use,
					   std::vector<int>& decl);
	//��麯���Ƿ�δ����
	bool checkFuncUse(std::string name);
	//��麯������ֵ�Ƿ�Ϊvoid
	bool checkFuncRet(std::string name);
	//��麯���Ƿ�������
	bool checkFuncDecl(std::string name);
	//��麯�������Ƿ�һ��
	bool checkFuncArgc(std::string name, int argc);
	//��麯������ֵ����������Ƿ�һ��
	bool checkFuncType(std::string name, bool has_ret);
public:
    bool is_error();

    std::string geterrorinfo();

	//���캯��
	SemanticAnalyzer(Parser& ps);
	//���뺯��
	void load(Parser& ps);
	//ִ�к���
	void execute();
	//��պ���
	void clear();
	//����Ϊtxt�ļ�
	void export_to_txt(const char* dst);
	//������
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

