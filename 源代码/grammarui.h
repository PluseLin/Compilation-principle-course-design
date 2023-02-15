#ifndef GRAMMARUI_H
#define GRAMMARUI_H

#include <QWidget>
#include"src/LL1Grammar.h"
#include<QStandardItemModel>

namespace Ui {
class GrammarUI;
}

class GrammarUI : public QWidget
{
    Q_OBJECT
private:
    void loadTable(LL1Grammar& g);
    void loadProd(LL1Grammar& g);

public:
    explicit GrammarUI(LL1Grammar& g,QWidget *parent = nullptr);
    ~GrammarUI();

private:
    Ui::GrammarUI *ui;
};

#endif // GRAMMARUI_H
