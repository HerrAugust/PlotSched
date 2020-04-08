#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H

#include <QGraphicsScene>

#include <QKeyEvent>
#include "rangeselector.h"

class EventView;

class CustomScene : public QGraphicsScene
{
  Q_OBJECT

  RangeSelector * range;

  bool pressed;
  QPointF pressed_at;

  /// last event shown on the scene
  EventView* _lastEventView;

public:
  explicit CustomScene(QObject *parent = 0);

  inline void setLastEventView(EventView* const e) { _lastEventView = e; }

signals:
  void rangeSelected(qreal, qreal);

public slots:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

  virtual void keyPressEvent(QKeyEvent *event);

};

#endif // CUSTOMSCENE_H
