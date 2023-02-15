#include "AsmGenerator.h"
using namespace std;

/* 非成员函数 */

/* MipsAsm成员函数 */
MipsAsm::MipsAsm(string opcode,
				 initializer_list<std::string> argv) {
	this->opcode = opcode;
	this->argv.assign(argv.begin(), argv.end());
	this->argc = this->argv.size();
}

string MipsAsm::str() {
	//本实验生成的code中，仅lw，sw指令会包含括号，因此特殊处理
	if (opcode == "lw" || opcode == "sw") {
		return opcode + " " + argv[0] + "," + argv[1] + "(" + argv[2] + ")";
	}
	//标签
	if (opcode.back() == ':')
		return opcode;
	string code;
	code += opcode + " ";
	for (int i = 0; i < argc; i++) {
		code += argv[i];
		if (i != argc - 1) code += ",";
	}
	return code;
}

/* AsmGenerator成员函数 */
string AsmGenerator::NewLabel(int target) {
	auto ptr = g_LabelTable.find(target);
	if (ptr != g_LabelTable.end())
		return ptr->second;
	return "label" + to_string(++LabelCnt);
}

void AsmGenerator::addAsmCode(MipsAsm code) {
	g_CodeList.push_back(code);
}

bool AsmGenerator::is_funcfirst(int i,string& field){
	for (auto pr : g_FuncTable) {
		if (i == pr.second.startpos) {
			field = pr.second.name;
			return true;
		}
	}
	return false;
}

void AsmGenerator::GetAddr(string var, string field,
						   string& base, string& offset) {
	//遍历变量表
	for (auto pr : g_VarTable) {
		if (pr.second.name == var) {
			if (pr.second.field == field) {
				base = "$sp";
				int of = pr.second.Varcnt;
				of = (of < 0) ? -4 * (of - 1) : -4 * of;
				offset = to_string(of);
				return;
			}
            else if (pr.second.field == "$public") {
				base = "$gp";
				offset = to_string((pr.second.Varcnt - 1) * 4);
				return;
			}
		}
	}
}

int AsmGenerator::getvarpos(std::string var, std::string field) {
	int offset = 1;
	for (auto pr : g_VarTable) {
		if (pr.second.field == field) {
			if (pr.second.name == var) break;
			else if (pr.second.width.size() == 0) offset++;
			else {
				int t = 1;
				for (auto n : pr.second.width) t *= n;
				offset += t;
			}
		}
	}
	return offset * 4;
}

FuncNode AsmGenerator::FindFuncNode(std::string name) {
	for (auto pr : g_FuncTable) {
		if (pr.second.name == name)
			return pr.second;
	}
	return FuncNode();
}

//申请寄存器，若是变量则会将其导入到寄存器中
//目前的想法是，$t0-$t8存放临时变量或内存变量,$a0-$a3存放参数
//$v1存放数字
string AsmGenerator::allocreg(string value, string field,bool has_i) {
	if (value.length() == 0) return "";
	//0可以用$0代替
	if (value == "0") return "$0";
	if (value == "EAX") return "$v0";
	//如果可以用i指令优化，则数字不用分配，否则用$t9分配
	if (isdigit(int(value[0]))) {
		if (has_i) return "";
		addAsmCode(MipsAsm("li", { "$t9",value}));
		return "$t9";
	}
	//若为临时变量或内存变量，先找是否已分配，若无则再分配
	else {
		if (is_temp(value)) {
			for (int i = 0; i < g_RegUsage.size(); i++) {
				if (g_RegUsage[i] == value) {
					return "$t" + to_string(i);
				}
			}
		}
		//没找到则分配
		g_RegUsage[g_UsageFront] = value;
		string reg = "$t" + to_string(g_UsageFront);
		g_UsageFront = (g_UsageFront + 1) % g_RegUsage.size();
		if (isdigit(int(value[0]))) {
			addAsmCode(MipsAsm("li", { reg,value }));
		}
		//若不是临时变量，要调入寄存器
		else if (!is_temp(value)) {
			string base, offset;
			GetAddr(value, field, base, offset);
			addAsmCode(MipsAsm("lw", { reg,offset,base }));
		}
		return reg;
	}
	return "";
}

