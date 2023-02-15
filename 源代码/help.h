#ifndef HELP_H
#define HELP_H

#include <QWidget>
#include<QTextCodec>
namespace Ui {
class Help;
}

class Help : public QWidget
{
    Q_OBJECT
private:
    QTextCodec* ui_codec=nullptr;
    void load();
public:
    explicit Help(QWidget *parent = nullptr);
    ~Help();

private:
    Ui::Help *ui;
};

#endif // HELP_H
