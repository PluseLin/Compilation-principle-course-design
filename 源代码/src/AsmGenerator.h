#pragma once
#include"SemanticAnalyzer.h"
/*
��ʵ������mips32ָ��еĻ���ָ�
��mips32�ܹ��У���32��ͨ�üĴ�����
����Լ��$v0�Ĵ�����ŷ���ֵ��$ra�Ĵ�����ŷ��ص�ַ.
��ʵ���У��ڹ��̵���ʱǰ�ĸ���������ڼĴ���$a0-$a3��.
ͬʱ���������ڴ��е�λ�á��������������ڴ�ջ֡��
ջ֡ʹ��$fp��Ϊջ֡��ַ��$sp��Ϊջ����
���̵��úͷ���ʱ��ͨ���޸���ֵ��ά��ջ֡��
Լ��ʹ��$t0-$t9�����ʱ������
*/
/*
ջ֡�ṹ��
����3
����2
����1
���ص�ַ
��һ֡$spֵ��ַ	<-$sp
��ʱ����1
��ʱ����2
				<-$fp
���͵�ַ��
*/
struct MipsAsm {
	std::string opcode;
	int argc;
	std::vector<std::string> argv;

	std::string str();
	MipsAsm(std::string opcode,
			std::initializer_list<std::string> argv);
};


class AsmGenerator{
private:
	//��������
	std::map<int, VarNode> g_VarTable;

	//��������
	std::map<int, FuncNode> g_FuncTable;

	//��Ԫʽ����
	std::vector<Quat> g_QuatList;
		
	//��ǩ��
	std::map<int, std::string> g_LabelTable;

	//���ɵĻ�����
	std::vector<MipsAsm> g_CodeList;

	//�м���뵽Ŀ������ַ�任
	//g_AddrTable[i]���ص����м����i��Ӧ�ĵ�һ���������±�
	std::vector<int> g_AddrTable;

	//��ǩ����
	int LabelCnt = 0;


	//�м��������ת��Ϊ������
	std::map<std::string, std::string> g_Quat2Asm{
		{"+","add"},{"-","sub"},{"*","mult"},{"/","div"},
		{"<","slt"},{"<=","sle"},{">","sgt"},{">=","sge"},
		{"==","seq"},{"!=","sne"},{"load","lw"},{"store","sw"}
	};


	//���ҿ���ʹ��FIFO���мĴ�������
	std::vector<std::string> g_RegUsage;
	//�ɷ���Ĵ�����
	const int g_RegNum = 9;
	int g_UsageFront = 0;//��ͷ����ʾ�´η�����˭
private:
	//�жϴ���Ԫʽ�Ƿ�Ϊĳ�������ײ�
	bool is_funcfirst(int i,std::string& field);
	//�жϱ����Ƿ�Ϊ��ʱ����
	bool is_temp(std::string var);

private:
	//ͳ�ƾֲ���������
	int LocalVarCnt(std::string field);
	//�ں������а����Һ�������
	FuncNode FindFuncNode(std::string name);
	
	//��ȡ����λ��
	int getvarpos(std::string var, std::string field);
	
	//����Ĵ��������Ǳ�����Ὣ�䵼�뵽�Ĵ�����
	//Ŀǰ���뷨�ǣ�$t0-$t9�����ʱ�������ڴ����
	//$v1�������
	std::string allocreg(std::string value,std::string field,
						 bool has_i=true);
	//��ȡ������λ��
	void GetAddr(std::string var, std::string field,
				 std::string& base, std::string& offset);
	//ɨ����Ԫʽ
	void scan();
	//�����±�ǩ
	std::string NewLabel(int target);
	//��ӻ�����
	void addAsmCode(MipsAsm code);
public:
	AsmGenerator(SemanticAnalyzer& sa);//���캯��
    //~AsmGenerator();
	void generate();//���ɻ�����
	void export_to_asm(const char* fp);//����Ϊasm�ļ�
	void test();//������
};

