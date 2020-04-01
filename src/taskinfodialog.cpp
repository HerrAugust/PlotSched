#include "taskinfodialog.h"
#include "ui_taskinfodialog.h"

#include <QDir>
#include <cstdlib>
#include <QLabel>
#include <QDebug>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

TaskInfoDialog::TaskInfoDialog(Node *node, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::TaskInfoDialog)
{
    ui->setupUi(this);

    _node = node;
    setIsGraph(false);
}

void TaskInfoDialog::setInfo(QString info)
{
    ui->label->setText(info);
}

void TaskInfoDialog::setGraphInfo(QString info)
{
    ui->labelGraphInfo->setText(info);
}

void TaskInfoDialog::setIsGraph(bool isIt)
{
    ui->groupBoxGraphInfo->setVisible(isIt);
    if(isIt) {
        ui->groupBoxGraphInfo->setTitle(QString("Graph info. Cur node = ") + QString::number(_node->getDotFileIndex()));

        showGraph();
    }
}

void TaskInfoDialog::showGraph()
{
    QStringList nameFilter("*.gv");
    QDir rootDir(_node->getDAG()->getRootFolder());
    QString dotfile = rootDir.entryList(nameFilter).at(0);
    std::string cmd = "xdot " + (_node->getDAG()->getRootFolder().append('/') + dotfile + " &").toStdString();
    if (std::system(cmd.c_str()) < 0) { qDebug() << "system gave error in " << __func__; }
}

void TaskInfoDialog::on_pushButtonGraphInfo_clicked()
{
    showGraph();
}

TaskInfoDialog::~TaskInfoDialog()
{
    delete ui;
}
