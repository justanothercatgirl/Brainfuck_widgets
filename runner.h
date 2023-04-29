#ifndef RUNNER_H
#define RUNNER_H

#ifndef q
#define q Qt::QueuedConnection
#endif

#include <QMutexLocker>
#include <QObject>
#include <QDebug>
#include <QThread> // // // // // // // // // // //
#include <atomic>
typedef QList<unsigned char> BFList;
namespace BrainFuck{

class Runner : public QObject
{
    Q_OBJECT

    std::atomic_bool stop = false;
public:
    QMutex Nigga;
    QString &code;
    BFList &tape;
    BFList &IS;
    explicit Runner(QString &string, BFList &array, BFList &is, QObject *parent = nullptr);
    int ptr, end;
    QString RunCode(const QString &code, BFList &tape, BFList InputSequence = BFList(0), int pos = 0);



public slots:
    void setStop();
    void resetStop();
    void Run(QString what, int TapePos = 0);
    void RecieveInput(unsigned char ch);
    void RunWithoutDebug(QString code, BFList InputSequence = BFList(0));


signals:
    void Finished(QString Output);
    void finished(int ptr);
    void RequestInput();
    void FinishedWDRun(QString output);

};

} // namespace BrainFuck

#endif // RUNNER_H
