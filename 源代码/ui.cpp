#include "ui.h"
#include "ui_ui.h"

#include<QFile>
#include<QFileDialog>
#include<QMessageBox>
#include<QDateTime>
#include<QtCore/QTextCodec>

#include"lexicalui.h"
#include"parserui.h"
#include"grammarui.h"
#include"predtableui.h"
#include"semanticui.h"
#include"help.h"

UI::UI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UI)
{
    ui->setupUi(this);
    ui_codec=QTextCodec::codecForName("GB2312");

    QFont font;
    QFontMetrics metrics(font);

    this->ui->SourceText->setTabStopWidth(4*metrics.width(' '));
    this->ui->SourceText->setReadOnly(true);
    this->ui->InfoText->setTabStopWidth(4*metrics.width(' '));
    this->ui->InfoText->setReadOnly(true);

    this->setWindowTitle(ui_title);
}

UI::~UI()
{
    if(ui_la!=nullptr) delete ui_la;
    if(ui_pa!=nullptr) delete ui_pa;
    if(ui_sa!=nullptr) delete ui_sa;
    if(ui_ag!=nullptr) delete ui_ag;
    delete ui;
}

QString ui_GetName(QString src){
    //"D:/C++/file.c"
    QString name;
    for(auto p=src.rbegin();p!=src.rend();p++){
        if(*p=='.'){
            p++;
            while(p!=src.rend() && *p!='/' && *p!='\\'){
                name.insert(0,*p);
                p++;
            }
        }
    }
    return name;
}

void MakeDotFile(ParserTree& tree){
    //dot文件内容
    std::string file;
    //bfs
    //std::vector<std::vector<std::vector<Word> > > Words;
    //先把节点画好
    auto Words=tree.GetWordList();
    auto Branchs=tree.GerBranchs();
    std::vector<std::pair<int,int> > Draw_Branch;
    file+="digraph G {\n";
    std::queue<int> Q;
    Q.push(1);//根节点为1
    while(!Q.empty()){
        int top=Q.front();Q.pop();
        file+="\tstruct"+std::to_string(top)+
              " [shape=ellipse,label=\""+Words[top].value+
              "\"];\n";
        for(auto ptr=Branchs[top].rbegin();ptr!=Branchs[top].rend();ptr++){
            Draw_Branch.push_back({top,*ptr});
            Q.push(*ptr);
        }
    }
    for(auto pr:Draw_Branch){
        int father=pr.first,child=pr.second;
        file+="\tstruct"+std::to_string(father)+" -> "+
              "struct"+std::to_string(child)+";\n";
    }
    file+="}\n";

//    for(int i=1;i<Words.size();i++){
//        for(auto ptr=Branchs[i].rbegin();ptr!=Branchs[i].rend();ptr++)
//        file+="struct"+std::to_string(*ptr)+
//                " [shape=ellipse,label=\""+Words[*ptr].value+
//                "\"];\n";
//    }
    //再画边


//    while(!Q.empty()){
//        int top=Q.front();Q.pop();
//        //std::cout<<top<<std::endl;
//        for(auto ptr=Branchs[top].rbegin();ptr!=Branchs[top].rend();ptr++){
//            file+="struct"+std::to_string(top)+" -> "+
//                    "struct"+std::to_string(*ptr)+";\n";
//            Q.push(*ptr);
//        }
//    }
//    file+="}\n";
    //写入dot文件
    std::ofstream fout("tree.dot");
    try {
        fout<<file;
    } catch (std::ofstream&) {
        std::cerr<<"Open Dot file fail!"<<std::endl;
        return;
    }
    //system生成图片
    system("cd Graphviz\\bin&\
            dot -Tpng ..\\..\\tree.dot -o ..\\..\\tree.png");
}

bool UI::ui_checksuccess(){
    if(ui_src.isEmpty()){
        QMessageBox::information(nullptr,
                                 QStringLiteral("提示"),
                                 QStringLiteral("请先打开文件！"));
        return false;
    }
    if(ui_success==false){
        QMessageBox::information(nullptr,
                                 QStringLiteral("提示"),
                                 QStringLiteral("编译失败，不展示结果"));
        return false;
    }
    return true;
}

