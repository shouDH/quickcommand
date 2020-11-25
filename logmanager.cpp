#include <QtMessageHandler>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QTextCodec>


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray localMsg = codec->fromUnicode(msg);
    QString text;

    QFile file("log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;

    QTextStream gOutStream(&file);
    switch (type)
    {
    case QtDebugMsg:
        text = QString("Debug >> %4(%2:%3) : %1)\r\n").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtInfoMsg:
        text = QString("Info >> %4(%2:%3) : %1\r\n").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtWarningMsg:
        text = QString("Warning >> %4(%2:%3) : %1\r\n").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtCriticalMsg:
        text = QString("Critical >> %4(%2:%3) : %1\r\n").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtFatalMsg:
        text = QString("Fatal >> %4(%2:%3) : %1\r\n").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
        abort();
    default:
        text = QString("Default >> %4(%2:%3) : %1\r\n").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
    }

    gOutStream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss ") + " >> " + text; //输出到txt文件
    gOutStream.flush(); //刷新缓冲区
}
