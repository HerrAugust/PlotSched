#ifndef CUSTOMTOOLBAR_H
#define CUSTOMTOOLBAR_H

#include <QObject>
#include <QToolBar>

class QToolButton;
class MainWindow;

class CustomToolBar : public QToolBar
{
  Q_OBJECT

private:
  QToolButton* buttonChangeTasksView, *buttonChangeCPUView, *buttonChangeGanntView;

  MainWindow *_mainWindow;

  void dehighlight(QToolButton *button);
  void highlight  (QToolButton *button);

  // highlight only one button, decoloring the other ones
  void highlightOnly(QToolButton *button);
public:
  CustomToolBar(MainWindow *parent = 0);

public slots:
  void buttonOpenSlot();
  void buttonRefreshSlot();

  void buttonZoomInSlot();
  void buttonZoomOutSlot();
  void buttonZoomFitSlot();
  void buttonChangeViewGanntSlot();
  void buttonChangeViewCPUSlot();
  void buttonChangeViewTasksSlot();

signals:
  void openButtonClicked();
  void refreshButtonClicked();

  void zoomInClicked();
  void zoomOutClicked();
  void zoomFitClicked();

  void changeViewTasksClicked();
  void changeViewGanntClicked();
  void changeViewCPUClicked();
};

#endif // CUSTOMTOOLBAR_H
