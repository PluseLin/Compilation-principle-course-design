#include "AsmGenerator.h"
using namespace std;

/* �ǳ�Ա���� */

/* MipsAsm��Ա���� */
MipsAsm::MipsAsm(string opcode,
				 initializer_list<std::string> argv) {
	this->opcode = opcode;
	this->argv.assign(argv.begin(), argv.end());
	this->argc = this->argv.size();
}

string MipsAsm::str() {
	//��ʵ�����ɵ�code�У���lw��swָ���������ţ�������⴦��
	if (opcode == "lw" || opcode == "sw") {
		return opcode + " " + argv[0] + "," + argv[1] + "(" + argv[2] + ")";
	}
	//��ǩ
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

/* AsmGenerator��Ա���� */
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
	//����������
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

//����Ĵ��������Ǳ�����Ὣ�䵼�뵽�Ĵ�����
//Ŀǰ���뷨�ǣ�$t0-$t8�����ʱ�������ڴ����,$a0-$a3��Ų���
//$v1�������
string AsmGenerator::allocreg(string value, string field,bool has_i) {
	if (value.length() == 0) return "";
	//0������$0����
	if (value == "0") return "$0";
	if (value == "EAX") return "$v0";
	//���������iָ���Ż��������ֲ��÷��䣬������$t9����
	if (isdigit(int(value[0]))) {
		if (has_i) return "";
		addAsmCode(MipsAsm("li", { "$t9",value}));
		return "$t9";
	}
	//��Ϊ��ʱ�������ڴ�����������Ƿ��ѷ��䣬�������ٷ���
	else {
		if (is_temp(value)) {
			for (int i = 0; i < g_RegUsage.size(); i++) {
				if (g_RegUsage[i] == value) {
					return "$t" + to_string(i);
				}
			}
		}
		//û�ҵ������
		g_RegUsage[g_UsageFront] = value;
		string reg = "$t" + to_string(g_UsageFront);
		g_UsageFront = (g_UsageFront + 1) % g_RegUsage.size();
		if (isdigit(int(value[0]))) {
			addAsmCode(MipsAsm("li", { reg,value }));
		}
		//��������ʱ������Ҫ����Ĵ���
		else if (!is_temp(value)) {
			string base, offset;
			GetAddr(value, field, base, offset);
			addAsmCode(MipsAsm("lw", { reg,offset,base }));
		}
		return reg;
	}
	return "";
}

//�жϱ����Ƿ�Ϊ��ʱ����
bool AsmGenerator::is_temp(std::string var) {
	//Լ����ʱ������ͷΪ"%"
	return var.length() > 0 && var[0] == '%';
}

//ͳ�ƾֲ���������
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
	//����ɨ��ÿһ����Ԫʽ��������Ӧ�Ļ����룬����¼�»���������
	//��ʼ�� $fp����0x1001fffc��$sp����ֵͬ,$gp����0x10000000
	//�����main�����������main����������ֱ��exit
	addAsmCode(MipsAsm("li", { "$sp","0x1001fffc" }));
	addAsmCode(MipsAsm("addi", { "$fp","$sp","0" }));
	addAsmCode(MipsAsm("li", { "$gp","0x10000000" }));
	string field;
	for (int i = 0; i < g_QuatList.size(); i++) {
		auto quat = g_QuatList[i];
		g_AddrTable.push_back(int(g_CodeList.size()));
		//�������Ԫʽ��Ӧĳ��������׵�ַ����Ҫά��ջ֡������һϵ�д���
		if (is_funcfirst(i,field)) {
			//���淵�ص�ַ
			addAsmCode(MipsAsm("sw", { "$ra","0","$fp" }));
			addAsmCode(MipsAsm("addi", { "$fp","$fp","-4" }));
			//������һ֡��sp
			addAsmCode(MipsAsm("sw", { "$sp","0","$fp" }));
			addAsmCode(MipsAsm("move", { "$sp","$fp" }));
			addAsmCode(MipsAsm("addi", { "$fp","$fp","-4" }));
			//�޸�fp
			string offset = to_string(LocalVarCnt(field) * 4);
			addAsmCode(MipsAsm("subi", { "$fp","$fp",offset }));
			//��ռĴ�����
			g_RegUsage.assign(g_RegNum, "");
		}
		//�������Ԫʽ�ǼӼ���Ƚ�
		//�ȽϷ����������setϵ��ָ�������Ϊ���ʱ�����üĴ���ֵΪ1������Ϊ0
		//��������������ڼӼ�
		if (quat.op == "+" || quat.op == "-" ||quat.op == ">" || quat.op == ">=" ||
			quat.op == "<" ||quat.op == "<=" || quat.op == "==" || quat.op == "!=") {
			string opcode = g_Quat2Asm[quat.op];
			bool has_i = quat.op == "+" || quat.op == "-";
			string var1 = allocreg(quat.argv1, field, has_i);
			string var2 = allocreg(quat.argv2, field, has_i);
			string var3 = allocreg(quat.result, field, has_i);
			//��ʾ�г���
			if (var1 == "" || var2 == "") {
				//���ǳ�������һ��������$v1�Ĵ���
				if (var1 == var2) {
					addAsmCode(MipsAsm("li", { "$v1",quat.argv1 }));
					addAsmCode(MipsAsm(opcode+"i", { var3,"$v1",quat.argv2 }));
				}
				//��һ������������Ҫ�������
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
			//Mips32ָ��mult�У���32λ����32λ������hi��lo�Ĵ�����
			//Mips32ָ��div�У��̴����lo�Ĵ��������������hi�Ĵ���
			//ʹ��mfloȡ��lo�Ĵ�����ֵ
			addAsmCode(MipsAsm("mflo", { var3 }));
		}
		//�������Ԫʽ�Ǹ�ֵ
		else if (quat.op == "mov") {
			//Ŀǰ�Ĵ���������Ĵ�������д���ڴ棬�Ա��ں����Ĵ���
			string src = allocreg(quat.argv1, field, 0),
				dst = allocreg(quat.result, field, 0);
			//movαָ����ڻ�������Ż�
			addAsmCode(MipsAsm("move", { dst,src }));
			//int offset = getvarpos(quat.result, field);
			string base, offset;
			GetAddr(quat.result, field, base, offset);
			if (base != "" && offset != "")
				addAsmCode(MipsAsm("sw", { dst,offset,base }));
		}
		//�������Ԫʽ����ת������������������
		else if (quat.op == "beq" || quat.op == "bne" || quat.op == "j") {
			//������ǩ�����Ǽ�
			//��ǩ��ʽΪ+4���ֿ�+�������תλ��
			int target = 0;
			if (quat.result[0] == '+')
				target = i + stoi(quat.result.substr(1));
			else
				target = i - stoi(quat.result.substr(1));
			string label = NewLabel(target);
			//�Ǽ�
			g_LabelTable[target] = label;
			
			//����beq��bne������Ĵ���
			if (quat.op == "beq" || quat.op == "bne") {
				//������iָ���Ż�
				string var1 = allocreg(quat.argv1, field, 0);
				string var2 = allocreg(quat.argv2, field, 0);
				addAsmCode(MipsAsm(quat.op, { var1,var2,label }));
			}
			//����j��ֱ����ת
			else {
				addAsmCode(MipsAsm(quat.op, { label }));
			}
		}
		//�������Ԫʽ�ǲ�����ջ
		else if (quat.op == "push") {
			//������ջ��������sw�ٶ�ָ��
			//����Ĵ���
			string var = allocreg(quat.argv1, field, 0);
			addAsmCode(MipsAsm("sw", { var,"0","$fp" }));
			addAsmCode(MipsAsm("addi", { "$fp","$fp","-4" }));
		}
		//�������Ԫʽ�Ǵ�ȡ
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
				//���������$sp��λ��
				//$sp-%T14+offset(offset<0)
				addAsmCode(MipsAsm("sub", { "$v1",base,var2 }));
				//addAsmCode(MipsAsm("li", { "$t9",offset }));
				//addAsmCode(MipsAsm("sub", { var2,"$t9",var2}));
			}
			else {
				//���������$gp��λ��
				//$gp+%T14+offset(offset>0)
				addAsmCode(MipsAsm("add", { "$v1",base,var2 }));
			}
			//��ȡ
			addAsmCode(MipsAsm(opcode, { var1,offset,"$v1"}));
		}
		//�������Ԫʽ�Ǻ�������
		else if (quat.op == "jal") {
			if (quat.result == "") continue;
			addAsmCode(MipsAsm("jal", { quat.result }));
			//�ҵ���Ӧ���̵���ڣ���Ԫʽ�׵�ַ��
			FuncNode node = FindFuncNode(quat.result);
			std::string offset = to_string(4 + node.argc * 4);
			//�������غ�ά��ջ֡����
			addAsmCode(MipsAsm("addi", { "$fp","$fp",offset }));
			//�Ǽ��ڱ�ǩ����
			g_LabelTable[node.startpos] = quat.result;
		}
		//�������Ԫʽ�Ǻ�������
		else if (quat.op == "jret") {
			//ȡ�����ص�ַ
			addAsmCode(MipsAsm("lw", { "$ra","4","$sp" }));
			//ά��ջ֡�Ĳ���
			addAsmCode(MipsAsm("move", { "$fp","$sp" }));
			addAsmCode(MipsAsm("lw", { "$sp","0","$sp" }));
			//����
			addAsmCode(MipsAsm("jr", {"$ra"}));
		}
		//����
		else if (quat.op == "halt") {
			//mips32�ܹ��У�ʹ���������������ֹ����
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
	const char* fp = "D:\\��ѧ����\\������\\�����ϵͳ�ṹ\\test.asm";
	export_to_asm(fp);
	cout << "�ļ�������: " << fp << " ��" << endl;
	if (1) {
		cout << "�Ĵ���ʹ��:" << endl;
		for (int i = 0; i < g_RegUsage.size(); i++) {
			cout << "$t" << i << ":\t" << g_RegUsage[i] << endl;
		}
		cout << endl;
	}
	if (1) {
		cout << "��ǩ��" << endl;
		for (auto pr : g_LabelTable) {
			cout << pr.first << " : " << pr.second << endl;
		}
		cout << endl;
	}
	if (1) {
		cout << "��ַת����" << endl;
		for (int i = 0; i < g_AddrTable.size();i++) {
			cout << "��Ԫʽ���:" << i << " ��Ӧ���������" << g_AddrTable[i] << endl;
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
		cerr << "д���������:" << fp << " ʱ���ļ���ʧ��!" << endl;
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
