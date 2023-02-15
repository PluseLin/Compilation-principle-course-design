#include "lexicalui.h"
#include "ui_lexicalui.h"

LexicalUI::LexicalUI(QString title,LexicalAnalyzer& la,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LexicalUI)
{
    ui->setupUi(this);
    QFont font;
    QFontMetrics metrics(font);
    this->ui->Text->setTabStopWidth(4*metrics.width(' '));
    this->ui->Text->setReadOnly(true);

    lxui_title=title;

    load(la);
}

void LexicalUI::load(LexicalAnalyzer& la){
    std::vector<Word> res;
    res.swap(la.GetWordList());
    std::cout<<la.GetWordList().size()<<std::endl;
    for(auto word:res){
        lxui_content+=QString::fromStdString(word.str())+"\r";
    }

    //填入文本框
    ui->Text->clear();
    ui->Text->insertPlainText(lxui_content);
    //修改标签
    this->setWindowTitle(lxui_title);
}

LexicalUI::~LexicalUI()
{
    lxui_content.clear();
    lxui_title.clear();
    delete ui;
}
