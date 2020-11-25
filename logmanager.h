#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QtMessageHandler>
#include <QString>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // LOGMANAGER_H