void UI::ui_compile_success(QString info){
    auto date=QDateTime::currentDateTime();
    QString time=date.toString("yyyy.MM.dd hh:mm:ss");
    QString log=time+" "+info+"\r";
    this->ui->InfoText->insertPlainText(log);
    this->ui_success=true;
}

void UI::ui_compile_error(QString info){
    auto date=QDateTime::currentDateTime();
    QString time=date.toString("yyyy.MM.dd hh:mm:ss");
    QString log=time+" "+info+"\r";
    this->ui->InfoText->insertPlainText(log);
    QMessageBox::information(nullptr,QStringLiteral("编译失败"),info);
    ui_success=false;
}

void UI::ui_Compile(bool GenQuat,bool GenAsm){
    //先判断有无打开文件
    if(ui_src.isEmpty()){
        QMessageBox::information(
                    nullptr,
                    QStringLiteral("提示"),
                    QStringLiteral("请先打开文件！"));
        return;
    }

    if(ui_la!=nullptr) delete ui_la;
    if(ui_pa!=nullptr) delete ui_pa;
    if(ui_sa!=nullptr) delete ui_sa;
    if(ui_ag!=nullptr) delete ui_ag;

    ui_success=false;

    //词法分析
    ui_la=new LexicalAnalyzer(ui_src.toLocal8Bit());
    if(!ui_la) exit(-1);
    ui_la->execute();
    if(ui_la->is_error()){
        //QString info=QString::fromStdString(ui_la->geterrorinfo());
        //ui_compile_error(info);
        auto info=QByteArray(ui_la->geterrorinfo().c_str());
        ui_compile_error(ui_codec->toUnicode(info));
        return;
    }

    //语法分析
    ui_pa=new Parser("Grammar.txt",*ui_la);
    if(!ui_pa) exit(-2);
    ui_pa->execute();
    if(ui_pa->is_error()){
        auto info=QByteArray(ui_pa->geterrorinfo().c_str());
        ui_compile_error(ui_codec->toUnicode(info));
        return;
    }

    //语义分析
    ui_sa=new SemanticAnalyzer(*ui_pa);
    if(!ui_sa) exit(-3);
    ui_sa->execute();
    if(ui_sa->is_error()){
        auto info=QByteArray(ui_sa->geterrorinfo().c_str());
        ui_compile_error(ui_codec->toUnicode(info));
        return;
    }
    QString name=ui_GetName(ui_src);
    if(GenQuat){
        QString Quatname=name+"_Quat.txt";
        ui_sa->export_to_txt(Quatname.toStdString().c_str());
        QString info=tr("编译成功，中间代码写入到同目录下")+Quatname+tr("中");
        ui_compile_success(info);
    }

    //生成目标代码
    if(GenAsm){
        ui_ag=new AsmGenerator(*ui_sa);
        if(!ui_ag) exit(-3);
        ui_ag->generate();
        QString Asmname=name+".asm";
        ui_ag->export_to_asm(Asmname.toStdString().c_str());
        QString info=tr("编译成功，目标代码写入到同目录下")+Asmname+tr("中");
        ui_compile_success(info);
    }
    //编译成功
    QMessageBox::information(
                nullptr,
                QStringLiteral("提示"),
                QStringLiteral("编译成功！"));
    ui_success=true;
}



/* 槽函数 */


void UI::on_actionGenQuat_triggered(){
    ui_Compile(true,false);
}

void UI::on_actionGenAsm_triggered(){
    ui_Compile(true,true);
}

