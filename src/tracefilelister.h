#ifndef TRACEFILELISTER_H
#define TRACEFILELISTER_H

#include <QDockWidget>
#include <QTreeWidget>

class MainWindow;

class TraceFileLister : public QDockWidget
{
  Q_OBJECT

  QTreeWidget * tree;
  MainWindow *_mainWindow;

  void addFolderToTree(QTreeWidgetItem * parent, QString dir);

private slots:
  void traceSelected(QTreeWidgetItem * i, int col);

public:
  explicit TraceFileLister(MainWindow *parent);
  ~TraceFileLister();

  void update(QString);

};

#endif // TRACEFILELISTER_H
