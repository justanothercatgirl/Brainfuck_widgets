#ifndef WIDGET_DEBUGGER_H
#define WIDGET_DEBUGGER_H

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QKeyEvent>
#include "runner.h"
#include <QThread>
#include <QWidget>
#include <QLabel>

#ifndef q
#define q Qt::QueuedConnection
#endif


namespace BrainFuck {

class Debugger : public QWidget
{
    Q_OBJECT

    friend class Runner;
    void GeneralSettings();
    void Connections();
    void InitWidgets();
    void SetLayout();
public:
    explicit Debugger(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    BFList tape = BFList(1023, 0);
    BFList is = BFList(0);
    int ptr = 0;
    int codepos = 0;
    int DebugArrayCellNum;
    QString RawCode;
    QString HtmlCode;
    QString CodeOutput = "";
    QString Compr = QString("+-<>,.][");

    QGridLayout *Layout = new QGridLayout(this);
    QHBoxLayout *TapeLayout;
    QList<QLabel*> TapeLabelList;
    QTextEdit *CodeViewer;
    QLabel *OutputLabel;
    QLineEdit *InputLine;
    QLabel *KeyboardShortcuts;

    QThread RT;
    Runner *runner;

    void setDebugArrayCellNum(int newDebugArrayCellNum);
    void do_something(int how);

    QString DetExecCode(char key);
    QString SkipCycleIter();
    QString SkipCycleCompl();
    QString NextStep();
    QString ToNextCycle();
    QString SkipToPoint();
    void Rewind();

public slots:
    void setCode(const QString &code);
    void setis (const QString &newis);
    void reset();
    void setPtr(int newPtr);

    void UpdateEverything(QString Output);
    void UpdateVisibleTape();                        //connect to runner//
    void UpdateCodeViewer();                         //connect to runner//
    void UpdateOutput();                             //connect to runner//
    unsigned char input();                           //connect to runner//

signals:
    void RunCode(QString what, int TapePos = 0);
    void SendInput(unsigned char ch);
    void FinishedRunning(QString output);
    void terminateRunning();



};

} // namespace BrainFuck

#endif // WIDGET_DEBUGGER_H
