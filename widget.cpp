#include "widget.h"

void Widget::InitSettings()
{
    if (!QDir::current().entryList(QDir::Filter::Dirs).contains("resources")){
        qDebug() << "Shit happened! no \"resources\" folder in application directory!";
        qDebug() << QDir::current();
        return;
    }
    QFile settingsJson("resources/settings.json");
    if(!settingsJson.open(QIODevice::ReadOnly)) return;
    QByteArray ReadData(settingsJson.readAll());

    QJsonParseError error;
    QJsonObject settings(QJsonDocument::fromJson(ReadData, &error).object());
    if(error.error){
        qCritical() << "Failed parsing" << error.errorString();
        settingsJson.close();
        return;
    }

    if(settings.contains("ZoomInValue") && settings.value("ZoomInValue").isDouble())
        ZoomInValue = settings.value("ZoomInValue").toInt(5);
    if(settings.contains("DebugActive") && settings.value("DebugActive").isBool())
        DebugActive = settings.value("DebugActive").toBool(false);
    if(settings.contains("CollapseShortCycles") && settings.value("collapseShortCycles").isBool())
        CollapseShortCycles = settings.value("CollapseShortCycles").toBool(true);
    if(settings.contains("SepLineOPB") && settings.value("SepLineOPB").isBool())
        SepLineOPB = settings.value("SepLineOPB").toBool(true);
    if(settings.contains("SepLineCLB") && settings.value("SepLineCLB").isBool())
        SepLineCLB = settings.value("SepLineCLB").toBool(true);
    if(settings.contains("FormattingEnabled") && settings.value("FormattingEnabled").isBool())
        FormattingEnabled = settings.value("FormattingEnabled").toBool(true);
    if(settings.contains("ColorizingEnabled") && settings.value("ColorizingEnabled").isBool())
        ColorizingEnabled = settings.value("ColorizingEnabled").toBool(true);
    if(settings.contains("MinimalSupIndex") && settings.value("MinimalSupIndex").isDouble())
        MinimalSupIndex = settings.value("MinimalSupIndex").toInt(3);
    if(settings.contains("DebugArrayCellNum") && settings.value("DebugArrayCellNum").isDouble())
        DebugArrayCellNum = settings.value("DebugArrayCellNum").toInt(10);
    if(settings.contains("AmountOfSpaces") && settings.value("AmountOfSpaces").isDouble())
        AmountOfSpaces = settings.value("AmountOfSpaces").toInt(4);
    if(settings.contains("AboutText") && settings.value("AboutText").isArray()){
        QJsonArray a = settings.value("AboutText").toArray();
        for(QJsonArray::ConstIterator string = a.constBegin(); string<a.constEnd(); ++string)
            about.append((*string).toString());
    }

    settingsJson.close();
}
void Widget::InitColors()
{
    if (!QDir::current().entryList(QDir::Filter::Dirs).contains("resources")){
        qDebug() << "Shit happened! no \"resources\" folder in application directory!";
        qDebug() << QDir::current();
        return;
    }

    QFile colorsJson("resources/colors.json");
    if(!colorsJson.open(QIODevice::ReadOnly)) return;
    QByteArray ReadData(colorsJson.readAll());
    QJsonParseError error;
    QJsonObject colors(QJsonDocument::fromJson(ReadData, &error).object());
    if(error.error){

        colorsJson.close();
        restoreDeafultColors();
        if(!colorsJson.open(QIODevice::ReadOnly)) return;

        ReadData = colorsJson.readAll();
        colors = QJsonDocument::fromJson(ReadData).object();
    }
    if(!(colors.contains("colors") && colors.value("colors").isArray())) return;
    QJsonArray colorArray(colors.value("colors").toArray());
    QJsonArray::ConstIterator i(colorArray.constBegin());
    for(; i<colorArray.constEnd(); ++i){
        QJsonArray map = (*i).toArray();
        ColorMap[map.at(0).toString()] = map.at(1).toString();
    }
    FormattingMap =
    {
        {'[', ColorMap.value("Format[]")},
        {']', ColorMap.value("Format[]")},
        {'+', ColorMap.value("Format+-")},
        {'-', ColorMap.value("Format+-")},
        {'.', ColorMap.value("Format.,")},
        {',', ColorMap.value("Format.,")},
        {'{', ColorMap.value("Format<>")},
        {'}', ColorMap.value("Format<>")},
        {' ', ""}
    };

    colorsJson.close();
}
void Widget::restoreDeafultColors()
{
    QFile colors("resources/colors.json");
    if(!colors.open(QIODevice::WriteOnly)) return;
    qDebug() << colors.readAll(); // // // //
    QTextStream s(&colors);
    s.seek(0);
    QString deafult = "{\n"
                      "    \"colors\": [\n"
                      "        [\"GeneralBackground\", \"#1F1E33\"],\n"
                      "        [\"GeneralText\",       \"#1F1E33\"],\n"
                      "        [\"ButtonBackground\",  \"#888888\"],\n"
                      "        [\"ButtonText\",        \"#1F1E33\"],\n"
                      "        [\"OutputBackground\",  \"#1F1E33\"],\n"
                      "        [\"OutputBorder\",      \"#FF0000\"],\n"
                      "        [\"OutputText\",        \"#FF0000\"],\n"
                      "        [\"EditorBackground\",  \"#BABABA\"],\n"
                      "        [\"EditorText\",        \"#888888\"],\n"
                      "        [\"Format[]\",          \"#00FF00\"],\n"
                      "        [\"Format<>\",          \"#0000FF\"],\n"
                      "        [\"Format.,\",          \"#00FF00\"],\n"
                      "        [\"Format+-\",          \"#FF0000\"]\n"
                      "    ]\n"
                      "}\n";
    s << deafult;
    colors.close();
}
void Widget::SetPageNames()
{
    QThread::currentThread()->setObjectName("main thread");
    //MOVE TO QStackedWidget WITH QStackedLayout
    //LATER, OF COURSE
    InitAllPages();
}
void Widget::ConnectFormatter()
{
    formatter = new Formatter(CollapseShortCycles, SepLineOPB, SepLineCLB, MinimalSupIndex, FormattingEnabled, ColorizingEnabled, AmountOfSpaces);
    formatter->setFormattingMap(FormattingMap);
    formatter->moveToThread(&FT);

    connect(this, &Widget::RunFormatter, formatter, &Formatter::FormatCode, q);
    connect(this, &Widget::RunColorizer, formatter, &Formatter::ColorizeCode, q);
    connect(this, &Widget::RunBoth, formatter, &Formatter::doBoth, q);
    connect(formatter, &Formatter::FinishedFormatting, this, &Widget::UpdateFormatted, q);
    connect(formatter, &Formatter::FinishedColorizing, this, &Widget::UpdateFormatted, q);
    connect(formatter, &Formatter::didBoth, this, &Widget::UpdateFormatted, q);
}
void Widget::ConnectDebugger()
{
    debugger = new BrainFuck::Debugger();
    debugger->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    OutputLabel = new QLabel("None", this);


    connect(debugger, &BrainFuck::Debugger::FinishedRunning, OutputLabel, &QLabel::setText);
    connect(this, &Widget::startDebugging, debugger, &QWidget::show, Qt::DirectConnection);
    connect(this, &Widget::startDebugging, debugger, &BrainFuck::Debugger::reset, Qt::DirectConnection);
    connect(this, &Widget::startDebugging, debugger, &BrainFuck::Debugger::setCode, Qt::DirectConnection);
    connect(this, &Widget::RunWithoutDebug, debugger->runner, &BrainFuck::Runner::RunWithoutDebug, q);
    connect(debugger->runner, &BrainFuck::Runner::FinishedWDRun, OutputLabel, &QLabel::setText, q);
    connect(debugger->runner, &BrainFuck::Runner::FinishedWDRun, &debugger->RT, &QThread::quit);
    connect(this, &Widget::terminateRunning, debugger->runner, &BrainFuck::Runner::setStop, Qt::DirectConnection);
    connect(this, &Widget::RunWithoutDebug, debugger->runner, &BrainFuck::Runner::resetStop, q);

}
void Widget::GeneralAppSettings()
{
    setObjectName("Main Application");
    setWindowTitle("BrainFuck Editor");
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setWindowIcon(QIcon("resources/icon.jpg"));
    setStyleSheet(QString
                  (
                      "Widget{background-color: %1;}"
                      "QLabel{background-color: %2;}"
                      "QPushButton{color: %3; background-color: %2; font: Bold; font-size: 20px}"
                      "QLineEdit{"
                      "color: %4; background-color: %5; font: italic monospace; font-size: 30px;"
                      "border: 2px solid %6;}"
                   ).arg(ColorMap.value("GeneralBackground")).arg(ColorMap.value("ButtonBackground"))
                    .arg(ColorMap.value("ButtonText")).arg(ColorMap.value("OutputText"))
                    .arg(ColorMap.value("OutputBackground")).arg(ColorMap.value("OutputBorder"))
                  );
    resize(1600, 900);
}
QString Widget::RemoveSup()
{
    //QScopedPointer<QTextEdit> temp(new QTextEdit);
    QTextEdit *temp = new QTextEdit;
    QString t1 = ColorMap.value("Format.,"), t2 = ColorMap.value("Format[]");
    QString t3 = ColorMap.value("Format<>"), t4 = ColorMap.value("Format+-");
    temp->setHtml(Editor->toHtml().remove(QRegularExpression(
        QString("<span style=\" font-weight:\\d+; color:(%1|%2|%3|%4); vertical-align:super;\">\\d*</span>")
        .arg(t1).arg(t2).arg(t3).arg(t4)
                                                                )));
    //FIX REGULAR EXPRESSION TO BE NOT TIED TO COLOR !!!!!!!!!!!!!!!//////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!?////////////////////////////////
    //temp->deleteLater();
    QString returnS = temp->toPlainText();
    delete temp;
    return returnS;
}
Widget::Widget(QWidget *parent): QWidget(parent)
{
    // I N I T I A L I Z I N G   L O C A L   V A R I A B L E S //
    CurrentFile = "";
    Files = new QColumnView(FocusWidget->widget(1));
    FileLayout = new QVBoxLayout(Files);
    ComingSoon->setStyleSheet(QString("color: %1; font: bold 72px;").arg(ColorMap.value("GeneralBackground")));

    // huge code chunks in functions above //
    InitSettings();
    InitColors();
    SetPageNames();
    ConnectFormatter();
    ConnectDebugger();
    GeneralAppSettings();

    // L A Y O U T //
    QList<QPushButton*> ConstButtList;
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->setObjectName("Main Application Layout");
    foreach(QString i, ConstButtNames)
    {
        ConstButtList.append(CreateButton(i, ConstButt.value(i)));
        ConstButtList.last()->setStyleSheet(QString("font: %1px; color: #1F1E33")
                                            .arg(QString::number(36)));
    }

    // C O N S T B U T T S   A N D   L A Y O U T //
    QWidget* UpButts = new QWidget(this);
    QWidget* LeftButts = new QWidget(this);
    QHBoxLayout* UpLayout = new QHBoxLayout(UpButts);
    QVBoxLayout* LeftLayout = new QVBoxLayout(LeftButts);
    for(int i = 0; i<6; i++)
    {
        ConstButtList.at(i)->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        UpLayout->addWidget(ConstButtList.at(i));
    }
    for(int i = 6; i<11; i++)
    {
        ConstButtList.at(i)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        if (ConstButtList.at(i)->text() == "Run")
        {
            delete ConstButtList.at(i);
            FileButton *temp = createButton("Run", 'r');
            temp->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            temp->setStyleSheet("font: 32px;");
            LeftLayout->addWidget(temp);
            continue;
        }
        LeftLayout->addWidget(ConstButtList.at(i));
    }
    QLabel* OutputSign = new QLabel("BF Output -> ", LeftButts);
    OutputSign->setStyleSheet("font: 36px; color: #1F1E33; background-color: #AAAAAA");
    OutputLabel->setStyleSheet(OutputSign->styleSheet());
    LeftLayout->addWidget(OutputSign);
    layout->addWidget(UpButts, 0, 0, 1, 32);
    layout->addWidget(LeftButts, 1, 0, 18, 4);
    layout->addWidget(OutputLabel, 16, 4, 2, 28);
    layout->setSizeConstraint(QLayout::SetNoConstraint);
    layout->addWidget(FocusWidget, 1, 4, 15, 28);

}
Widget::~Widget()
{
    qInfo() << this << "DECONSTRUCTED";
}

