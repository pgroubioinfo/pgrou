#include <iostream>
#include <graphviz/gvc.h>
#include <boost/make_shared.hpp>
#include <QStringList>
#include "Exceptions.h"
#include "GVSkeletonGraph.h"
#include "MainWindow.h"

// UTILS

// add subgraph
static inline Agraph_t* _agsubg(Agraph_t* object, QString name) {
	return agsubg(object, const_cast<char *>(qPrintable(name)));
}

// add node
static inline Agnode_t* _agnode(Agraph_t* object, QString name) {
	return agnode(object, const_cast<char *>(qPrintable(name)));
}


// CONSTRUCTORS

GVSkeletonGraph::GVSkeletonGraph(Agraph_t* graph) : _graph(graph) {	
	setGraphAttributes();
}

GVSkeletonGraph::~GVSkeletonGraph() {    
    agclose(_graph);
}


//Set basic graph attributes for graphviz
const qreal GVSkeletonGraph::nodeSize = 50;
const qreal GVSkeletonGraph::sepValue = 12.0;

void GVSkeletonGraph::setGraphAttributes(void) {

    //Selection of display mode
    QString mode;

    if(MainWindow::mwThis->getDisplayMode() == 0){

        //Low
        mode = "false";
    }
    else if(MainWindow::mwThis->getDisplayMode() == 1){

        //High
        mode = "true";
    }

    // set graph attributes
    _agset(_graph, "overlap", "scale");
    _agset(_graph, "splines", mode);
    //_agset(_graph, "splines", "curved"); // with fdp, "curved" provokes edge overlap on nodes
	_agset(_graph, "dpi", "96,0");
    QString strSepValue = QString::number(sepValue).prepend("+");
    _agset(_graph, "sep", strSepValue);

    // divide the wanted width by the DPI to get the value in points
    QString nodePtsWidth = QString("%1").arg(GVSubGraph::nodeSize/_agget(_graph, "dpi", "96,0").replace(',', ".").toDouble());
	//GV uses , instead of . for the separator in floats
    _agnodeattr(_graph, "width", nodePtsWidth.replace('.', ","));

}

//Set name
void GVSkeletonGraph::setLabel (const QString& name) {
	_agset(_graph, "label", name);
}

//Retrieve nodes for drawing (after layout has been done)
QList<GVNode> GVSkeletonGraph::nodes() {	
	QList<GVNode> list;
	qreal dpi = this->getDPI();
    for (QMap<QString, Agnode_t*>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it) {
        Agnode_t *node = it.value();
        GVNode object;
        object.name = node->name; //Node name
		//Position
        qreal x =  node->u.coord.x * (dpi/GVGraph::DotDefaultDPI);
        qreal y = -node->u.coord.y * (dpi/GVGraph::DotDefaultDPI);
        object.centerPos = QPoint(x, y);
		//Size
        object.height = node->u.height * dpi;
        object.width = node->u.width * dpi;
        list << object;
    }

    return list;
}

