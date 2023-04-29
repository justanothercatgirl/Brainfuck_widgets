#include "formatter.h"

Formatter::Formatter(QObject *parent): QObject{parent} {}
Formatter::Formatter(bool newCollapseShortCycles, bool newSepLineOPB, bool newSepLineCLB, int newMinimalSupIndex, bool newFormattingEnabled, bool newColorizingEnabled, int newAmountOfSpaces, QObject *parent)
    : CollapseShortCycles(newCollapseShortCycles), SepLineOPB(newSepLineOPB), SepLineCLB(newSepLineCLB), MinimalSupIndex(newMinimalSupIndex), FormattingEnabled(newFormattingEnabled), ColorizingEnabled(newColorizingEnabled), AmountOfSpaces(newAmountOfSpaces), QObject{parent}
{}

void Formatter::setFormattingMap(const QMap<QChar, QString> &newFormattingMap)
{
    FormattingMap = newFormattingMap;
}

void Formatter::FormatCode(QString code)
{

    emit FinishedFormatting(code);
}
void Formatter::ColorizeCode(QString code)
{
    RemoveSup;
    QString ReplStr("<b style=\"color:%1;\">%2%3</b>");
    QString SupStr("<sup>%1</sup>");
    code.replace('>', '}');
    code.replace('<', '{');
    code.replace("\r\n", "<br>");
    for(int iter = code.size()-1; iter>0; iter--){
        if (code.at(iter) == '/') SkipComment(iter, code, true);
        if(!FormattingMap.keys().contains(code.at(iter))) continue;
        int j = iter;
        QChar symb = code.at(iter);
        while (code.at(iter) == code.at(iter-1)) {iter--;if(!iter)break;}
        if (symb == ' ') code.replace(iter, j-iter+1, ReplStr.arg("").arg(QString("&nbsp;").repeated(j-iter+1)).arg(""));
        else if (j-iter) code.replace(iter, j-iter+1, ReplStr.arg(FormattingMap.value(symb)).arg(QString(symb).repeated(j-iter+1)).arg(j-iter+1>=MinimalSupIndex?SupStr.arg(QString::number(j-iter+1)):""));
        else code.replace(iter, 1, ReplStr.arg(FormattingMap.value(symb)).arg(symb).arg(""));
        if (iter == 1 && FormattingMap.keys().contains(code.at(0))){
            QChar symb = code.at(0);
            code.replace(0, 1, ReplStr.arg(FormattingMap.value(symb)).arg(symb).arg(""));
        }

    }
    code.replace('}', "&gt;");
    code.replace('{', "&lt;");
    emit FinishedColorizing(code);
}
void Formatter::doBoth(QString code)
{
    FormatCode(code);
    ColorizeCode(code);
}

bool Formatter::isValidIndex(int i, QString &code) const
{
    return (i>=0 && i<code.size());
}
bool Formatter::isCodeSymb(QChar chr)
{
    if (RawCodeSymbs.contains(chr)) return true;
    else return false;
}
void Formatter::InsertNewLine(int &pos, QString &code)
{
    if (InsertNewLine(code, pos)) pos+=2;
}
bool Formatter::InsertNewLine(QString &code, int pos)
{
    if (pos>=code.size()-2) return false;
    bool Unneeded = code.sliced(pos, 2) =="\r\n" || code.at(pos) == '\n';
    if (Unneeded) return false;
    code.insert(pos, "\r\n");
    return true;
}
void Formatter::InsertSpaces(int &pos, int Amount, QString &code, bool Reverse)
{
    if (Reverse) pos-=InsertSpaces(code, pos, Amount);
    else pos+=InsertSpaces(code, pos, Amount);
}
int  Formatter::InsertSpaces(QString &code, int pos, int Amount)
{
    if(!pos) return 0;
    int Needed, j = pos;
    while (code.at(j) == ' ') {j++; if (j+1>code.size()) break;}
    Needed = Amount - (j-pos);
    code.insert(pos, QString(' ').repeated(Needed));
    return Needed;
}
bool Formatter::SkipComment(int &i, QString &code, bool inverse)
{
    if (i>=code.size() || i<0) return false;
    if (code.at(i) != '/') return false;
    int j;
    if (inverse) j = code.lastIndexOf('/', i-1)-1;
    else j = code.indexOf('/', i+1)+1;
    if (!isValidIndex(j, code)) return false;
    i = j;
    return true;
}
int  Formatter::CountOBC(QString &code, int Start) const
{
    int OPBA = code.sliced(0, Start+1).count('[');
    int CLBA = code.sliced(0, Start+1).count(']');
    return OPBA-CLBA;
}