const QString Widget::GetRawCode(bool PreserveComments)
{
    QString code = RemoveSup();
    if(PreserveComments) for(int i = code.size()-1; i>0; i--)
    {
        if(code.at(i) == '/') i = code.lastIndexOf('/', i-1);
        else if (!RawRawCodeSymbs.contains(code.at(i))) code.remove(i, 1);
    }
    else
    {
        int j = 0;
        while (j>=0 && j<code.size()-1){
            j = code.indexOf('/');
            int k = code.indexOf('/', j+1);
            if(k<0) break;
            code.remove(j, k-j+1);
        }
        for (int i = code.size()-1; i>=0; i--) if (!RawRawCodeSymbs.contains(code.at(i))) code.remove(i, 1);
    }
    code.squeeze();
    qInfo() << code.capacity() << code;
    return code;
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_F1){
        emit terminateRunning();
    }
    else QWidget::keyPressEvent(e);
}
void Widget::IDEPreferences()
{
    qInfo() << "IDEPREFERENCES TRIGGERED";
    ComingSoon->show();
}
void Widget::IDEHelp()
{
    qInfo() << "IDEHELP TRIGGERED";
    ComingSoon->show();
}
void Widget::Open()
{
    QString FilePath = QFileDialog::getOpenFileName();
    QFile* current = new QFile(FilePath);
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
    FilesOpened.insert(FilePath, strings);
    current->close();
    delete current;
    updateEditPage();
    FocusWidget->setCurrentIndex(1); //Edit Page

}
void Widget::SaveChanges()
{
    FilesOpened[CurrentFile] = RemoveSup().split('\n');
}
void Widget::Close()
{
    QString File = sender()->objectName();
    if(FilesOpened.keys().contains(File)) FilesOpened.remove(File);
    updateEditPage();
}
void Widget::SaveAs(bool SaveRaw, bool SaveRawRaw)
{
    SaveChanges();
    QFile File(CurrentFile);
    if(File.open(QIODevice::WriteOnly)){
        QTextStream stream(&File);
        stream.seek(0);
        if (!SaveRaw) {
        QStringList CSL = FilesOpened.value(CurrentFile);
        for(int i = 0; i < CSL.size(); i++)
            stream << CSL.at(i) << Qt::endl;
        }
        else stream << GetRawCode(!SaveRawRaw);
    }
    else qCritical() << "Coud not save file!";
    File.close();
}
void Widget::NewFile()
{
    qInfo() << "NEWFILE TRIGGERED";
    ComingSoon->show();
    //QDialog something
}
void Widget::About()
{
    QMessageBox::about(this, "About", about.sliced(0, about.size()-3).join('\n'));
    QMessageBox::aboutQt(nullptr);
}

