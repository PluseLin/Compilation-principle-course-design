#ifndef PARSERTREEUI_H
#define PARSERTREEUI_H

#include <QWidget>
#include<QtGui>
#include<QWheelEvent>
#include<QGraphicsView>
#include"src/Parser.h"

namespace Ui {
class ParserTreeUI;
}


class ParserTreeUI : public QWidget
{
    Q_OBJECT
private:
    void MakeDotFile(ParserTree& tree);
   // void MakeTreeView(ParserTree& tree);
public:
    explicit ParserTreeUI(QString name,
                          ParserTree& tree,
                          QWidget *parent = nullptr);
    ~ParserTreeUI();

private:
    Ui::ParserTreeUI *ui;
    QGraphicsScene* ptui_scene;
};

#endif // PARSERTREEUI_H
