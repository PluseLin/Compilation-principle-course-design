#pragma once
#include"SemanticAnalyzer.h"
/*
本实验生成mips32指令集中的基础指令。
在mips32架构中，有32个通用寄存器。
其中约定$v0寄存器存放返回值，$ra寄存器存放返回地址.
本实验中，在过程调用时前四个参数存放于寄存器$a0-$a3中.
同时保留其在内存中的位置。其余参数存放于内存栈帧。
栈帧使用$fp作为栈帧基址，$sp作为栈顶。
过程调用和返回时会通过修改其值来维护栈帧。
约定使用$t0-$t9存放临时变量。
*/
/*
栈帧结构：
参数3
参数2
参数1
返回地址
上一帧$sp值地址	<-$sp
临时变量1
临时变量2
				<-$fp
（低地址）
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
	//变量名表
	std::map<int, VarNode> g_VarTable;

	//函数名表
	std::map<int, FuncNode> g_FuncTable;

	//四元式序列
	std::vector<Quat> g_QuatList;
		
	//标签表
	std::map<int, std::string> g_LabelTable;

	//生成的汇编代码
	std::vector<MipsAsm> g_CodeList;

	//中间代码到目标代码地址变换
	//g_AddrTable[i]返回的是中间代码i对应的第一个汇编代码下标
	std::vector<int> g_AddrTable;

	//标签计数
	int LabelCnt = 0;


	//中间代码运算转换为汇编代码
	std::map<std::string, std::string> g_Quat2Asm{
		{"+","add"},{"-","sub"},{"*","mult"},{"/","div"},
		{"<","slt"},{"<=","sle"},{">","sgt"},{">=","sge"},
		{"==","seq"},{"!=","sne"},{"load","lw"},{"store","sw"}
	};


	//暂且考虑使用FIFO进行寄存器分配
	std::vector<std::string> g_RegUsage;
	//可分配寄存器数
	const int g_RegNum = 9;
	int g_UsageFront = 0;//队头，表示下次分配用谁
private:
	//判断此四元式是否为某函数的首部
	bool is_funcfirst(int i,std::string& field);
	//判断变量是否为临时变量
	bool is_temp(std::string var);

private:
	//统计局部变量个数
	int LocalVarCnt(std::string field);
	//在函数表中按名找函数表项
	FuncNode FindFuncNode(std::string name);
	
	//获取变量位移
	int getvarpos(std::string var, std::string field);
	
	//申请寄存器，若是变量则会将其导入到寄存器中
	//目前的想法是，$t0-$t9存放临时变量或内存变量
	//$v1存放数字
	std::string allocreg(std::string value,std::string field,
						 bool has_i=true);
	//获取变量的位移
	void GetAddr(std::string var, std::string field,
				 std::string& base, std::string& offset);
	//扫描四元式
	void scan();
	//生成新标签
	std::string NewLabel(int target);
	//添加汇编代码
	void addAsmCode(MipsAsm code);
public:
	AsmGenerator(SemanticAnalyzer& sa);//构造函数
    //~AsmGenerator();
	void generate();//生成汇编代码
	void export_to_asm(const char* fp);//导出为asm文件
	void test();//测试用
};

