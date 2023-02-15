#include "parserui.h"
#include "ui_parserui.h"

void ParserUI::load(Parser& ps){
    //加载分析记录
    auto log=ps.GetLog();
    ui->InfoText->clear();
    ui->ProdText->clear();

    for(std::string line:log){
        QString temp=psui_codec->toUnicode(line.c_str())+"\r";
        ui->InfoText->insertPlainText(temp);
    }

    //加载使用的产生式(dfs)
    std::vector<std::vector<Word> > Prods;
    ps.ps_Tree->dfs_export(Prods);
    for(auto Prod:Prods){
        QString temp;
        for(auto p=Prod.begin();p!=Prod.end();p++){
            std::string w=(p->id=="$Vn")?p->value:p->id;
            QString word=psui_codec->toUnicode(w.c_str());
            temp+=word+" ";
            if(p==Prod.begin()) temp+="-> ";
        }
        temp+="\r";
        ui->ProdText->insertPlainText(temp);
    }
}

ParserUI::ParserUI(QString name,
                   Parser& ps,
                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParserUI)
{
    ui->setupUi(this);

    psui_codec=QTextCodec::codecForName("GB2312");

    QFont font;
    QFontMetrics metrics(font);
    this->ui->ProdText->setTabStopWidth(4*metrics.width(' '));
    this->ui->ProdText->setReadOnly(true);
    this->ui->InfoText->setTabStopWidth(4*metrics.width(' '));
    this->ui->InfoText->setReadOnly(true);

    load(ps);

    this->setWindowTitle(name);
}

ParserUI::~ParserUI(){
    delete ui;
}
