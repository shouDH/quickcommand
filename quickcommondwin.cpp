#include "quickcommondwin.h"
#include "ui_quickcommondwin.h"

#include <QFile>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDebug>

#include <Windows.h>

QuickCommondWin::QuickCommondWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QuickCommondWin)
{
    ui->setupUi(this);

    //read config
    qInfo()<<("开始读取配置文件----------------------------------");
    QSettings* configIni = new QSettings("quickcommand.ini", QSettings::IniFormat);
    QString CMD_HOME_ = configIni->value("PATH/QUICKCMDHOME").toString();
    QString CMD_DIR_ = configIni->value("PATH/QUICKCMDDIR").toString();
    delete configIni;
    qInfo()<<("CMD_HOME_:" + CMD_HOME_);
    qInfo()<<("CMD_DIR_:" + CMD_DIR_);

    this->cmdPath = CMD_HOME_.append("/").append(CMD_DIR_);
    qInfo()<<("cmdPath:" + this->cmdPath);
    qInfo()<<("读取配置文件结束----------------------------------");

    //check path
    qInfo()<<("开始配置注册表信息----------------------------------");
    bool needInit = true;
    QSettings regedit("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString pathStr = regedit.value("Path", "error").toString();
    QStringList pathList = pathStr.split(";");
    for(int i = 0; i < pathList.size(); ++i){
        QString path = pathList.at(i);
        QString temp = this->cmdPath;
        if(path == temp.replace("/", "\\")){
            qInfo()<<("注册表信息已存在，无需配置----------------------------------");
            qInfo()<<("环境变量配置为" + path);
            needInit = false;
            break;
        }
    }

    //init path
    if(needInit){
        //create dir
        QDir cmdDir;
        bool f = true;
        if(!cmdDir.exists(this->cmdPath)){
            qInfo()<<("注册表对应目录不存在，开始新建目录：" + this->cmdPath + "----------------------------------");
            f = cmdDir.mkdir(this->cmdPath);
        }

        if(f){
            qInfo()<<("新建目录成功：" + this->cmdPath + "----------------------------------");
            QString path = this->cmdPath;
            int hr = this->addEnvPath(path.replace("/", "\\").toStdString().c_str());
            if(hr != 0){
                qInfo()<<("注册表配置失败----------------------------------");
            }
        }else{
            qInfo()<<("新建目录失败：" + this->cmdPath + "----------------------------------");
        }
    }
    qInfo()<<("结束配置注册表信息----------------------------------");

    //create cmd table
    ui->tableWidget->setColumnCount(2);
    QStringList tableHeader;
    tableHeader << QString("命令名称") << QString("命令程序");
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget->setColumnWidth(1, 350);

    this->updateCmdTable();
}

int QuickCommondWin::addEnvPath(QString envValue) {
    QSettings regedit("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString regValue = regedit.value("Path", "").toString();
    regedit.setValue("Path", regValue.append(";").append(envValue));

    SendMessage(HWND_BROADCAST,WM_SETTINGCHANGE,0,(LPARAM)TEXT("Environment"));

    return 0;
}

QuickCommondWin::~QuickCommondWin()
{
    delete ui;
}

void QuickCommondWin::on_pushButton_clicked()
{
    QString cmdName = ui->lineEdit->text();
    QString cmdLink = ui->lineEdit_2->text();

    if(cmdName.isEmpty()){
        QMessageBox::critical(0, "错误", "请指定快速命令名称！", QMessageBox::Ok);
        return;
    }
    if(cmdLink.isEmpty()){
        QMessageBox::critical(0, "错误", "请指定快速命令应用！", QMessageBox::Ok);
        return;
    }

    if(this->cmdNameList.contains(cmdName)){
        QMessageBox::critical(0, "警告", "指定快速命令名称重复！", QMessageBox::Ok);
        return;
    }

    qInfo()<<("创建命令快捷方式----------------------------------");
    //create cmd
    QString cmdPath = this->cmdPath;
    QString path = cmdPath.append("\\").append(cmdName).append(".lnk");

    qInfo()<<("cmdName:" + cmdName);
    qInfo()<<("cmdLink:" + cmdLink);
    qInfo()<<("path:" + path);
    qInfo()<<("cmdPath:" + cmdPath);
    bool f = QFile::link(cmdLink, path);

    //update cmd table
    if(f){
        qInfo()<<("创建命令快捷方式成功----------------------------------");
        this->cmdNameList.append(cmdName);
        int index = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(index);
        ui->tableWidget->setItem(index, 0, new QTableWidgetItem(cmdName));
        ui->tableWidget->setItem(index, 1, new QTableWidgetItem(cmdLink));
    }else{
        qInfo()<<("创建命令快捷方式失败----------------------------------");
        QMessageBox::critical(0, "错误", "新增命令失败！", QMessageBox::Ok);
        return;
    }
}

void QuickCommondWin::on_toolButton_clicked()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("打开文件");
    QString filename = fileDialog->getOpenFileName(0, "打开文件", "/");
    ui->lineEdit_2->setText(filename);

    delete fileDialog;
}
void QuickCommondWin::on_delete_2_triggered()
{
    qInfo()<<("删除命令----------------------------------");
    int row = ui->tableWidget->currentRow();
    QTableWidgetItem* cmdNameItem = ui->tableWidget->item(row, 0);
    int index = this->cmdNameList.indexOf(cmdNameItem->text());

    QString cmdPath = this->cmdPath;
    QString path = cmdPath.append("\\").append(cmdNameItem->text()).append(".lnk");
    QFileInfo FileInfo(path);
    if(FileInfo.isFile() && FileInfo.exists()){
        bool f = QFile::remove(path);
        if(f){
            ui->tableWidget->removeRow(row);
            this->cmdNameList.removeAt(index);

            delete cmdNameItem;
            qInfo()<<("删除命令成功----------------------------------");
        }else{
            qInfo()<<("删除命令失败----------------------------------");
        }
    }else{
        qInfo()<<("命令快捷方式不存在----------------------------------");
    }
}
void QuickCommondWin::updateCmdTable()
{
    QDir cmdDir;
    cmdDir.cd(this->cmdPath);
    QFileInfoList cmdList = cmdDir.entryInfoList();

    int index = 0;
    for(int i = 0; i < cmdList.size(); ++i){
        QFileInfo cmd = cmdList.at(i);
        if(cmd.isSymLink()){
            ui->tableWidget->insertRow(index);
            this->cmdNameList.append(cmd.fileName().remove(".lnk"));
            ui->tableWidget->setItem(index, 0, new QTableWidgetItem(cmd.fileName().remove(".lnk")));
            ui->tableWidget->setItem(index, 1, new QTableWidgetItem(cmd.readLink()));

            index ++;
        }
    }
}
