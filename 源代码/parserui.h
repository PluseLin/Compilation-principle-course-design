#ifndef PARSERUI_H
#define PARSERUI_H

#include <QWidget>
#include"src/Parser.h"
#include"QTextCodec"

namespace Ui {
class ParserUI;
}

class ParserUI : public QWidget
{
    Q_OBJECT
private:
    QTextCodec* psui_codec=nullptr;

private:
    void load(Parser& ps);


public:
    explicit ParserUI(QString name,
                      Parser& ps,
                      QWidget *parent = nullptr);
    ~ParserUI();

private:
    Ui::ParserUI *ui;
};

#endif // PARSERUI_H
