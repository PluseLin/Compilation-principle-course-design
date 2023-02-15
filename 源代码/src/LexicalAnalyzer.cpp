#include "LexicalAnalyzer.h"
using namespace std;

const char* KeyWord[] = {
	"int","void","if","else","while","return"
};
const int KeyWord_Number = 6;

const char* RelopChar[] = {
	"<",">","!"
};
const int RelopChar_Number = 3;

const char* Symbol[] = {
	"+",	"-",	"*",	"/",	"=",
	"==",	">",	">=",	"<",	"<=",
	"!=",	";",	",",	"(",	")",
	"{",	"}",	"[",	"]",
};
const int Symbol_Number = 19;

const char* EndSymbol = "#";
const char* Identity_Name="$id";
const char* Constant_Name="$const";
extern const char* eps = "$eps";
extern const char* endc = "$#";

/* �ǳ�Ա���� */

int ArrayFind(const char** array, int size, string goal) {
	for (int i = 0; i < size; i++) {
		if (goal == array[i]) {
			return i;
		}
	}
	return - 1;
}

void LexicalAnalyzer::reload(const char* fp) {
	lx_fp = fp;
}

/* Word��Ա���� */

string Word::str() {
	return string(/*"line"+to_string(line)+" :*/"< " + id + ", " + value + " >");
}

bool Word::operator< (const Word& word)const {
	return this->id < word.id || (this->id == word.id && this->value < word.value)
		|| (this->value == word.value && this->line < word.line);
}

Word& Word::operator=(const Word& word) {
	this->id = word.id;
	this->value = word.value;
	this->line = word.line;
	return *this;
}

/* LexicalAnalyzer��Ա���� */

LexicalAnalyzer::LexicalAnalyzer(const char* fp) {	
	reload(fp);
}

LexicalAnalyzer::~LexicalAnalyzer(){
    lx_fp.clear();
    lx_Wordist.clear();
    lx_errorinfo.clear();
    lx_error=false;
}


string LexicalAnalyzer::readfile() {
	//��ȡ�ļ�
	ifstream fin(lx_fp);
	if (!fin.is_open()) {
		cerr << "Open source file: " << lx_fp << " fail!" << endl;
		exit(-1);
	}
	string text,temp;
	while (!fin.eof()) {
		getline(fin, temp);
		text += temp+"\n";
	}
	text += EndSymbol;
	//�ر��ļ�
	fin.close();
	return text;
}

void LexicalAnalyzer::execute() {
	//��ȡ�ļ�
	string text = readfile();
	//ɨ�裬�õ��ʷ��������
	scan(text);
}

void LexicalAnalyzer::addWord(std::string id, std::string value,int line) {
	lx_Wordist.push_back(Word(id, value,line));
}

