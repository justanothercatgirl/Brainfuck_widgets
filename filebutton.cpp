#include "filebutton.h"

FileButton::FileButton(QMap<QString, const char *> map, QString text, QWidget *parent)
: QPushButton(text, parent)
{
    ButtTexts = map.keys();
    ButtSlots = map.values();
    Options = new QColumnView();
    OptionsLayout = new QVBoxLayout(Options);

    foreach(auto i, ButtTexts){
        QPushButton *b = new QPushButton(i, Options);
        connect(b, SIGNAL(clicked()), parent, map.value(i));
        connect(b, &QPushButton::clicked, Options, &QWidget::hide);
        b->setObjectName(this->objectName());
        ButtList << b;
        OptionsLayout->addWidget(b, 1, Qt::AlignTop);
    }
    connect(this, &FileButton::RightButtonClicked, this, &FileButton::ShowOptions, Qt::DirectConnection);
    connect(this, &QWidget::objectNameChanged, this, &FileButton::ChON, Qt::DirectConnection);
}

void FileButton::mousePressEvent(QMouseEvent *e)
{
    if(!hitButton(e->pos())){
        e->ignore();
        return;
    }
    setDown(true);
    repaint();

    if (e->button() == Qt::LeftButton) {
        emit pressed();
    }
    else if (e->button() == Qt::RightButton){
        emit RightButtonClicked();
    }

}

void FileButton::ShowOptions()
{
    if (Options->isVisible()) Options->hide();
    else Options->show();
}
void FileButton::ChON(QString s)
{
    foreach(auto i, ButtList)
        i->setObjectName(s);
}
