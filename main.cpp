#include "widget.h"
#include <QApplication>

/*
    TODO's:
        settings.json -> done

        QStackedWidget -> done
            move HomeWidgets to QStackedWidget

        close files properly
            why wouldn't it just work???

        context menu
            try to find an appropriate QWidget for this

        Icon, redesign and colors
        {
            ?function to convert integers to hexadecimal colours
            Pre-set themes
        }

        DESIGN MACRO SYSTEM FOR BF
            I can't even begin to imagine how would this work bro

        Debugger.cpp
        {
            Run the code in real-time
            Communication with Runner
        }

        Compile BF
        {
            Make use of existing compiler (and leave soutouts)
            error handling (somehow...)
            Optimization (methods from wikipedia, maybe)
        }

        QSyntaxHighlight
            instead of crazy html-colouring, will probably result in better performance

        STATIC BUILD
            yeah that's obvious

*/
int main(int argc, char **argv)
{
    QApplication a(argc,argv);
    Widget w; w.show();
    return a.exec();
}
