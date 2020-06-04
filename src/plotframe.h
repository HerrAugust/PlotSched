#ifndef PLOTFRAME_H
#define PLOTFRAME_H

#include <QGraphicsItemGroup>
#include <QList>
#include <QVector>
#include <QString>
#include <QGraphicsSimpleTextItem>

/**
 * @brief The PlotFrame class
 * If I understood it right, a
 * QGraphicalView is a widget that serves as a container
 * of a QGraphicalScene (see class CustomScene), which
 * in turn contains many QGraphicalItems (see class EventView),
 * i.e. rectangles, arrows, texts and so on.
 *
 * A PlotFrame is ???
 */
class PlotFrame : public QGraphicsItemGroup
{
  qreal vertical_offset, horizontal_offset;

  /// set of tasks in this plotframe
  QList<QGraphicsSimpleTextItem *> callers;

  /// set of lines in this plotframe
  QVector<QGraphicsLineItem *> lines;

public:
  PlotFrame(qreal offset = 50, QGraphicsItem * parent = 0);

  /// add a row to the plot: t0 |____________
  void addRow(const QString &title);

  /// set lines width
  void setLinesWidth(qreal width);

  /// set lines height
  void setCPUHeight(qreal height);

  /// add a CPU to the plot (for the CPU view)
  void addCPU(const QString &cpu_name);
};

#endif // PLOTFRAME_H
