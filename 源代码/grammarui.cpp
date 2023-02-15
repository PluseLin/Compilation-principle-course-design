#include "grammarui.h"
#include "ui_grammarui.h"

void GrammarUI::loadTable(LL1Grammar& g){
    std::set<Vn> VnSet=g.GetVnSet();
    //创建模型
    auto model=new QStandardItemModel();
    QHeaderView* headerView = ui->Table->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    //修改列名称
    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal, "FIRST");
    model->setHeaderData(1,Qt::Horizontal, "FOLLOW");
    //修改行名称，并填入内容
    model->setRowCount(VnSet.size());
    int i=0;
    int maxl=1;
    for(auto vn:VnSet){
        QVariant qv(QString::fromStdString(vn));
        model->setHeaderData(i,Qt::Vertical,qv);
        //FIRST
        std::set<Vt> first=g.First(vn);
        QString temp;
        int cnt=0,line=1;
        for(auto vt:first){
            temp+=QString::fromStdString(vt)+" ";
            if((++cnt)%5==0 && cnt!=first.size()) temp+="\n";
        }
        line=(cnt+4)/5;
        maxl=std::max(line,maxl);
        model->setItem(i,0,new QStandardItem(temp));
        //FOLLOW
        std::set<Vn> follow=g.Follow(vn);
        temp="";
        cnt=0,line=0;
        for(auto vt:follow){
            temp+=QString::fromStdString(vt)+" ";
            if((++cnt)%5==0 && cnt!=follow.size()) temp+="\n";
        }
        line=(cnt+4)/5;
        maxl=std::max(line,maxl);
        model->setItem(i,1,new QStandardItem(temp));
        i++;
    }
    //绑定
    ui->Table->setModel(model);
    //改变行高
    for(int i=0;i<VnSet.size();i++){
        ui->Table->setRowHeight(i,maxl*25);
    }
}

void GrammarUI::loadProd(LL1Grammar& g){
    //取出产生式
    auto Prod=g.GetProdList();
    //逐个填入到文本框
    ui->ProdText->clear();
    for(auto prod:Prod){
        QString temp=QString::fromStdString(prod.str())+"\r";
        ui->ProdText->insertPlainText(temp);
    }
}

GrammarUI::GrammarUI(LL1Grammar& g,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GrammarUI)
{
    ui->setupUi(this);
    loadTable(g);
    loadProd(g);
    this->setWindowTitle("LL1文法");
}

GrammarUI::~GrammarUI()
{
    delete ui;
}
