#ifndef TASKINFODIALOG_H
#define TASKINFODIALOG_H

#include "dag.h"

#include <QDialog>

namespace Ui
{
class TaskInfoDialog;
}

class TaskInfoDialog : public QDialog
{
    Q_OBJECT

protected:
    /// The DAG node this panel refers to. Optional field
    Node *_node = NULL;

public:
    explicit TaskInfoDialog(Node *dag = 0, QWidget *parent = 0);
    ~TaskInfoDialog();

    void setInfo(QString info);

    void setGraphInfo(QString info);

    void setIsGraph(bool isIt);

    void showGraphImg();

private slots:
    void on_pushButtonGraphInfo_clicked();

private:
    Ui::TaskInfoDialog *ui;
};

#endif // TASKINFODIALOG_H
