#include "customtoolbar.h"

#include <QToolButton>

CustomToolBar::CustomToolBar(QWidget * parent) :
  QToolBar(parent)
{
  QToolButton * buttonOpen = new QToolButton(this);
  buttonOpen->setIcon(QIcon(":/icons/assets/folder64x.png"));
  this->addWidget(buttonOpen);

  connect(buttonOpen, SIGNAL(clicked()), this, SLOT(buttonOpenSlot()));


  QToolButton * buttonRefresh = new QToolButton(this);
  buttonRefresh->setIcon(QIcon(":/icons/assets/refresh64x.png"));
  this->addWidget(buttonRefresh);

  connect(buttonRefresh, SIGNAL(clicked()), this, SLOT(buttonRefreshSlot()));


  this->addSeparator();

//  QToolButton * buttonZoomFit = new QToolButton(this);
//  buttonZoomFit->setIcon(QIcon::fromTheme("zoom-fit-best"));
//  this->addWidget(buttonZoomFit);
//  connect(buttonZoomFit, SIGNAL(clicked()), this, SLOT(buttonZoomFitSlot()));

  buttonChangeTasksView = new QToolButton(this);
  buttonChangeTasksView->setIcon(QIcon(":/icons/assets/tau64x.png"));
  buttonChangeTasksView->setToolTip("Show tasks");
  buttonChangeTasksView->setDisabled(true);
  this->addWidget(buttonChangeTasksView);
  connect(buttonChangeTasksView, SIGNAL(clicked()), this, SLOT(buttonChangeViewTasksSlot()));

  buttonChangeGanntView = new QToolButton(this);
  buttonChangeGanntView->setIcon(QIcon(":/icons/assets/gannt64x.png"));
  buttonChangeGanntView->setToolTip("Gannt diagram");
  this->addWidget(buttonChangeGanntView);
  connect(buttonChangeGanntView, SIGNAL(clicked()), this, SLOT(buttonChangeViewGanntSlot()));

  buttonChangeCPUView = new QToolButton(this);
  buttonChangeCPUView->setIcon(QIcon(":/icons/assets/cpu64x.png"));
  buttonChangeCPUView->setToolTip("Show cores load");
  buttonChangeCPUView->setDisabled(true);
  this->addWidget(buttonChangeCPUView);
  connect(buttonChangeCPUView, SIGNAL(clicked()), this, SLOT(buttonChangeViewCPUSlot()));

  this->addSeparator();

  QToolButton * buttonZoomIn = new QToolButton(this);
  buttonZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
  buttonZoomIn->setToolTip("Zoom in");
  this->addWidget(buttonZoomIn);
  connect(buttonZoomIn, SIGNAL(clicked()), this, SLOT(buttonZoomInSlot()));

  QToolButton * buttonZoomOut = new QToolButton(this);
  buttonZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
  buttonZoomOut->setToolTip("Zoom out");
  this->addWidget(buttonZoomOut);
  connect(buttonZoomOut, SIGNAL(clicked()), this, SLOT(buttonZoomOutSlot()));


  highlightOnly(buttonChangeGanntView);
}

void CustomToolBar::buttonOpenSlot()
{
  emit openButtonClicked();
}

void CustomToolBar::buttonRefreshSlot()
{
  emit refreshButtonClicked();
}

void CustomToolBar::buttonZoomInSlot()
{
  emit zoomInClicked();
}

void CustomToolBar::buttonZoomOutSlot()
{
  emit zoomOutClicked();
}

void CustomToolBar::buttonZoomFitSlot()
{
  emit zoomFitClicked();
}

void CustomToolBar::buttonChangeViewGanntSlot()
{
  highlightOnly(buttonChangeGanntView);
  emit changeViewGanntClicked();
}

void CustomToolBar::buttonChangeViewCPUSlot()
{
    highlightOnly(buttonChangeCPUView);
    emit changeViewCPUClicked();
}

void CustomToolBar::buttonChangeViewTasksSlot()
{
    highlightOnly(buttonChangeTasksView);
    emit changeViewTasksClicked();
}

void CustomToolBar::highlightOnly(QToolButton* button) {
    dehighlight(buttonChangeTasksView);
    dehighlight(buttonChangeCPUView);
    dehighlight(buttonChangeGanntView);

    highlight(button);
}

void CustomToolBar::highlight(QToolButton* button) {
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, QColor(Qt::green));
    button->setAutoFillBackground(true);
    button->setPalette(pal);
    button->update();
}

void CustomToolBar::dehighlight(QToolButton* button) {
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, QColor(Qt::transparent));
    button->setAutoFillBackground(true);
    button->setPalette(pal);
    button->update();
}
