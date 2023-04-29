#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>
#include <QMap>
#include <QFile>
#include <QColumnView>

//TOMORROW MOVE WHOLE CLASS TO WIDGET CLASS
//THIS CLASS WAS A MISTAKE FROM THE VERY BEGINNING
//YES, IT WILL TAKE ME BACK A LOT, BUT WHO CARES
//I HAVE TO MOVE ON FORWARD

//though brainfuck debuggers just has to be a different class

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    QMap<QString, QStringList> FilesOpened;


public slots:
    void InitAllPages();
    void toHomePage();
    void toEditPage();
    void updateEditPage();

    void toSettingsPage();
    void toHelpPage();

    void DeleteAllWidgets();

    void Open();
    void NewFile();
    void Settings();
    void AddOpenedFile(QString FileName);
    void RemoveOpenedFile(QString FilaName);
    void FileButtPushed();
    void UpdateFileButts();


    QPushButton *CreateButton(QString text, const char* slot, QGridLayout* layout, int x = 0, int y = 0, int xspan = 0, int yspan = 0);
    QPushButton* CreateButton(QString text, const char* slot);


signals:
    void OPEN();
    void NEWFILE();
    void SETTINGS();


private:
    ///H O M E P A G E///
    QStringList ButtNames = {"Open", "Create New", "Settings"};
    QList<const char*> ButtSlots = {SLOT(Open()), SLOT(NewFile()), SLOT(Settings())};
    QList<QPushButton*> ButtList;

    ///E D I T P A G E///
    QList<QWidget*> WidgetList;
    QList<QGridLayout*> LayoutList;
    QColumnView* Files = new QColumnView; //set Parent EditWidget
    QVBoxLayout* FileLayout = new QVBoxLayout(Files);
    QTextEdit* Editor; //set parent EditWidget
    QMap<QString, QPushButton*> FileButts;

    ///O T H E R///
    QGridLayout* layout = new QGridLayout(this);



};
#endif // MAINWIDGET_H
