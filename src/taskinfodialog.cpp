#include "taskinfodialog.h"
#include "ui_taskinfodialog.h"
#include "utils.h"

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
    ui->labelGraphImg->setVisible(false);

    _node = node;
    setIsGraph(false);
}

void TaskInfoDialog::setInfo(QString info)
{
    ui->labelGeneralInfo->setText(info);
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

        showGraphImg();
    }
}

void TaskInfoDialog::showGraphImg()
{
    QString filename = "";
    auto res = searchFileInAllSubdirs("graph.png", _node->getDAG()->getRootFolder());
    if (res.size() == 0)
        return;
    filename = res.at(0);
    qDebug() << "found graph.png at " << filename;

    /** set content to show center in label */
    ui->labelGraphImg->setVisible(true);
    ui->labelGraphImg->setAlignment(Qt::AlignCenter);
    QPixmap pix;
    if(pix.load(filename)){
        /** scale pixmap to fit in label'size and keep ratio of pixmap */
        pix = pix.scaled(ui->labelGraphImg->size(),Qt::KeepAspectRatio);
        ui->labelGraphImg->setPixmap(pix);
    }
    else {
        qDebug() << "Error showing " << filename;
    }
}

void TaskInfoDialog::on_pushButtonGraphInfo_clicked()
{
    QStringList nameFilter("*.gv");
    QDir rootDir(_node->getDAG()->getRootFolder());
    QString dotfile = rootDir.entryList(nameFilter).at(0);
    std::string cmd = "xdot " + (_node->getDAG()->getRootFolder().append('/') + dotfile + " &").toStdString();
    if (std::system(cmd.c_str()) < 0) { qDebug() << "system gave error in " << __func__; }
}

TaskInfoDialog::~TaskInfoDialog()
{
    delete ui;
}