//Retrieve edges for drawing (after layout has been done)
QList<GVEdge> GVSkeletonGraph::edges() { 
	QList<GVEdge> list;
	qreal dpi = this->getDPI();
    for(QMap<QPair<QString, QString>, Agedge_t*>::const_iterator it=_edges.begin(); it!=_edges.end();++it) {
        Agedge_t *edge=it.value();
        GVEdge object;		
        //Fill the source and target node names
        object.source=edge->tail->name;
        object.target=edge->head->name;		
        //Calculate the path from the spline
        if((edge->u.spl->list!=0) && (edge->u.spl->list->size%3 == 1)) {
            //If there is a starting point, draw a line from it to the first curve point
            if(edge->u.spl->list->sflag) {
                object.path.moveTo	(	edge->u.spl->list->sp.x*(dpi/GVGraph::DotDefaultDPI)
									,	(-edge->u.spl->list->sp.y)*(dpi/GVGraph::DotDefaultDPI)
									);
                object.path.lineTo	(	edge->u.spl->list->list[0].x*(dpi/GVGraph::DotDefaultDPI)
									,	(-edge->u.spl->list->list[0].y)*(dpi/GVGraph::DotDefaultDPI)
									);
            } else
                object.path.moveTo	(	edge->u.spl->list->list[0].x*(dpi/GVGraph::DotDefaultDPI)
									,	(-edge->u.spl->list->list[0].y)*(dpi/GVGraph::DotDefaultDPI)
									);
            //Loop over the curve points
            for(int i=1; i<edge->u.spl->list->size; i+=3)
                object.path.cubicTo	(	edge->u.spl->list->list[i].x   * (dpi/GVGraph::DotDefaultDPI)
									, 	(-edge->u.spl->list->list[i].y)   * (dpi/GVGraph::DotDefaultDPI)
									,	edge->u.spl->list->list[i+1].x * (dpi/GVGraph::DotDefaultDPI)
									,	(-edge->u.spl->list->list[i+1].y) * (dpi/GVGraph::DotDefaultDPI)
									,	edge->u.spl->list->list[i+2].x * (dpi/GVGraph::DotDefaultDPI)
									,	(-edge->u.spl->list->list[i+2].y) * (dpi/GVGraph::DotDefaultDPI)
									);
            //If there is an ending point, draw a line to it
            if(edge->u.spl->list->eflag)
                object.path.lineTo	(	edge->u.spl->list->ep.x*(dpi/GVGraph::DotDefaultDPI)
									,	(-edge->u.spl->list->ep.y)*(dpi/GVGraph::DotDefaultDPI)
									);
        }

        list << object;
    }

    return list;
}

//Node management
int GVSkeletonGraph::countNodes (void) { return _nodes.count(); }
void GVSkeletonGraph::addNode(const QString& name) {
    if(_nodes.contains(name)) removeNode(name);	
    _nodes.insert(name, _agnode(_graph, name));
}
void GVSkeletonGraph::addNodes(const QStringList& names) {
    for(int i=0; i<names.size(); ++i)
        addNode(names.at(i));
}
void GVSkeletonGraph::removeNode(const QString& name) {
    if(_nodes.contains(name)) {
        agdelete(_graph, _nodes[name]);
        _nodes.remove(name);
        QList<QPair<QString, QString> >keys=_edges.keys();
        for(int i=0; i<keys.size(); ++i)
            if(keys.at(i).first==name || keys.at(i).second==name)
                removeEdge(keys.at(i));
    }
}


Agnode_t* GVSkeletonGraph::getNode (const QString& name) {
	if(_nodes.contains(name)) return _nodes[name];
	for (QMap<QString, GVSubGraphPtr>::const_iterator it = _subgraphs.begin(); it != _subgraphs.end(); ++it)
		if (it.value()->hasNode(name)) return it.value()->getNode(name);
	return NULL;
}
void GVSkeletonGraph::clearNodes() {
    QList<QString> keys=_nodes.keys();
    for(int i=0; i<keys.size(); ++i)
        removeNode(keys.at(i));
}

//Edge management
void GVSkeletonGraph::addEdge(const QString &source, const QString &target) {
    if (hasNode(source) && hasNode(target)) {
        QPair<QString, QString> key(source, target);
        if(!_edges.contains(key))
            _edges.insert(key, agedge(_graph, getNode(source), getNode(target)));
    }
}
void GVSkeletonGraph::removeEdge(const QString &source, const QString &target) {
    removeEdge(QPair<QString, QString>(source, target));
}
void GVSkeletonGraph::removeEdge(const QPair<QString, QString>& key) {
    if(_edges.contains(key)) {
        agdelete(_graph, _edges[key]);
        _edges.remove(key);
    }
}

//Getter for graph DPI
qreal GVSkeletonGraph::getDPI() {
	return _agget(_graph, "dpi", "96,0").replace(',', ".").toDouble();
}

Agedge_t* GVSkeletonGraph::getEdge(const QString &sourceName, const QString &targetName) {
    QPair<QString, QString> key(sourceName, targetName);
    if (!_edges.contains(key)) return NULL;
    return _edges.value(key);
}


Agraph_t* GVSkeletonGraph::graph() { return this->_graph; }
