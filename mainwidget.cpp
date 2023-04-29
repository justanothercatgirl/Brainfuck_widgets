#include "mainwidget.h"
#include "qwidget.h"
MainWidget::MainWidget(QWidget *parent): QWidget{parent}
{
    qInfo() << "constructing" << this;

    for(int i = 0; i<4; i++){
        WidgetList.append(new QWidget(this));
        LayoutList.append(new QGridLayout(WidgetList.last()));
        WidgetList.last()->setObjectName("MainWidgetMode"+QString::number(i));
        LayoutList.last()->setObjectName("MainWidgetLayout"+QString::number(i));
    }
    qInfo() << WidgetList << LayoutList << "Generated";
    connect(this, SIGNAL(OPEN()), parent, SLOT(Open()));
    connect(this, SIGNAL(NEWFILE()), parent, SLOT(NewFile()));
    connect(this, SIGNAL(SETTINGS()), parent, SLOT(IDEPreferences()));
    connect(parent, SIGNAL(AddOpenedFile(QString)), this, SLOT(AddOpenedFile(QString)));
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->addWidget(WidgetList.at(0));
}
MainWidget::~MainWidget()
{
    qInfo() << "deconstructed" << this;
}


void MainWidget::InitAllPages()
{
    // //////////INITIALIZING HOME WIDGET////////// //
    QLabel *Welcome = new QLabel("Welcome to BF Editor!", this);
    setStyleSheet("QLabel {font: 69px; color: #1F1E33}");
    QWidget *FancyStuff = new QWidget(this);
    FancyStuff->setAutoFillBackground(true); FancyStuff->setPalette(QPalette(Qt::red, Qt::red));
    LayoutList.at(0) -> addWidget(Welcome, 2, 2, 3, 24);
    LayoutList.at(0) -> addWidget(FancyStuff, 6, 10, 8, 16);
    for(int i = 0; i<3; i++)
        ButtList.append(CreateButton(ButtNames.at(i), ButtSlots.at(i), LayoutList.at(0), 6+3*i, 2, 2, 6));


    // //////////INITIALIZING EDIT WIDGET////////// //
    QColumnView* Files = new QColumnView(this);
    QVBoxLayout* FileLayout = new QVBoxLayout(Files);
    Editor = new QTextEdit("...And your code goes here", WidgetList.at(1));
    FileLayout->addWidget(new QLabel("Here will\nrest the\nfiles you\nopen"));
    LayoutList.at(1) -> addWidget(Files, 0, 0, 15, 5);
    LayoutList.at(1) -> addWidget(Editor, 0, 6, 15, 22);



    // //////////INITIALIZING SETTINGSS WIDGET////////// //



    // //////////INITIALIZING HELP WIDGET////////// //



}

void MainWidget::toHomePage()
{
    qInfo() << "toHomePage";
    DeleteAllWidgets();
    layout->addWidget(WidgetList.at(0));
}
void MainWidget::toEditPage()
{
    qInfo() << "toEditPage";
    DeleteAllWidgets();
    layout->addWidget(WidgetList.at(1));
}
void MainWidget::updateEditPage()
{
    if(!FilesOpened.isEmpty()){
            UpdateFileButts();
            foreach(QPushButton* Butt, FileButts.values())
                Files->layout()->addWidget(Butt);
        }
        else{
            FileLayout->addWidget(new QLabel("Here will\nrest the\nfiles you\nopen"));
        }
}
void MainWidget::toSettingsPage()
{
    qInfo() << "toSettingsPage";
    DeleteAllWidgets();
    layout->addWidget(WidgetList.at(2));
}
void MainWidget::toHelpPage()
{
    qInfo() << "toHelpPage";
    DeleteAllWidgets();
    layout->addWidget(WidgetList.at(3));
}

void MainWidget::DeleteAllWidgets()
{
    /*foreach(QWidget* widget, layout->findChildren<QWidget*>(Qt::FindDirectChildrenOnly)) delete widget;*/
    delete layout;
    layout = new QGridLayout(this);
}

void MainWidget::Open()
{
    emit OPEN();
}
void MainWidget::NewFile()
{
    emit NEWFILE();
}
void MainWidget::Settings()
{
    emit SETTINGS();
}

void MainWidget::AddOpenedFile(QString FileName)
{
    qInfo() << "MainWidget" << this << "opening" << FileName;
    QFile* current = new QFile(FileName);
    QStringList strings;
    if(current->open(QIODevice::ReadOnly)){
        QTextStream stream(&*current);
        stream.seek(0);
        while(!stream.atEnd()) strings.append(stream.readLine());
    }
    else {
        qCritical() << "Cannot open file!";
        return;
    }
    FilesOpened.insert(FileName, strings);
    current->close();
    delete current;
    qInfo() << FilesOpened.keys();
}
void MainWidget::RemoveOpenedFile(QString FileName)
{
    if(FilesOpened.keys().contains(FileName)) FilesOpened.remove(FileName);
    return;
}
void MainWidget::FileButtPushed()
{
    QString Name = sender()->objectName();
    qInfo() << Name;
    Editor->setText(FilesOpened.value(Name).join("\n"));

}
void MainWidget::UpdateFileButts()
{
    foreach(QString x, FilesOpened.keys()){
        qsizetype index = x.lastIndexOf("/")+1;
        qInfo() << x.sliced(index, x.size()-index);
        FileButts[x] = CreateButton(x.sliced(index, x.size()-index), SLOT(FileButtPushed()));
        FileButts.last()->setObjectName(x);
    }
}

QPushButton *MainWidget::CreateButton(QString text, const char *slot)
{
    QPushButton *butt = new QPushButton(text, this);
    butt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    butt->setObjectName("MainWidget" + text);
    connect(butt, SIGNAL(clicked()), this, slot);
    return butt;
}
QPushButton *MainWidget::CreateButton(QString text, const char *slot, QGridLayout *layout, int x, int y, int xspan, int yspan)
{
    QPushButton *butt = CreateButton(text, slot);
    layout->addWidget(butt, x, y, xspan, yspan);
    return butt;
}
