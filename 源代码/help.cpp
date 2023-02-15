#include "help.h"
#include "ui_help.h"
#include<QMessageBox>
#include <QFile>
#include<iostream>

void Help::load(){
    QFile file("readme.txt");
    ui->HelpText->clear();
    try {
        file.open(QIODevice::ReadOnly);
    } catch (QFile&) {
        QMessageBox::information(nullptr,tr("错误"),tr("无法打开readme.txt"));
        this->close();
        return;
    }
    while(!file.atEnd()){
        QString line=ui_codec->toUnicode(file.readLine());
        ui->HelpText->insertPlainText(line);
    }
    file.close();
}

Help::Help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Help){

    ui->setupUi(this);

    QFont font;
    QFontMetrics metrics(font);
    this->ui->HelpText->setTabStopWidth(4*metrics.width(' '));
    this->ui->HelpText->setReadOnly(true);

    ui_codec=QTextCodec::codecForName("utf-8");

    load();

    this->setWindowTitle("帮助");
}

Help::~Help(){
    delete ui;
}
