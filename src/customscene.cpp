#include "customscene.h"
#include "eventsmanager.h"
#include "eventview.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QDebug>

CustomScene::CustomScene(QObject *parent) :
  QGraphicsScene(parent)
{
  pressed = false;
  range = 0;
}


void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
  //qDebug() << "Mouse pressed at " << mouseEvent->lastScenePos();

  switch (mouseEvent->button()) {
    case Qt::LeftButton :
      qDebug() << "Mouse pressed at : " << mouseEvent->lastScenePos();
//      qDebug() << "Scene width : " << this->width();
      break;
    case Qt::RightButton :
      pressed = true;
      pressed_at = mouseEvent->lastScenePos();

      range = new RangeSelector;
      range->setStartPoint(pressed_at);
      range->setEndPoint(pressed_at);
      this->addItem(range);
      range->setVisible();
      break;
    default : break;
  }

  QGraphicsScene::mousePressEvent(mouseEvent);
}


void CustomScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
  //qDebug() << "Mouse released at " << mouseEvent->lastScenePos();

  switch (mouseEvent->button()) {
    case Qt::LeftButton :
      break;
    case Qt::RightButton :
      this->removeItem(range);
      delete range;
      range = 0;
      pressed = false;

      if (pressed_at.x() != mouseEvent->lastScenePos().x())
        emit rangeSelected(pressed_at.x(), mouseEvent->lastScenePos().x());
      break;
    default : break;
  }

  QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if (!pressed)
    return;

  range->setEndPoint(mouseEvent->lastScenePos());
  QGraphicsScene::mouseMoveEvent(mouseEvent);
  //qDebug() << "Moving mouse to " << mouseEvent->lastScenePos();
}

void CustomScene::keyPressEvent(QKeyEvent *keyEvent)
{
    QGraphicsView *view = views().first();
    switch (keyEvent->key()) {
      case Qt::Key_Home:
        qDebug() << "Keyboard pressed: " << keyEvent->text();
        views().at(0)->centerOn(0, view->y());
        break;
    case Qt::Key_End:
      qDebug() << "Keyboard pressed: " << keyEvent->text();
      view->centerOn(_lastEventView);
      break;
      default : break;
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}
