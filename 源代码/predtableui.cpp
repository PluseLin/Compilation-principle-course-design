#include "predtableui.h"
#include "ui_predtableui.h"


void PredTableUI::load(LL1Grammar& g){
    int charwidth=10;
    std::vector<int> width;
    //创建模型
    auto model=new QStandardItemModel();

    //取出Vt，Vn
    auto VtSet=g.GetVtSet();
    auto VnSet=g.GetVnSet();
    width.assign(VtSet.size(),50);
    //行名称非终符，列名称是终结符
    //修改列名称
    model->setColumnCount(int(VtSet.size()));
    int i=0;
    for(Vt vt:VtSet){
        model->setHeaderData(i++,Qt::Horizontal,QString::fromStdString(vt));
    }
    //修改行名称，并填入每行，调整列宽
    model->setRowCount(int(VnSet.size()));
    i=0;
    for(Vn vn:VnSet){
        int j=0;
        model->setHeaderData(i,Qt::Vertical,QString::fromStdString(vn));
        for(Vt vt:VtSet){
            //调用预测分析表接口，取出产生式
            auto prod=g.PredProduction(vn,vt);
            QString temp;
            if(prod.right.size()==0)
                temp="";
            else
                temp=QString::fromStdString(prod.str());
            width[j]=std::max(width[j],temp.length()*charwidth);
            model->setItem(i,j,new QStandardItem(temp));
            j++;
        }
        i++;
    }
    //绑定
    ui->Table->setModel(model);
    //改变列宽
    for(int i=0;i<width.size();i++){
        ui->Table->setColumnWidth(i,width[i]);
    }
}

PredTableUI::PredTableUI(LL1Grammar& g,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PredTableUI)
{
    ui->setupUi(this);
    load(g);
    this->setWindowTitle("LL1预测分析表");
}

PredTableUI::~PredTableUI()
{
    delete ui;
}