void Widget::InitAllPages()
{
    // //////////INITIALIZING HOME WIDGET////////// //
    QList<LayoutWidget> FocusWidgets(4);
    // 0 - Home, 1 - Edit, 2 - Settings, 3 - Help
    for(auto& [widget, layout]: FocusWidgets)
    {
        widget = new QWidget(this);
        layout = new QGridLayout(widget);
    }

    QLabel *Welcome = new QLabel("Welcome to BrainFork Code Editor!", this);
    Welcome->setStyleSheet(QString("font: 69px; color: %1").arg(ColorMap.value("GeneralText")));
    QTextEdit *FancyStuff = new QTextEdit(this);
    FancyStuff->setText(about.join('\n'));
    FancyStuff->setStyleSheet("background-color: gray; color: black; font: bold 28px");
    FancyStuff->setReadOnly(true);

    FocusWidgets[0].layout -> addWidget(Welcome, 2, 2, 3, 24);
    FocusWidgets[0].layout -> addWidget(FancyStuff, 6, 10, 8, 16);
    for(int i = 0; i<3; i++)
        ButtList.append(CreateButton( ButtNames.at(i), ButtSlots.at(i),
                                     FocusWidgets[0].layout, 6+3*i, 2, 2, 6 ));
    // //////////INITIALIZING EDIT WIDGET////////// //
    Editor = new QTextEdit("...And here goes your code", FocusWidgets[1].widget);
    DeafultLabel = new QLabel("Here will\r\nrest the\r\nfiles you\r\nopen...");
    Inputs = new QLineEdit(FocusWidgets[1].widget);
    Inputs->setToolTip("Input sequence\n"
                       "Enter numbers from 0 to 255 separated by coma with space\n"
                       "e.g. \'69, 42, 0, 72, 7\'. Invalid inputs will be skipped, deafult input is 0\n"
                       "Numbers not ranging from 0 to 255 will be reinterpreted as unsigned char\n"
                       "(taking last 8 bits, basically number%256)");
    Inputs->setText("Inputs sequence");
    Inputs->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    DeafultLabel->setStyleSheet("color: black; font: 36px; ");
    DeafultLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
    FileLayout->addWidget(DeafultLabel, 1);
    FocusWidgets[1].layout -> addWidget(Files, 0, 0, 15, 5);
    FocusWidgets[1].layout -> addWidget(Editor, 0, 5, 13, 22);
    FocusWidgets[1].layout -> addWidget(Inputs, 13, 5, 2, 22);
    Editor->zoomIn(ZoomInValue);
    Editor->setUndoRedoEnabled(true);
    Editor->setReadOnly(true);
    Editor->setStyleSheet(QString("background-color: %1; color: %2")
                          .arg(ColorMap.value("EditorBackground")).arg(ColorMap.value("EditorText")));
    Files ->setStyleSheet(QString("background-color: %1;").arg(ColorMap.value("ButtonBackground")));
    QFont font = Editor->font();
    font.setFamily("Courier New");
    Editor->setFont(font);
    // //////////INITIALIZING SETTINGSS WIDGET////////// //
    FocusWidgets[2].widget = CS;
    FocusWidgets[2].widget -> setStyleSheet("font-size: 44px");
    // //////////INITIALIZING HELP WIDGET////////// //
    FocusWidgets[3].widget = CS;
    FocusWidgets[3].widget -> setStyleSheet("font-size: 44px");

    foreach(const LayoutWidget &LW, FocusWidgets)
        FocusWidget -> addWidget(LW.widget);
    //deez nuts
}
void Widget::updateEditPage()
{
    foreach(auto ch, FileLayout->findChildren<QPushButton*>())
        ch->hide();
    delete FileLayout;
    QThread::currentThread()->msleep(100);
    if (FilesOpened.empty()){
        DeafultLabel->show();
        Editor->setReadOnly(true);
        return;
    }
    Editor->setReadOnly(false);
    DeafultLabel->hide();
    FileLayout = new QVBoxLayout(Files);
    FileLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
    if(!FilesOpened.empty()){
        foreach(QString name, FilesOpened.keys()){
            qsizetype x = name.lastIndexOf('/')+1;
            FileButton *butt = createButton(name.sliced(x, name.size()-x), 'f');
            butt->setObjectName(name);
            FileLayout->addWidget(butt, 1);
        }
    }
}

