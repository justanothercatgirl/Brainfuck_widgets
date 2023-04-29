#include "debugger.h"
#include <QSizePolicy>
namespace BrainFuck{

void Debugger::GeneralSettings()
{
    setDebugArrayCellNum(10); //change in settings//
    setStyleSheet("QWidget{background-color: gray;}"
                  "QTextEdit{background-color: #999999; color: #1f1e33;}"
                  "QLabel{background-color: #999999; color: black; font: ExtraBold;}"
                  "QLineEdit{background-color: #999999; color: blue; font: Bold;}");
    setFocusPolicy(Qt::ClickFocus);
    resize(727, 420);
}
void Debugger::Connections()
{
    runner = new Runner(RawCode, tape, is);
    runner->moveToThread(&RT);

    connect(this, &Debugger::RunCode, runner, &Runner::Run, q);
    connect(runner, &Runner::RequestInput, this, &Debugger::input, q);
    connect(this, &Debugger::SendInput, runner, &Runner::RecieveInput, q);
    connect(runner, &Runner::Finished, this, &Debugger::UpdateEverything, q);
    connect(runner, &Runner::finished, this, &Debugger::setPtr);
    connect(InputLine, &QLineEdit::textEdited, this, &Debugger::setis);
    connect(this, &Debugger::terminateRunning, runner, &Runner::setStop);
}
void Debugger::InitWidgets()
{
    TapeLabelList = QList<QLabel*>(DebugArrayCellNum);
    QList<QLabel*>::Iterator i;
    for (i = TapeLabelList.begin(); i<TapeLabelList.end(); ++i)
        *i = new QLabel("0\nNull");

    CodeViewer = new QTextEdit(this);
    CodeViewer->setReadOnly(true);
    OutputLabel = new QLabel(this);
    UpdateOutput();
    InputLine = new QLineEdit(this);
    InputLine->setToolTip("Input Line (same as the one in Main window)");
    InputLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    InputLine->setStyleSheet("font: 24px;");
    KeyboardShortcuts = new QLabel(this);
    KeyboardShortcuts->setText("Key shortcuts:\n"
                               "F8: Next step (step: sequence of same characters, goes inside cycles)\n"
                               "F9: To beginning of next cycle\n"
                               "F10: Skip one cycle iteration (if not in cycle - same as F9)\n"
                               "F11: Skip nearest cycle completely (same as go to it's closing \']\')\n"
                               "F12: Go to next \'point\' (#, user-defined)");
    CodeViewer->setHtml(HtmlCode);
    CodeViewer->setText("Hello this\nshould\nwork???\n\n\ni guess"); // // // // // // //

}
void Debugger::SetLayout()
{
    TapeLayout = new QHBoxLayout();
    foreach(QLabel *x, TapeLabelList) TapeLayout->addWidget(x);

    Layout->addLayout(TapeLayout, 0, 0, 4, 27);
    Layout->addWidget(CodeViewer, 4, 0, 14, 10);
    Layout->addWidget(OutputLabel, 5, 11, 3, 15);
    Layout->addWidget(InputLine, 9, 11, 3, 15);
    Layout->addWidget(KeyboardShortcuts, 13, 11, 4, 15);
}
Debugger::Debugger(QWidget *parent): QWidget{parent}
{

    GeneralSettings();
    InitWidgets();
    SetLayout();
    Connections();
}

void Debugger::keyPressEvent(QKeyEvent *event)
{                                                        // /// //// ///// ////// /////// //////// ///////// //////// /////// ////// ///// //// /// //
    if(RawCode.size()<=3){                              // /// //// ///// ////// /////// //////// ///////// //////// /////// ////// ///// //// /// //
        this->hide();                                  // /// //// ///// ////// /////// //////// ///////// //////// /////// ////// ///// //// /// //
        return;                                       // /// //// ///// ////// /////// //////// ///////// //////// /////// ////// ///// //// /// //
    }                                                // /// //// ///// ////// /////// //////// ///////// //////// /////// ////// ///// //// /// //
                                                    // /// //// ///// ////// /////// //////// ///////// //////// /////// ////// ///// //// /// //
    char key;                                      // /// //// ///// ////// /////// //////// ///////// //////// /////// ////// ///// //// /// //
    switch (event->key()) {
    case Qt::Key_F8:
        qInfo() << "F8";
        key = 8;
        break;
    case Qt::Key_F9:
        qInfo() << "F9";
        key = 9;
        break;
    case Qt::Key_F10:
        qInfo() << "F10";
        key = 10;
        break;
    case Qt::Key_F11:
        qInfo() << "F11";
        key = 11;
        break;
    case Qt::Key_F12:
        qInfo() << "F12";
        key = 12;
        break;
    case Qt::Key_S:
        //qInfo() << "stooooooop";
        emit terminateRunning();
        return;
    default:
        qInfo() << event->key();
        break;
    }
    emit RunCode(DetExecCode(key), ptr);
}

void Debugger::setDebugArrayCellNum(int newDebugArrayCellNum)
{
    DebugArrayCellNum = newDebugArrayCellNum;
}
void Debugger::do_something(int how)
{
    while (how) {qInfo() << "doing somewthing on thread" << QThread::currentThread() << QString::number(how); how--;};
}

//ADD EOL CONDITION TO EVERY LOOP-SKIPPING CYCLE (AT LEAST, BETTER DEBUGGING OPTION);
QString Debugger::DetExecCode(char key)
{
    QString ExecCode;
    switch(key) {
    case 8:
        ExecCode = NextStep();
        break;
    case 9:
        ExecCode = ToNextCycle();
        break;
    case 10:
        ExecCode = SkipCycleIter();
        break;
    case 11:
        ExecCode = SkipCycleCompl();
        break;
    case 12:
        ExecCode = SkipToPoint();
        break;
    default:
        qInfo() << "key error";
        break;
    }
    return ExecCode;
}
QString Debugger::SkipCycleIter()
{
    bool InsideCycle;
    int i = codepos;
    int cStart = i, cEnd = i;
    int aStart, aEnd;
    QString addit = "";
    if (RawCode.at(i)!='['){
        while (cStart >= 0)
        { // Goind backwards
            --cStart;
            if (RawCode.at(cStart) == '[') {
                InsideCycle = true; // that means aEnd = cEnd //
                aStart = i;
                break;
            }
            if(RawCode.at(cStart) == ']') {
                InsideCycle = false;
                aStart = cStart;
                cStart = RawCode.indexOf('[', i);
                aEnd = cStart-2;
            }
        } // Goind backwards

        cEnd = cStart+1;

        while (cEnd < RawCode.size())
        { // Going forwards
            if(RawCode.at(cEnd) == ']') break;
            else if (RawCode.at(cEnd) == '['){
                int loops = 1;
                while (loops){
                    ++cEnd;
                    if(RawCode.at(cEnd) == '[') ++loops;
                    else if (RawCode.at(cEnd) == ']') --loops;
                }
            }
            ++cEnd;
        } // Going forwards
        if (InsideCycle) aEnd = cEnd-1;
        addit = RawCode.sliced(aStart+1, aEnd-aStart); // +1? -1? //
    }
    else{
        cStart = i;
        cEnd = cStart++;
        int loops = 1;
        while (loops){
            ++cEnd;
            if(RawCode.at(cEnd) == '[') ++loops;
            else if (RawCode.at(cEnd) == ']') --loops;
        }
        aStart = 0; aEnd = 0;
    }

    QString rString = addit + RawCode.sliced(cStart+1, cEnd-cStart);
    qInfo() << addit << " + " << RawCode.sliced(cStart+1, cEnd-cStart); // //////////////////////////////////////////////////////
    codepos = cEnd+1;
    return rString;
}
QString Debugger::SkipCycleCompl()
{
    bool InsideCycle;
    int i = codepos;
    int cStart = i, cEnd = i;
    int aStart, aEnd;
    QString addit = "";
    if (RawCode.at(i)!='['){
        while (cStart >= 0)
        { // Goind backwards
            --cStart;
            if (RawCode.at(cStart) == '[') {
                InsideCycle = true; // that means aEnd = cEnd //
                aStart = i;
                break;
            }
            if(RawCode.at(cStart) == ']') {
                InsideCycle = false;
                aStart = cStart;
                cStart = RawCode.indexOf('[', i);
                aEnd = cStart-2;
            }
        } // Goind backwards

        cEnd = cStart+1;

        while (cEnd < RawCode.size())
        { // Going forwards
            if(RawCode.at(cEnd) == ']') break;
            else if (RawCode.at(cEnd) == '['){
                int loops = 1;
                while (loops){
                    ++cEnd;
                    if(RawCode.at(cEnd) == '[') ++loops;
                    else if (RawCode.at(cEnd) == ']') --loops;
                }
            }
            ++cEnd;
        } // Going forwards
        if (InsideCycle) aEnd = cEnd-1;
        addit = RawCode.sliced(aStart+1, aEnd-aStart); // +1? -1? //
    }
    else{
        cStart = i;
        cEnd = cStart++;
        int loops = 1;
        while (loops){
            ++cEnd;
            if(RawCode.at(cEnd) == '[') ++loops;
            else if (RawCode.at(cEnd) == ']') --loops;
        }
        aStart = 0; aEnd = 0;
    }

    QString rString = addit + QString('[') + RawCode.sliced(cStart+1, cEnd-cStart) + QString(']');
    qInfo() << rString; // //////////////////////////////////////////////////////
    codepos = cEnd+1;
    return rString;
}
QString Debugger::NextStep()
{
    int i = codepos;
    if (RawCode.at(i) == ']'){
        int loops = 1;
        while (loops){
            --i;
            if (RawCode.at(i) == ']') ++loops;
            else if(RawCode.at(i) == '[') --loops;
        }
    }
    while (!Compr.contains(RawCode.at(i))) ++i;
    QChar temp = RawCode.at(i);
    QChar iter = temp;
    while(iter == temp || !Compr.contains(iter)) { ++i; iter = RawCode.at(i); }
    QString rString = RawCode.sliced(codepos, i-codepos);
    qInfo() << rString; // //////////////////////////////////////////////////////
    codepos = i;
    return rString;
}
QString Debugger::ToNextCycle()
{
    int i = codepos;
    while (!Compr.contains(RawCode.at(i))) ++i;
    if (RawCode.at(i) == '['){
        int loops = 1;
        while (loops){
            ++i;
            if(RawCode.at(i) == '[') ++loops;
            else if (RawCode.at(i) == ']') --loops;
        }
    }
    i = RawCode.indexOf('[', i);
    QString rString = RawCode.sliced(codepos, i-codepos);
    qInfo() << rString; // //////////////////////////////////////////////////////
    codepos = i;
    return rString;
}
QString Debugger::SkipToPoint()
{
    QString rString;
    int i = codepos;
    int a = RawCode.indexOf('#', i);
    if (a<0) return "";
    QString slice = RawCode.sliced(i, a-i);
    if(slice.count('[') == slice.count(']'))
    {
        codepos = a;
        return slice;
    }
    if(slice.count('[') < slice.count(']'))
    {
        int cS = i, cE = i, aS = i, aE = i;
        int loops = 1;
        while (loops && cS>=0){
            if(RawCode.at(cS) == ']') ++loops;
            else if (RawCode.at(cS) == '[') --loops;
            --cS;
        }
        ++loops;
        while (loops && cS<RawCode.size()){
            if(RawCode.at(cE) == '[') ++loops;
            else if (RawCode.at(cE) == ']') --loops;
            ++cE;
        }
        aE = cE-1;
        rString = RawCode.sliced(aS, aE-aS+1) + RawCode.sliced(cS, cE-cS+1) + RawCode.sliced(cE+1, a-cE);
    }
    else
    {
        int evilPos = RawCode.indexOf('[', i);
        rString = RawCode.sliced(i, a-i+1);
        rString.remove(evilPos, 1);
    }
    qInfo() << rString; // //////////////////////////////////////////////////////
    codepos = a;
    return rString;
}
void Debugger::Rewind()
{

}

void Debugger::setCode(const QString &code)
{
    RawCode = code;
}
void Debugger::setis(const QString &newis)
{
    QList l1 = newis.split(", ");
    for(int j = 0; j<l1.size(); j++){
        QString i = l1.at(j);
        bool ok;
        unsigned char ch = static_cast<unsigned char>(i.toInt(&ok));
        if(ok) is[j] = ch;
    }
    qInfo() << is; // // // // // // // // // // // // //
}
void Debugger::reset()
{
    tape = BFList(1023, 0);
    is = BFList(0);
    ptr = 0;
    codepos = 0;
    QString CodeOutput = "";
    runner->resetStop();
}

void Debugger::setPtr(int newPtr)
{
    ptr = newPtr;
}

void Debugger::UpdateEverything(QString Output)
{
    UpdateVisibleTape();
    UpdateCodeViewer();
    UpdateOutput();
}
void Debugger::UpdateVisibleTape()
{
    QList<QLabel*>::Iterator i = TapeLabelList.begin();
    BFList::Iterator j = tape.begin() + (ptr < DebugArrayCellNum >> 1 ? 0 : ptr);
    while (i!=TapeLabelList.end() && j!=tape.end()) {
        (*i)->setText( QString("%1%2").arg(QString::number(*j)).arg(QChar(*j)) );
        ++j;
        ++i;
    }
}
void Debugger::UpdateCodeViewer()
{

}
void Debugger::UpdateOutput()
{
    OutputLabel->setText(CodeOutput.isEmpty()?QString("code output"):CodeOutput);
}
unsigned char Debugger::input()
{
    unsigned char x = 'A';

    return x;
}


} //namespace BrainFuck
