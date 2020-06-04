#include "plotframe.h"

#include <QPen>
#include <QFont>

PlotFrame::PlotFrame(qreal offset, QGraphicsItem *parent) :
  QGraphicsItemGroup(parent)
{
  vertical_offset = offset;
  horizontal_offset = offset * 2;
}

/// adds a row to the plot
/// t0 |___________
void PlotFrame::addRow(const QString &title)
{
    unsigned int count = callers.count();

    qreal y = count * vertical_offset;

    QGraphicsSimpleTextItem * t = new QGraphicsSimpleTextItem(title, this);
    t->setPos(0, y - t->boundingRect().height());
    t->setPen(QPen(Qt::white));
    callers.append(t);
    this->addToGroup(t);

    // the horizontal line (indicating advancing time) of: t0 |______________
    QGraphicsLineItem * l = new QGraphicsLineItem(20, y, 20, y, this);
    l->setPen(QPen(Qt::white));
    lines.append(l);
    this->addToGroup(l);
}

/// |      |
/// |      |
///  ______
/// | BIG0 |
///  ------
/// adds a CPU representation to the plot. Use either addCPU() or addRow().
void PlotFrame::addCPU(const QString &cpuName) {
    unsigned int count = callers.count();

    qreal x = count * horizontal_offset;

    // CPU name (label)
    QGraphicsSimpleTextItem * t = new QGraphicsSimpleTextItem(cpuName, this);
    t->setPos(x, -t->boundingRect().height());
    t->setPen(QPen(Qt::white));
    t->setFont(QFont("Sans", 18));
    callers.append(t);
    this->addToGroup(t);

    // left vertical line
    QGraphicsLineItem * lleft = new QGraphicsLineItem(x - 20, 0, x - 20, 0, this);
    lleft->setPen(QPen(Qt::white));
    lines.append(lleft);
    this->addToGroup(lleft);

    // right vertical line
    QGraphicsLineItem * lright = new QGraphicsLineItem(x + 50, 0, x + 50, 0, this);
    lright->setPen(QPen(Qt::white));
    lines.append(lright);
    this->addToGroup(lright);
}

void PlotFrame::setLinesWidth(qreal width)
{
  for (QVector<QGraphicsLineItem *>::iterator it = lines.begin(); it != lines.end(); ++it) {
    QLineF old_line = (*it)->line();
    // offset_x != 0 for not hitting the CPU/task name
    (*it)->setLine(20, old_line.y1(), width, old_line.y1());
  }
}

void PlotFrame::setCPUHeight(qreal height)
{
  for (QVector<QGraphicsLineItem *>::iterator it = lines.begin(); it != lines.end(); ++it) {
    QLineF old_line = (*it)->line();
    (*it)->setLine(old_line.x1(), -30, old_line.x1(), -height);
  }
}
