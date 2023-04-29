#ifndef WIDGET_H
#define WIDGET_H

#include <QScopedPointer>
#include <QStackedWidget>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonObject>
#include <QFileDialog>
#include <QScrollArea>
#include <QJsonArray>
#include <QComboBox>
#include <QFile>
#include <QDir>
#include "filebutton.h"  //QColumnView, QHBoxLayout, QPushButton, QMouseEvent, QMap
#include "formatter.h"   //QRegularExpression, QObject, QThread, QDebug, QMap
#include "debugger.h"    //QGridLayout, QHBoxLayout, QLineEdit, QTextEdit, QWidget, QLabel, QThread, runner.h
#ifndef q                 //"runner.h": QMutexLocker, QObject, QDebug, QKeyEvent
    #define q Qt::QueuedConnection
#endif
#ifdef RemoveSup
    #undef RemoveSup
#endif
#define CS new QLabel("Coming soon!")
typedef QMap<QString, const char*> ButtonMap;

struct LayoutWidget
{
    QWidget *widget;
    QGridLayout *layout;
};

class Widget : public QWidget
{
    Q_OBJECT

    friend class FileButton;
private:
    void InitSettings();
    void InitColors();
    void restoreDeafultColors();
    void SetPageNames();
    void ConnectFormatter();
    void ConnectDebugger();
    void GeneralAppSettings();
    QString RemoveSup();

    // W I D G E T //
    QGridLayout *layout = new QGridLayout(this);
    QStackedWidget *FocusWidget = new QStackedWidget(this);
    void InitAllPages();
    QComboBox *FocusWidgetModes;
    const ButtonMap ConstButt =
    {
        {"IDEPreferences", SLOT(IDEPreferences())},
        {"IDEHelp", SLOT(IDEHelp())},
        {"Open", SLOT(Open())},
        {"SaveAs", SLOT(SaveAs())},
        {"NewFile", SLOT(NewFile())},
        {"About", SLOT(About())},

        {"Home", SLOT(ChangeFocusWidget())},
        {"Edit", SLOT(ChangeFocusWidget())},
        {"Run", SLOT(DebugTriggered())},
        {"Settings", SLOT(ChangeFocusWidget())},
        {"Help", SLOT(ChangeFocusWidget())}
    };
    const QMap<QString, int> ButtonsToFocusWidget
    {
        {"Home", 0},
        {"Edit", 1},
        {"Settings", 2},
        {"Help", 3}
    };
    const QStringList ConstButtNames =
    {
        "IDEPreferences", "IDEHelp", "Open", "SaveAs", "NewFile", "About",
        "Home", "Edit", "Run", "Settings", "Help"
    };
    QLabel *OutputLabel;

    // H O M E P A G E //
    const QStringList ButtNames = {"Open", "Create New", "Settings"};
    const QList<const char*> ButtSlots = {SLOT(Open()), SLOT(NewFile()), SLOT(SettPage())};
    QList<QPushButton*> ButtList;
    QStringList about;

    // E D I T P A G E //
    const QString RawCodeSymbs = QString(" \t<>,.-+[]/#{}\n\r\n");
    const QString RawRawCodeSymbs = QString("[]<>-+,.#");
    QMap<QChar, QString> FormattingMap;
    const ButtonMap RUNBUTTONMAP =
    {
        {"Change run mode", SLOT(ChangeRunMode())}
    };
    const ButtonMap FILEBUTTONMAP =
    {
        {"Close File", SLOT(Close())},
        {"Do nothing", SLOT(DoNothing())}
    };
    QMap<QString, QPushButton*> FileButts;
    QVBoxLayout *FileLayout;
    QLabel *DeafultLabel;
    QString CurrentFile;
    QColumnView *Files;
    QTextEdit *Editor;
    QLineEdit *Inputs;

    // D E B U G G I N G //
    QThread FT;
    BrainFuck::Debugger *debugger;
    Formatter *formatter;

    // S E T T P A G E //
    QStringList OptionsList;

public:
    Widget(QWidget *parent = nullptr);
    ~ Widget();
    const QString GetRawCode(bool PreserveComments = false);
    QMap<QString, QStringList> FilesOpened;
    QLabel *ComingSoon = CS;
    QMap<QString, QString> ColorMap =
    {
        {"GeneralBackground", ""},
        {"GeneralText",       ""},
        {"ButtonBackground",  ""},
        {"OutputBackground",  ""},
        {"OutputBorder",      ""},
        {"OutputText",        ""},
        {"EditorBackground",  ""},
        {"Format[]",          ""},
        {"Format<>",          ""},
        {"Format.,",          ""},
        {"Format+-",          ""}

    };

    bool DebugActive;                      //   GlobalSettings.json   //
    bool CollapseShortCycles;              //   GlobalSettings.json   //
    bool SepLineOPB;                       //   GlobalSettings.json   //
    bool SepLineCLB;                       //   GlobalSettings.json   //
    int  MinimalSupIndex;                  //   GlobalSettings.json   //
    bool FormattingEnabled;                //   GlobalSettings.json   //
    bool ColorizingEnabled;                //   GlobalSettings.json   //
    int  AmountOfSpaces;                   //   GlobalSettings.json   //
    int  DebugArrayCellNum;                //   GlobalSettings.json   //
    int  ZoomInValue;                      //   GlobalSettings.json   //
    void keyPressEvent(QKeyEvent *e);

private slots:
    // OPENED FILES
    void Close();
    void FileButtPushed();
    // DIALOGUES
    void IDEPreferences();
    void IDEHelp();
    void Open();
    void SaveChanges();
    void SaveAs(bool SaveRaw = false, bool SaveRawRaw = false);
    void NewFile();
    void About();
    // MAIN WIDGET MODES
    void updateEditPage();
    // BRAINFUCK DEBUG
    void DebugTriggered();
    // OTHER

public slots:
    FileButton  *createButton(QString text, char type);
    QPushButton *CreateButton(QString text, const char* slot, QGridLayout* layout,
                              int x = 0, int y = 0, int xspan = 0, int yspan = 0);
    QPushButton *CreateButton(QString text, const char* slot);
    void UpdateFormatted(QString code);
    void ChangeRunMode();
    void ChangeFocusWidget();

signals:
    void RunFormatter(QString code);
    void RunColorizer(QString code);
    void RunBoth(QString code);
    void startDebugging(const QString &code);
    void RunWithoutDebug(QString code, BFList InputSequence);
    void terminateRunning();
};

#endif // WIDGET_H