void Widget::DebugTriggered()
{
    qInfo() << "debugTriggered";
    BFList IS; QStringList OLL = Inputs->text().split(", ");
    foreach(QString i, OLL){
        bool ok;
        unsigned char ch = static_cast<unsigned char>(i.toInt(&ok));
        if(ok) IS.append(ch);
    }

    using namespace BrainFuck;
    if(!DebugActive){
        debugger->RT.start();
        emit RunWithoutDebug(GetRawCode(false), IS);
        return;
    }
    if (FocusWidget->currentIndex() == 1) debugger->show();
    emit startDebugging(GetRawCode(true));

}

FileButton  *Widget::createButton(QString text, char type)
{
    FileButton *b;
    switch(type){
    case 'r':
    {
        b = new FileButton(RUNBUTTONMAP, text, this);
        connect(b, &QPushButton::clicked, this, &Widget::DebugTriggered);
        break;
    }
    case 'f':
    {
        b = new FileButton(FILEBUTTONMAP, text, this);
        connect(b, &QPushButton::clicked, this, &Widget::FileButtPushed);
        break;
    }
    } //switch(type)
    return b;
}
QPushButton *Widget::CreateButton(QString text, const char *slot, QGridLayout *layout, int x, int y, int xspan, int yspan)
{
    QPushButton *butt = CreateButton(text, slot);
    layout->addWidget(butt, x, y, xspan, yspan);
    butt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    return butt;
}
QPushButton *Widget::CreateButton(QString text, const char *slot)
{
    QPushButton* button(new QPushButton(this));
    connect(button, SIGNAL(released()), this, slot);
    button->setText(text);
    button->setObjectName(text);
    button->setAutoFillBackground(true);
    //Vika's colors: QColor(124, 124, 124), QColor(26, 26, 26)
    return button;
}

void Widget::UpdateFormatted(QString code)
{
    Editor->setHtml(code);
    FT.quit();
}
void Widget::ChangeRunMode()
{
    DebugActive = !DebugActive;
    qInfo() << DebugActive;
}
void Widget::ChangeFocusWidget()
{
    int index = ButtonsToFocusWidget[sender()->objectName()];
    FocusWidget->setCurrentIndex(index);

}
void Widget::FileButtPushed()
{
    SaveChanges();
    CurrentFile = sender()->objectName();
    QString Text = FilesOpened.value(CurrentFile).join("\r\n");
    FT.start();
    emit RunFormatter(Text);
    emit RunColorizer(Text);
}
