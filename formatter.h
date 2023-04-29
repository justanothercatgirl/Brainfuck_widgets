#ifndef FORMATTER_H
#define FORMATTER_H

#include <QRegularExpression>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QMap>

#ifndef q
#define q Qt::QueuedConnection
#endif
#define RemoveSup code.remove(QRegularExpression("<sup>\\d+</sup>"))
//#define remhtml remove(QRegularExpression("<span style=\" font-weight:\\d+; color:#(ff0000|00ff00|00ffff|0000ff); vertical-align:super;\">\\d*</span>"))

class Formatter : public QObject
{
    Q_OBJECT
public:
    explicit Formatter(QObject *parent = nullptr);
    Formatter(bool newCollapseShortCycles, bool newSepLineOPB, bool newSepLineCLB, int newMinimalSupIndex,
              bool newFormattingEnabled, bool newColorizingEnabled, int newAmountOfSpaces, QObject *parent = nullptr);

    const QString RawCodeSymbs = QString(" <>,.-+[]/#{}\n\r\n");
    QMap<QChar, QString> FormattingMap =
    {
        {'[', "#00FFFF"},
        {']', "#00FFFF"},
        {'.', "#00FF00"},
        {',', "#00FF00"},
        {'+', "#FF0000"},
        {'-', "#FF0000"},
        {'}', "#0000FF"},
        {'{', "#0000FF"},
        {' ', ""}
    };

    bool isValidIndex(int i, QString &code) const;
    bool isCodeSymb(QChar chr);
    void InsertNewLine(int &pos, QString &code);
    bool InsertNewLine(QString &code, int pos);
    void InsertSpaces(int &pos, int Amount, QString &code, bool Reverse = false);
    int  InsertSpaces(QString &code, int pos, int Amount);
    bool SkipComment(int &i, QString &code, bool inverse = false);
    int CountOBC(QString &code, int Start) const;

    bool CollapseShortCycles = true;        // GlobalSettings.json //
    bool SepLineOPB = false;                // GlobalSettings.json //
    bool SepLineCLB = true;                 // GlobalSettings.json //
    int MinimalSupIndex = 3;                // GlobalSettings.json //
    bool FormattingEnabled = true;          // GlobalSettings.json //
    bool ColorizingEnabled = true;          // GlobalSettings.json //
    int AmountOfSpaces = 6;                 // GlobalSettings.json //

    void setFormattingMap(const QMap<QChar, QString> &newFormattingMap);

public slots:
    void FormatCode(QString code);
    void ColorizeCode(QString code);
    void doBoth(QString code);
signals:
    void FinishedFormatting(QString code);  //connect to widget
    void FinishedColorizing(QString code);  //connect to widget
    void didBoth(QString code);             //connect to widget

};

#endif // FORMATTER_H
