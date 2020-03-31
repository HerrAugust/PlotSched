/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RTLIB2_0_DAG_H
#define RTLIB2_0_DAG_H

#include "config.h"
#include "task.h"

#include <QVector>

class DAG;

class Node
{
    friend DAG;

protected:
    /// the corresponding task (todo creates a circular reference)
    Task *_task;

    /// node successors and predecessors
    QVector<Node *> _successors, _predecessors;

    /// the DAG this node belongs to. Will be useful for debug
    DAG *_dag;

    /// node index
    unsigned int _index;

    /// node index in its dot file
    unsigned int _dotFileIndex;

    /// number of inserted nodes
    static unsigned int _noInsertedNodes;

public:
    Node(DAG *dag);

    inline bool isSource() const;

    inline bool isSink() const;

    inline DAG *getDAG() const { return _dag; }

    inline unsigned int getIndex() const { return _index; }

    inline unsigned int getDotFileIndex() const { return _dotFileIndex; }

    inline void getDotFileIndex(unsigned int i) { _dotFileIndex = i; }

    inline QVector<Node *> getSuccessors() const { return _successors; }

    inline void setSuccessors(QVector<Node *> s) { _successors = s; }

    inline QVector<Node *> getPredecessors() const { return _predecessors; }

    inline void setPredecessors(QVector<Node *> s) { _predecessors = s; }

    inline void setTask(Task *task) { _task = task; }

    inline Task *getTask() const { return _task; }

    bool operator==(Node &other) { return _index == other._index; }

    // node to string. Call it also with this->str() :)
    QString toString() const;

    QString str() const { return toString(); }
};

/**
     * Implementation of graph.
     * 
     * DAG g;
     * g->fromFile("adjMatrix.txt");
     * list<Node*> cp = g->getCriticalPath();
     */
class DAG
{
private:
    // nodes list
    QVector<Node *> _nodes;

    // adjacency list. this is not stricly needed and can _nodes could be used instead
    QVector<QVector<int>> _adj;

    // DAG ID
    unsigned int _index;

    // number of inserted DAGs
    static unsigned int _noInsertedDAGs;

    // DAG deadline
    TICK _deadline;

    /// true if you have already removed sink and source nodes
    bool _isSourceAndSinkNodesRemoved = false;

    /// path of dot file for this DAG
    QString _rootFolderDAG;

    // add an isolated node
    void addNode(Node *n);

    // add edge between two nodes
    void addEdge(Node *from, Node *to);

    // add edge between two nodes given their index
    void addEdge(unsigned int indexFrom, unsigned int indexTo);

    // you state the graph is complete.
    // this function prepares the graph for later computations
    void commit();

public:
    DAG() {}

    ~DAG();

    inline unsigned int getIndex() const { return _index; }

    // construct graph from txt file containing an adj matrix
    void fromFile(QString pathAdjMatrixTxt);

    // removes a node from the DAG
    void removeNode(Node *n);

    /// removes source and sink nodes from DAG
    void removeSourceAndSinkNodes();

    // returns the nodes
    inline QVector<Node *> getNodes() const { return _nodes; }

    inline void setNodes(QVector<Node *> &nodes) { _nodes = nodes; }

    // returns source nodes
    QVector<Node *> getSourceNodes() const;

    // returns sink nodes
    QVector<Node *> getSinkNodes() const;

    /// true if you have already required to removed sink and source nodes
    inline bool isSinkAndSourceRemoved() const { return _isSourceAndSinkNodesRemoved; }

    /// set path to dot file for this graph
    inline void setRootFolder(QString path) { _rootFolderDAG = path; }

    /// returns path to dot file for this graph
    inline QString getRootFolder() const { return _rootFolderDAG; }

    // returns DAG deadline
    inline TICK getDeadline() const { return _deadline; }

    // set DAG deadline = period
    inline void setDeadline(TICK deadline) { _deadline = deadline; }

    // returns node successors
    inline QVector<Node *> getSuccessorsOfNode(Node *n) const { return n->getSuccessors(); }

    // returns node predecessors
    inline QVector<Node *> getPredecessorsOfNode(Node *n) const { return n->getPredecessors(); }

    // graph to string
    QString str() const;
};

#endif