void UI::on_actionOpen_triggered(){
    //选择文件
    QString fp=QFileDialog::getOpenFileName(
        this,tr("open C file"),"./",tr("C Source Fild(*.c)")
    );
    if(fp.isEmpty()) return;
    //若已有文件打开，先关闭之
    if(ui_src!="")
        on_actionClose_triggered();
    QFile file(fp);
    //try catch打开文件
    try {
        file.open(QIODevice::ReadOnly);
        this->ui_src=fp;
    } catch (QFile&) {
        QMessageBox::information(NULL,tr("警告"),tr("文件")+fp+tr("无法打开"));
        return;
    }

    QTextCodec* codec=QTextCodec::codecForName("GB2312");

    //先清空框
    this->ui->SourceText->clear();
    //填入到代码区域
    while(!file.atEnd()){
        QByteArray line=file.readLine();
        this->ui->SourceText->insertPlainText(codec->toUnicode(line));
    }
    file.close();
    //填入日志
    auto date=QDateTime::currentDateTime();
    QString time=date.toString("yyyy.MM.dd hh:mm:ss");
    QString log=time+tr(" 打开文件")+fp+"\r";
    this->ui->InfoText->insertPlainText(log);
    //更改标题
    this->setWindowTitle(ui_GetName(ui_src)+".c");
    //设置编译标志
    ui_success=false;
}


void UI::on_actionClose_triggered(){
    if(ui_src.isEmpty())
        return;
    //清空文本框
    this->ui->SourceText->clear();
    //插入日志
    auto date=QDateTime::currentDateTime();
    QString time=date.toString("yyyy.MM.dd hh:mm:ss");
    QString log=time+tr(" 关闭文件")+ui_src+"\r";
    this->ui->InfoText->insertPlainText(log);
    //修改ui_fp,ui_success
    ui_src="";
    ui_success=false;
    //更改标题
    this->setWindowTitle(ui_title);
}


void UI::on_actionExit_triggered()
{
    this->close();
}



void UI::on_actionLexical_triggered(){
    //先判断有无打开文件以及编译成功
    if(ui_checksuccess()==false)
        return;
    //获取文件名
    QString name=ui_GetName(ui_src)+".c";
    //创建窗口并打开
    auto SubUI=new LexicalUI(name+" 词法分析结果",*ui_la);
    SubUI->show();
    //delete SubUI;
}

void UI::on_actionParser_triggered(){
    //先判断有无打开文件以及编译成功
    if(ui_checksuccess()==false)
        return;
    //获取文件名
    QString name=ui_GetName(ui_src)+".c";
    //创建窗口并打开
    auto SubUI=new ParserUI(name+" 语法分析结果",*ui_pa);
    SubUI->show();
}

void UI::on_actionGrammar_triggered(){
    LL1Grammar* g=new LL1Grammar();
    g->Init("Grammar.txt");
    GrammarUI* SubUI=new GrammarUI(*g);
    SubUI->show();
    delete g;
}

void UI::on_actionparserTree_triggered(){
    //先判断有无打开文件以及编译成功
    if(ui_checksuccess()==false)
        return;
    //获取文件名
    QString name=ui_GetName(ui_src)+".c";
    //直接用cmd打开创建的图片
    QMessageBox::information(nullptr,tr("提示"),tr("稍后会生成语法树图片，请不要关闭黑窗！"));
    MakeDotFile(*(ui_pa->ps_Tree));
    system("tree.png");
}

void UI::on_actionSemantic_triggered(){
    //先判断有无打开文件以及编译成功
    if(ui_checksuccess()==false)
        return;
    //获取文件名
    QString name=ui_GetName(ui_src)+".c";
    //创建窗口并打开
    auto SubUI=new SemanticUI(name+" 语法分析结果",*ui_sa);
    SubUI->show();
}

void UI::on_actionPredictTable_triggered(){
    LL1Grammar* g=new LL1Grammar();
    g->Init("Grammar.txt");
    PredTableUI* SubUI=new PredTableUI(*g);
    SubUI->show();
    delete g;
}

void UI::on_actionHelp_triggered(){
    auto SubUI=new Help();
    SubUI->show();
}

void UI::on_actionAbout_triggered(){
    QMessageBox::information(
        nullptr,
        tr("关于"),
        tr("程序名称：LinCC\r\
            作者：林佳奕\r\
            学号：1951444\r\
            专业：计算机科学与技术\r\
            指导教师：高秀芬\r\
            完成日期：2022-1-27")
    );
}
