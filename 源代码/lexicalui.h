#ifndef LEXICALUI_H
#define LEXICALUI_H

#include <QWidget>
#include"src/LexicalAnalyzer.h"

namespace Ui {
class LexicalUI;
}

class LexicalUI : public QWidget
{
    Q_OBJECT

private:
    QString lxui_content;
    QString lxui_title;
private:
    void load(LexicalAnalyzer& la);

public:
    explicit LexicalUI(QString title,
                       LexicalAnalyzer& la,
                       QWidget *parent = nullptr);
    ~LexicalUI();

private:
    Ui::LexicalUI *ui;
};

#endif // LEXICALUI_H
