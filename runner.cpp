#include "runner.h"

namespace BrainFuck{

QString Runner::RunCode(const QString &code, BFList &tape, BFList InputSequence, int pos)
{
    int inputs = 0;
    QString output;
    QString::ConstIterator o = code.constBegin();
    int loops = 0;
    BFList::Iterator i = tape.begin()+pos;
    for (; o<code.constEnd() && !stop; ++o){
        switch ((*o).toLatin1()) {
        case '>': if(++i == tape.end()) tape.append(0); break;
        case '<': --i; break;
        case '+': ++(*i); break;
        case '-': --(*i); break;
        case ',': *i = InputSequence.value(inputs); ++inputs; break;
        case '.': output+=QChar(*i);  break;
        case '[':
            if(*i) continue;
            ++loops;
            while (loops) switch((*++o).toLatin1()){
            case '[': loops++; break;
            case ']': loops--; break;
            }
        case ']':
            if(!(*i)) continue;
            ++loops;
            while (loops) switch((*--o).toLatin1()){
            case '[': loops--; break;
            case ']': loops++; break;
            }
        }
    }
    ptr = i - tape.begin();
    return output;
}

void Runner::setStop()
{
    qInfo() << "setStop";
    stop = true;
}

void Runner::resetStop()
{
    qInfo() << "resetStop";
    stop = false;
}

Runner::Runner(QString &string, BFList &array, BFList &is, QObject *parent): code(string), tape(array), IS(is)
{
    ptr = 0;
    end = code.size();

}

void Runner::Run(QString what, int TapePos)
{
    QMutexLocker tipidor(&Nigga);
    QString output = RunCode(what, tape, IS, TapePos);
    emit Finished(output);
    emit finished(ptr);

}
void Runner::RecieveInput(unsigned char ch) // REMOVE LATER //
{

}

void Runner::RunWithoutDebug(QString code, BFList InputSequence)
{
    //qInfo() << "starting running w debug on" << QThread::currentThread();
    //qInfo() << code << InputSequence;
    BFList temp(1023);
    QString output = RunCode(code, temp, InputSequence);
    //qInfo() << output;
    emit FinishedWDRun(output);
}

} // namespace BrainFuck
