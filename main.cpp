#include "quickcommondwin.h"
#include "logmanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);
    QuickCommondWin w;
    w.show();
    return a.exec();
}
