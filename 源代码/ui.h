#ifndef UI_H
#define UI_H

#include <QMainWindow>
#include "src/LexicalAnalyzer.h"
#include "src/Parser.h"
#include "src/SemanticAnalyzer.h"
#include "src/AsmGenerator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UI; }
QT_END_NAMESPACE

QString ui_GetName(QString src);

void MakeDotFile(ParserTree& tree);

class UI : public QMainWindow
{
    Q_OBJECT

public:
    UI(QWidget *parent = nullptr);
    ~UI();

private:
    LexicalAnalyzer* ui_la=nullptr;
    Parser* ui_pa=nullptr;
    SemanticAnalyzer* ui_sa=nullptr;
    AsmGenerator* ui_ag=nullptr;

private:
    const QString ui_title="LinCC UI";
    QTextCodec* ui_codec=nullptr;
    QString ui_src="";
    QString ui_compile_src="";
    bool ui_success=false;


private:
    bool ui_checksuccess();

    void ui_compile_success(QString info);

    void ui_compile_error(QString info);

    void ui_Compile(bool GenQuat=false,bool GenAsm=false);


private slots:
    void on_actionGenQuat_triggered();

    void on_actionOpen_triggered();

    void on_actionClose_triggered();

    void on_actionExit_triggered();

    void on_actionGenAsm_triggered();

    void on_actionLexical_triggered();

    void on_actionParser_triggered();

    void on_actionGrammar_triggered();

    void on_actionparserTree_triggered();

    void on_actionSemantic_triggered();

    void on_actionPredictTable_triggered();

    void on_actionHelp_triggered();

    void on_actionAbout_triggered();

private:
    Ui::UI *ui;
};
#endif // UI_H
