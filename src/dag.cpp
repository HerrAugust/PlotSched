#include "dag.h"
#include "config.h"

#include <sstream>
#include <fstream>
#include <string>
#include <QFile>
#include <QDebug>

using namespace std;

unsigned int Node::_noInsertedNodes = 1;
unsigned int DAG::_noInsertedDAGs = 1;

Node::Node(DAG *dag)
{
    _index = _noInsertedNodes;
    _noInsertedNodes++;
    _dag = dag;
}

QString Node::toString() const
{
    stringstream ss;
    ss << _task->str().toStdString(); // we'll see..
    return QString::fromStdString(ss.str());
}

bool Node::isSource() const
{
    return !_dag->isSinkAndSourceRemoved() && getPredecessors().size() == 0;
}

bool Node::isSink() const
{
    return !_dag->isSinkAndSourceRemoved() && getSuccessors().size() == 0;
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

// add an isolated node
void DAG::addNode(Node *n)
{
    _nodes.push_back(n);
}

// removes a node from the DAG
void DAG::removeNode(Node *n)
{
    _nodes.removeOne(n);

    for (const auto &otherNode : _nodes)
    {
        auto preds = otherNode->getPredecessors();
        if (preds.contains(n))
        {
            preds.removeOne(n);
            otherNode->setPredecessors(preds);
        }
        auto succ = otherNode->getSuccessors();
        if (succ.contains(n))
        {
            succ.removeOne(n);
            otherNode->setSuccessors(succ);
        }
    }

    delete n;
}

// add edge between two nodes
void DAG::addEdge(Node *from, Node *to)
{
    Q_ASSERT(from != to);

    QVector<Node *> succ = from->getSuccessors();
    succ.push_back(to);
    QVector<Node *> pred = to->getPredecessors();
    pred.push_back(from);
    from->setSuccessors(succ);
    to->setPredecessors(pred);
}

// add edge between two nodes
void DAG::addEdge(unsigned int indexFrom, unsigned int indexTo)
{
    addEdge(_nodes.at(indexFrom), _nodes.at(indexTo));
}

// construct graph from txt file containing an adj matrix
void DAG::fromFile(QString pathAdjMatrixTxt)
{
    QFile fPathAdjMatrixTxt(pathAdjMatrixTxt);
    Q_ASSERT(pathAdjMatrixTxt.endsWith("_adj_mx.txt"));

    if (!fPathAdjMatrixTxt.open(QFile::ReadOnly | QFile::Text))
        ROUTINE_CANNOT_OPEN_FILE(fPathAdjMatrixTxt)
    QTextStream inPathAdjMatrixTxt(&fPathAdjMatrixTxt);
    unsigned int linesNo = inPathAdjMatrixTxt.readAll().split("\n").size();
    fPathAdjMatrixTxt.close();

    // set DAG deadline
    QString pathDL = QString(pathAdjMatrixTxt).replace(".txt", "_dl.txt");
    QFile fPathDL(pathDL);
    if (!fPathDL.open(QFile::ReadOnly | QFile::Text))
        ROUTINE_CANNOT_OPEN_FILE(fPathDL)
    QTextStream inPathDL(&fPathDL);
    _deadline = (TICK)inPathDL.readAll().toFloat();
    fPathDL.close();

    // add nodes with their WCET
    unsigned int wcets[linesNo];
    QString pathWcet = QString(pathAdjMatrixTxt).replace(".txt", "_wcet.txt");
    QFile fPathWcet(pathWcet);
    if (!fPathWcet.open(QFile::ReadOnly | QFile::Text))
        ROUTINE_CANNOT_OPEN_FILE(fPathWcet)
    QTextStream inPathWcet(&fPathWcet);
    QString filecontent = inPathWcet.readAll().trimmed();
    fPathWcet.close();
    for (auto wcet : filecontent.split(" "))
        addNode(new Node(this));

    // add edges. recycling some old code..
    ifstream myfile(pathAdjMatrixTxt.toStdString());
    string line;
    int lineNo = 0;
    while (std::getline(myfile, line))
    {
        int connectedNodeIndex = 0;
        for (QString chara : QString::fromStdString(line).split(" "))
        {
            if (chara[0] == '1')
                addEdge(lineNo, connectedNodeIndex);
            connectedNodeIndex++;
        }
        lineNo++;
    }
    myfile.close();

    commit();
}

void DAG::commit()
{
    Q_ASSERT(_deadline > 0);
    Q_ASSERT(getNodes().size() > 0);

    _adj.resize(getNodes().size());
    for (const auto &node : getNodes())
        for (const auto &succ : node->getSuccessors())
            _adj[node->getIndex() - 1].push_back(succ->getIndex() - 1);

    Node::_noInsertedNodes = 1;

    removeSourceAndSinkNodes();
}

void DAG::removeSourceAndSinkNodes()
{
    Q_ASSERT(!this->isSinkAndSourceRemoved());

    removeNode(_nodes[0]);
    removeNode(_nodes[_nodes.size() - 1]);

    _isSourceAndSinkNodesRemoved = true;
}
