#include "plot.h"

#include <QGridLayout>
#include <QWheelEvent>

#include <QDebug>

Plot::Plot(QWidget *parent) :
  QWidget(parent)
{
  scene = new CustomScene(this);
  view = new QGraphicsView(scene);

  this->setLayout(new QGridLayout(this));
  this->layout()->addWidget(view);

  setStyleSheet("background-color: black;");

  connect(scene, SIGNAL(rangeSelected(qreal,qreal)), this, SLOT(rangeSelected(qreal, qreal)));
}


void Plot::addNewItem(QGraphicsItem * i)
{
  scene->addItem(i);
}


void Plot::clear()
{
  scene->clear();
}


void Plot::rangeSelected(qreal init, qreal end)
{
  qDebug() << "Zooming range selected : " << init << " " << end;
  qDebug() << "Center point : " << (init + end) / 2;
  qDebug() << "View width : " << view->width();
  emit zoomChanged(init, end, view->width());
}


qreal Plot::updateSceneView(qreal center)
{
  qDebug() << "The center is : " << center;
  scene->setSceneRect(scene->itemsBoundingRect());

  qreal old_y = scene->itemsBoundingRect().y();
  view->centerOn(center, old_y);

  return scene->itemsBoundingRect().right();
}
