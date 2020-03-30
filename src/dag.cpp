#include "dag.h"

#include <sstream>

using namespace std;

QString Node::toString() const
{
    stringstream ss;
    ss << ""; // we'll see..
    return QString::fromStdString(ss.str());
}

QVector<Node *> DAG::getSourceNodes() const
{
    QVector<Node *> res;

    for (const auto &elem : getNodes())
    {
        if (elem->getPredecessors().size() == 0)
            res.push_back(elem);
    }

    return res;
}

QVector<Node *> DAG::getSinkNodes() const
{
    QVector<Node *> res;

    for (const auto &elem : getNodes())
    {
        if (elem->getSuccessors().size() == 0)
            res.push_back(elem);
    }

    return res;
}

QString DAG::str() const
{
    stringstream s;
    for (const auto &elem : _nodes)
    {
        s << elem->str().toStdString() << endl
          << " has successors: " << endl;
        for (Node *succ : elem->getSuccessors())
            s << "\t" << succ->str().toStdString() << endl;
        s << endl;
    }
    return QString::fromStdString(s.str());
}