void LexicalAnalyzer::scan(string& text) {
	clear();

	//ѭ��ɨ��һ��
	/* DFA ��̬0
	0 NF������ 1����ĸ 3���»��� 4��/ 5��# 15������13
	1 NF������ 1������ 2
	2 F �����Ϊ���֣��ص� 0��i-=2
	3 NF��������ĸ�»��� 3������ 4
	4 F �����ж��Ƿ�Ϊ�ؼ��֣������ж�Ϊid���ص�0,i-=2
	5 NF���»��� 5����ĸ 3������ 6
	6 F ���ʷ�����
	7 NF���»��� 8��* 9������ 6
	8 NF���س� 9������ 8
	9 F ���������ص�0��i-=2
	10NF��* 11������ 10
	11NF��* 11��/ 12������10
	12 F���������ص�0��i-=2
	13NF���س��ո�tab��14������13
	14 F���ж��Ƿ�Ϊ���÷��ţ����ǣ������Ϊ���ţ��ص�0��i-=2���������
	*/
	int state = 0;
	int line = 1;
	int len = text.length();
	string value;
	for (int i = 0; i < len; i++) {
		//��ȡ�ַ�
		char ch = text[i];
		//״̬ת��
		switch (state) {
			//0 NF������ 1����ĸ 3���»��� 4�� / 5���ո�س�tab 0������13
			case 0:
				value = ch;
				if (isdigit(ch))								state = 1;
				else if (isalpha(ch))							state = 3;
				else if (ch == '_')								state = 5;
				else if (ch == '/')								state = 7;
				else if (isspace(ch) || ch == '\n') {
					state = 0;
				}
				else											state = 13;
				break;
			//	1 NF������ 1������ 2
			case 1:
				if (isdigit(ch))			state = 1, value += ch;
				else						state = 2;
				break;
			//	2 F �����Ϊ���֣��ص� 0��i-=2
			case 2:
				addWord(Constant_Name, value, line);
				value = "", state = 0;
				i-=2;
				break;
			//	3 NF��������ĸ�»��� 3������ 4
			case 3:
				if (isalnum(ch) || ch == '_') state = 3, value += ch;
				else state = 4;
				break;
			//	4 F �����ж��Ƿ�Ϊ�ؼ��֣������ж�Ϊid���ص�0, i-=2
			case 4:
				if (int pos = ArrayFind(KeyWord, KeyWord_Number, value) != -1) {
					addWord("$" + value, value, line);
				}
				else
					addWord(Identity_Name, value, line);
				
				value = "", state = 0, i -= 2;
				break;
			//	5 NF���»��� 5����ĸ 3������ 6
			case 5:
				if (ch == '_')			state = 5, value += ch;
				else if (isalpha(ch))	state = 3, value += ch;
				else					state = 6;
				break;
			//	6 F ���ʷ�����
			case 6:
				addError(line,ch);
				return;
			//	7 NF��/ 8�� * 9������ 6
			case 7:
				if (ch == '/')				state = 8, value += ch;
				else if (ch == '*')			state = 10, value += ch;
				else						state = 6;
				break;
			//	8 NF���س� 9������ 8
			case 8:
				if (ch == '\n') state = 9;
				else			state = 8;
				break;
			//	9 F ���������ص�0��i-=2
			case 9:
				state = 0, value = "", i-=2;
				break;
			//	10NF�� * 11������ 10
			case 10:
				if (ch == '*')				state = 11;
				else {
					state = 10;
				}
			//	11NF�� * 11�� / 12������10
			case 11:
				if (ch == '*')				state = 11;
				else if (ch == '/')			state = 12;
				else { 
					state = 10; 
				}
				break;
			//	12 F���������ص�0��i-=2
			case 12:
				state = 0, value = "", i-=1;
				break;
			//	13NF���س��ո�tab 14������=����13������=��14������13
			case 13:
				if (isspace(ch) || isalnum(ch) || ch == '_')
					state = 14;
				else if (ArrayFind(Symbol, Symbol_Number, value) != -1) {
					if (ArrayFind(Symbol, Symbol_Number, value + ch) != -1)
						state = 13, value += ch;
					else state = 14;
				}
				else													
					value += ch,state = 13;
				break;
			//	14 F���ж��Ƿ�Ϊ���÷��ţ����ǣ������Ϊ���ţ��ص�0��i-=2���������
			case 14:
				if (int pos = ArrayFind(Symbol, Symbol_Number, value) != -1) {
					addWord("$" + string(value), value, line);
					value = "", state = 0, i-=2;
				}
				else {
					addError(line, ch);
					return;
				}
				break;
			//�������
			default:
				addError(line, ch);
				return;
		}
	}
	addWord("$#", "#", line); 
}

void LexicalAnalyzer::clear() {
	this->lx_Wordist.clear();
	this->lx_errorinfo = "";
	this->lx_error = false;
}

void LexicalAnalyzer::addError(int line,char ch) {
    string temp = "�ʷ��������󣺳���λ���ַ�\\" +
        to_string(int(ch)) + "(" + ch + ")������";
	this->lx_errorinfo = temp;
	this->lx_error = true;
}

//�ж��Ƿ��д�
bool LexicalAnalyzer::is_error() {
	return lx_error;
}
//����������Ϣ
string LexicalAnalyzer::geterrorinfo() {
	return lx_errorinfo;
}

vector<Word>& LexicalAnalyzer::GetWordList() {
	return lx_Wordist;
}

//������
void LexicalAnalyzer::test() {
	execute();
	if (lx_error) {
		cout << lx_errorinfo << endl;
	}
	for (Word w : lx_Wordist) {
		cout << w.str() << endl;
	}
}
