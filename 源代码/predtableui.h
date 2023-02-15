#ifndef PREDTABLEUI_H
#define PREDTABLEUI_H

#include <QWidget>
#include"src/LL1Grammar.h"
#include<QStandardItemModel>

namespace Ui {
class PredTableUI;
}

class PredTableUI : public QWidget
{
    Q_OBJECT
private:
    void load(LL1Grammar& g);


public:
    explicit PredTableUI(LL1Grammar& g,
                         QWidget *parent = nullptr);
    ~PredTableUI();

private:
    Ui::PredTableUI *ui;
};

#endif // PREDTABLEUI_H
