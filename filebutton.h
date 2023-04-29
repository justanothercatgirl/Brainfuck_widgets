#ifndef FILEBUTTON_H
#define FILEBUTTON_H

#include <QColumnView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QMap>

class FileButton : public QPushButton
{
    Q_OBJECT
    Q_DISABLE_COPY(FileButton)
public:
    FileButton(QMap<QString, const char*> map, QString text = "", QWidget *parent = nullptr);
    QList<QPushButton*> ButtList;
    QStringList ButtTexts;
    QList<const char*> ButtSlots;
    QColumnView *Options; //WHERE THE FUCK DOES THESE OPTIONS SHOW???
    QVBoxLayout *OptionsLayout;
    void SendCMSignal();


public slots:
    void mousePressEvent(QMouseEvent *e);
    void ShowOptions();
    void ChON(QString s);
signals:
    void RightButtonClicked();
    void TransferSig();
};

#endif // FILEBUTTON_H
