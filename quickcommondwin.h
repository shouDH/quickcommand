#ifndef QUICKCOMMONDWIN_H
#define QUICKCOMMONDWIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class QuickCommondWin; }
QT_END_NAMESPACE

class QuickCommondWin : public QMainWindow
{
    Q_OBJECT 
    QString cmdPath;
    QStringList cmdNameList;

public:
    QuickCommondWin(QWidget *parent = nullptr);
    ~QuickCommondWin();

private slots:
    void on_pushButton_clicked();
    void on_toolButton_clicked();
    void on_delete_2_triggered();
    void updateCmdTable();
    int addEnvPath(QString envValue);

private:
    Ui::QuickCommondWin *ui;
};
#endif // QUICKCOMMONDWIN_H