//判断变量是否为临时变量
bool AsmGenerator::is_temp(std::string var) {
	//约定临时变量开头为"%"
	return var.length() > 0 && var[0] == '%';
}

//统计局部变量个数
int AsmGenerator::LocalVarCnt(string field) {
	int cnt = 0;
	for (auto pr : g_VarTable) {
		if (pr.second.field == field && pr.second.Varcnt > 0) {
			if (pr.second.width.size() == 0) cnt++;
			else {
				int t = 1;
				for (auto w : pr.second.width) t *= w;
				cnt += t;
			}
		}
	}
	return cnt;
}

void AsmGenerator::scan() {
	//依次扫描每一个四元式，生成相应的汇编代码，并记录下汇编代码数量
	//初始化 $fp存入0x1001fffc，$sp存入同值,$gp存入0x10000000
	//如果有main函数，则调用main函数，否则直接exit
	addAsmCode(MipsAsm("li", { "$sp","0x1001fffc" }));
	addAsmCode(MipsAsm("addi", { "$fp","$sp","0" }));
	addAsmCode(MipsAsm("li", { "$gp","0x10000000" }));
	string field;
	for (int i = 0; i < g_QuatList.size(); i++) {
		auto quat = g_QuatList[i];
		g_AddrTable.push_back(int(g_CodeList.size()));
		//如果此四元式对应某函数块的首地址，需要维护栈帧，生成一系列代码
		if (is_funcfirst(i,field)) {
			//保存返回地址
			addAsmCode(MipsAsm("sw", { "$ra","0","$fp" }));
			addAsmCode(MipsAsm("addi", { "$fp","$fp","-4" }));
			//保存上一帧的sp
			addAsmCode(MipsAsm("sw", { "$sp","0","$fp" }));
			addAsmCode(MipsAsm("move", { "$sp","$fp" }));
			addAsmCode(MipsAsm("addi", { "$fp","$fp","-4" }));
			//修改fp
			string offset = to_string(LocalVarCnt(field) * 4);
			addAsmCode(MipsAsm("subi", { "$fp","$fp",offset }));
			//清空寄存器表
			g_RegUsage.assign(g_RegNum, "");
		}
		//如果此四元式是加减或比较
		//比较符的运算采用set系列指令，即条件为真的时候设置寄存器值为1，否则为0
		//因此其运算类似于加减
		if (quat.op == "+" || quat.op == "-" ||quat.op == ">" || quat.op == ">=" ||
			quat.op == "<" ||quat.op == "<=" || quat.op == "==" || quat.op == "!=") {
			string opcode = g_Quat2Asm[quat.op];
			bool has_i = quat.op == "+" || quat.op == "-";
			string var1 = allocreg(quat.argv1, field, has_i);
			string var2 = allocreg(quat.argv2, field, has_i);
			string var3 = allocreg(quat.result, field, has_i);
			//表示有常量
			if (var1 == "" || var2 == "") {
				//都是常量，则一个导入至$v1寄存器
				if (var1 == var2) {
					addAsmCode(MipsAsm("li", { "$v1",quat.argv1 }));
					addAsmCode(MipsAsm(opcode+"i", { var3,"$v1",quat.argv2 }));
				}
				//仅一个变量，不需要导入变量
				else if (var1 == "") {
					//addAsmCode(MipsAsm("li", { "$v1",quat.argv1 }));
					addAsmCode(MipsAsm(opcode+"i", { var3,var2,quat.argv1 }));
				}
				else {
					//addAsmCode(MipsAsm("li", { "$v1",quat.argv2 }));
					addAsmCode(MipsAsm(opcode + "i", { var3,var1,quat.argv2}));
				}
			}
			else {
				addAsmCode(MipsAsm(opcode, { var3,var1,var2 }));
			}
		}
		else if (quat.op == "*" || quat.op == "/") {
			string opcode = g_Quat2Asm[quat.op];
			string var1 = allocreg(quat.argv1, field, 0);
			string var2 = allocreg(quat.argv2, field, 0);
			string var3 = allocreg(quat.result, field, 0);
			addAsmCode(MipsAsm(opcode, { var1,var2 }));
			//Mips32指令mult中，高32位，低32位存在于hi，lo寄存器中
			//Mips32指令div中，商存放于lo寄存器，余数存放于hi寄存器
			//使用mflo取出lo寄存器的值
			addAsmCode(MipsAsm("mflo", { var3 }));
		}
		//如果此四元式是赋值
		else if (quat.op == "mov") {
			//目前的处理是申请寄存器，并写回内存，以便于后续的处理
			string src = allocreg(quat.argv1, field, 0),
				dst = allocreg(quat.result, field, 0);
			//mov伪指令，可在汇编器中优化
			addAsmCode(MipsAsm("move", { dst,src }));
			//int offset = getvarpos(quat.result, field);
			string base, offset;
			GetAddr(quat.result, field, base, offset);
			if (base != "" && offset != "")
				addAsmCode(MipsAsm("sw", { dst,offset,base }));
		}
		//如果此四元式是跳转（有条件或无条件）
		else if (quat.op == "beq" || quat.op == "bne" || quat.op == "j") {
			//创建标签，并登记
			//标签格式为+4，分开+后记下跳转位置
			int target = 0;
			if (quat.result[0] == '+')
				target = i + stoi(quat.result.substr(1));
			else
				target = i - stoi(quat.result.substr(1));
			string label = NewLabel(target);
			//登记
			g_LabelTable[target] = label;
			
			//若是beq和bne，申请寄存器
			if (quat.op == "beq" || quat.op == "bne") {
				//不适用i指令优化
				string var1 = allocreg(quat.argv1, field, 0);
				string var2 = allocreg(quat.argv2, field, 0);
				addAsmCode(MipsAsm(quat.op, { var1,var2,label }));
			}
			//若是j，直接跳转
			else {
				addAsmCode(MipsAsm(quat.op, { label }));
			}
		}
		//如果此四元式是参数入栈
		else if (quat.op == "push") {
			//参数入栈操作，先sw再动指针
			//申请寄存器
			string var = allocreg(quat.argv1, field, 0);
			addAsmCode(MipsAsm("sw", { var,"0","$fp" }));
			addAsmCode(MipsAsm("addi", { "$fp","$fp","-4" }));
		}
		//如果此四元式是存取
		//< store , a , %T14 , %T13 > a[%T14] <- %T13
		// sw %T13,%T14(a)
		//< load , a , %T15 , %T16 >  a[%T15] -> %T16
		// lw %T16,%T15(a)
		else if (quat.op == "load" || quat.op == "store") {
			string opcode = g_Quat2Asm[quat.op];
			string var1 = allocreg(quat.result, field, 0);	//from/to
			string var2 = allocreg(quat.argv2, field, 0);	//offset of base
			string base, offset;
			GetAddr(quat.argv1, field, base, offset);
			if (base == "$sp") {
				//计算相对于$sp的位移
				//$sp-%T14+offset(offset<0)
				addAsmCode(MipsAsm("sub", { "$v1",base,var2 }));
				//addAsmCode(MipsAsm("li", { "$t9",offset }));
				//addAsmCode(MipsAsm("sub", { var2,"$t9",var2}));
			}
			else {
				//计算相对于$gp的位移
				//$gp+%T14+offset(offset>0)
				addAsmCode(MipsAsm("add", { "$v1",base,var2 }));
			}
			//存取
			addAsmCode(MipsAsm(opcode, { var1,offset,"$v1"}));
		}
		//如果此四元式是函数调用
		else if (quat.op == "jal") {
			if (quat.result == "") continue;
			addAsmCode(MipsAsm("jal", { quat.result }));
			//找到对应过程的入口（四元式首地址）
			FuncNode node = FindFuncNode(quat.result);
			std::string offset = to_string(4 + node.argc * 4);
			//函数返回后维护栈帧操作
			addAsmCode(MipsAsm("addi", { "$fp","$fp",offset }));
			//登记在标签表中
			g_LabelTable[node.startpos] = quat.result;
		}
		//如果此四元式是函数返回
		else if (quat.op == "jret") {
			//取出返回地址
			addAsmCode(MipsAsm("lw", { "$ra","4","$sp" }));
			//维护栈帧的操作
			addAsmCode(MipsAsm("move", { "$fp","$sp" }));
			addAsmCode(MipsAsm("lw", { "$sp","0","$sp" }));
			//返回
			addAsmCode(MipsAsm("jr", {"$ra"}));
		}
		//其他
		else if (quat.op == "halt") {
			//mips32架构中，使用以下两条语句终止程序
			addAsmCode(MipsAsm("li", { "$v0","10" }));
			addAsmCode(MipsAsm("syscall", {}));
		}
	}
	/*
	for (auto pr : g_LabelTable) {
		int pos = g_AddrTable[pr.first] + 1;
		g_CodeList.insert(g_CodeList.begin()+pos, MipsAsm({ pr.second + ":",{} }));
	}
	*/
}

