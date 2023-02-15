#include "semanticui.h"
#include "ui_semanticui.h"

void SemanticUI::loadQuat(SemanticAnalyzer& sa){
    auto QuatList=sa.GetQuatList();
    ui->QuatText->clear();
    int i=0;
    for(auto quat:QuatList){
        QString line=QString::fromStdString(
                    std::to_string(i++)+"\t"+quat.str()+"\r"
                    );
        ui->QuatText->insertPlainText(line);
    }
}

void SemanticUI::loadFunc(SemanticAnalyzer& sa){
    auto FuncTable=sa.GetFuncTable();
    ui->FuncText->clear();
    for(auto pr:FuncTable){
        QString line=QString::fromStdString(
            "函数名称:"+pr.second.name+"\r"+
            "函数返回类型:"+pr.second.type+"\r"+
            "函数参数个数:"+std::to_string(pr.second.argc)+"\r"+
            "函数长度:"+std::to_string(pr.second.Quatcnt)+"\r"+
            "函数起始位置:"+std::to_string(pr.second.startpos)+"\r"+
            "\r"
        );
        ui->FuncText->insertPlainText(line);
    }
}

void SemanticUI::loadVar(SemanticAnalyzer& sa){
    auto VarTable=sa.GetVarTable();
    ui->VarText->clear();
    for(auto pr:VarTable){
        QString line=QString::fromStdString(
            "变量名称:"+pr.second.name+"\r"+
            "变量类型:"+pr.second.type+"\r"+
            "变量作用域:"+pr.second.field+"\r"+
            "第几个变量:"+std::to_string(pr.second.Varcnt)+"\r"+
            "维数:"+std::to_string(pr.second.width.size())+"\r"+
            "\r"
        );
        ui->VarText->insertPlainText(line);
    }
}

SemanticUI::SemanticUI(QString name,
                       SemanticAnalyzer& sa,
                       QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SemanticUI)
{
    ui->setupUi(this);
    QFont font;
    QFontMetrics metrics(font);
    this->ui->QuatText->setTabStopWidth(4*metrics.width(' '));
    this->ui->QuatText->setReadOnly(true);

    loadQuat(sa);
    loadFunc(sa);
    loadVar(sa);

    this->setWindowTitle(name);
}

SemanticUI::~SemanticUI()
{
    delete ui;
}
