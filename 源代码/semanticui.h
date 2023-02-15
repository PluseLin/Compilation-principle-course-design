#ifndef SEMANTICUI_H
#define SEMANTICUI_H

#include <QWidget>
#include "src/SemanticAnalyzer.h"


namespace Ui {
class SemanticUI;
}

class SemanticUI : public QWidget
{
    Q_OBJECT
private:
    void loadQuat(SemanticAnalyzer& sa);
    void loadFunc(SemanticAnalyzer& sa);
    void loadVar(SemanticAnalyzer& sa);

public:
    explicit SemanticUI(QString name,
                        SemanticAnalyzer& sa,
                        QWidget *parent = nullptr);
    ~SemanticUI();

private:
    Ui::SemanticUI *ui;
};

#endif // SEMANTICUI_H