void AsmGenerator::test() {
	generate();
	const char* fp = "D:\\大学资料\\大三上\\计算机系统结构\\test.asm";
	export_to_asm(fp);
	cout << "文件导出至: " << fp << " 中" << endl;
	if (1) {
		cout << "寄存器使用:" << endl;
		for (int i = 0; i < g_RegUsage.size(); i++) {
			cout << "$t" << i << ":\t" << g_RegUsage[i] << endl;
		}
		cout << endl;
	}
	if (1) {
		cout << "标签表" << endl;
		for (auto pr : g_LabelTable) {
			cout << pr.first << " : " << pr.second << endl;
		}
		cout << endl;
	}
	if (1) {
		cout << "地址转换表" << endl;
		for (int i = 0; i < g_AddrTable.size();i++) {
			cout << "四元式序号:" << i << " 对应汇编代码序号" << g_AddrTable[i] << endl;
		}
		cout << endl;
	}
}

void AsmGenerator::generate() {
	scan();
}

void AsmGenerator::export_to_asm(const char* fp) {
	ofstream fout(fp);
	if (!fout.is_open()) {
		cerr << "写入汇编代码至:" << fp << " 时，文件打开失败!" << endl;
		exit(-1);
	}
	vector<string>line;
	for (auto code : g_CodeList) {
		line.push_back(code.str());
	}
	for (auto pr : g_LabelTable) {
		int pos = g_AddrTable[pr.first];
		line[pos] = pr.second + ":\n" + line[pos];
	}
	for (auto l : line) fout << l << endl;
	fout.close();
}

AsmGenerator::AsmGenerator(SemanticAnalyzer& sa) {
    //g_VarTable.swap(sa.sa_VarTable);
    for(auto pr:sa.sa_VarTable){
        g_VarTable[pr.first]=pr.second;
    }
    //g_FuncTable.swap(sa.sa_FuncTable);
    for(auto pr:sa.sa_FuncTable){
        g_FuncTable[pr.first]=pr.second;
    }
    //g_QuatList.swap(sa.sa_QuatList);
    g_QuatList.assign(sa.sa_QuatList.begin(),sa.sa_QuatList.end());
	g_RegUsage.assign(g_RegNum, "");
}
/*
AsmGenerator::~AsmGenerator(){
    g_VarTable.clear();
    g_FuncTable.clear();
    g_QuatList.clear();
    g_LabelTable.clear();
    g_CodeList.clear();
    g_AddrTable.clear();
    g_Quat2Asm.clear();
    g_RegUsage.clear();
}*/
