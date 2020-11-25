#include "commandeditor.h"
#include "ui_commandeditor.h"
#include "qfile.h"
#include "qstandardpaths.h"

CommandEditor::CommandEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CommandEditor)
{
    ui->setupUi(this);
}

CommandEditor::~CommandEditor()
{
    delete ui;
}

void CommandEditor::on_pushButton_2_clicked()
{
    QString command = ui->lineEdit->text();
    QString path = ui->lineEdit_2->text();
    QFile::link(path, QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).append("/").append(command).append(".lnk"));
}
