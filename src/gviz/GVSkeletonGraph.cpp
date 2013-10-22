#include <iostream>
#include <graphviz/gvc.h>
#include <boost/make_shared.hpp>
#include <QStringList>
#include <QRectF>
#include "Exceptions.h"
#include "GVSkeletonGraph.h"

const qreal GVSkeletonGraph::DotDefaultDPI=72.0;
const qreal GVSkeletonGraph::nodeSize = 50;
const qreal GVSkeletonGraph::sepValue = 12.0;

static inline Agnode_t* _agnode(Agraph_t* object, QString name){
	return agnode(object, const_cast<char *>(qPrintable(name)));
}

static inline Agraph_t* _agopen(QString name, int kind){
	return agopen(const_cast<char *>(qPrintable(name)),kind);
}

static inline QString _agget(void *object, QString attr, QString alt=QString()){
	QString str = agget(object, const_cast<char *>(qPrintable(attr)));
	if(str==QString()) return alt;
	else return str;
}

static inline Agsym_t* _agnodeattr(Agraph_t* object, QString attr, QString value){
	return agnodeattr(object, const_cast<char *>(qPrintable(attr)),const_cast<char *>(qPrintable(value)));
}

static inline Agsym_t* _agedgeattr(Agraph_t* object, QString attr, QString value){
	return agedgeattr(object, const_cast<char *>(qPrintable(attr)),const_cast<char *>(qPrintable(value)));
}

static inline void _gvLayout(GVC_t* context, Agraph_t* graph, QString alg){
	gvLayout(context, graph, const_cast<char *>(qPrintable(alg)));
}

GVSkeletonGraph::GVSkeletonGraph(QString name, QFont font) {
	_context = gvContext();
	_graph = _agopen(name, AGDIGRAPHSTRICT);
	setGraphAttributes();
	setFont(font);
}

GVSkeletonGraph::~GVSkeletonGraph(){
	gvFreeLayout(_context,_graph);
	agclose(_graph);
	gvFreeContext(_context);
}

void GVSkeletonGraph::setGraphAttributes(){
	
	setGraphObjectAttributes(_graph,"dpi","96,0");
	QString strSepValue = QString::number(sepValue).prepend("+");
	setGraphObjectAttributes(_graph, "sep", strSepValue);
	
	QString nodePtsWidth = QString("%1").arg(GVSkeletonGraph::nodeSize/_agget(_graph,"dpi", "96,0").replace(',',".").toDouble());
	_agnodeattr(_graph, "width", nodePtsWidth.replace('.',","));
}

void GVSkeletonGraph::setGraphObjectAttributes(void *object, QString attr, QString value){
	agsafeset(object, const_cast<char *>(qPrintable(attr)),const_cast<char *>(qPrintable(value)),const_cast<char *>(qPrintable(value)));
}

void GVSkeletonGraph::setFont(QFont font){
	_font = font;
	setGraphObjectAttributes(_graph,"fontname",font.family());
}

void GVSkeletonGraph::applyLayout(){
	gvFreeLayout(_context, _graph);
	_gvLayout(_context, _graph, "fdp");
}

QList<GVNode> GVSkeletonGraph::nodes(){
	QList<GVNode> list;
	qreal dpi = this->getDPI();
	for(QMap<QString, Agnode_t*>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it){
		Agnode_t *node = it.value();
		GVNode object;
		object.name = node->name;
		
		qreal x = node ->u.coord.x *(dpi/GVSkeletonGraph::DotDefaultDPI);
		qreal y = -node ->u.coord.y *(dpi/GVSkeletonGraph::DotDefaultDPI);
		
		object.centerPos = QPoint(x,y);
		
		object.height = node->u.height * dpi;
		object.width = node->u.width * dpi;
		
		list << object;
	}
	
	return list;
}

void GVSkeletonGraph::addNode(const QString& name){
	if(_nodes.contains(name)) removeNode(name);
	_nodes.insert(name, _agnode(_graph, name));
}

void GVSkeletonGraph::removeNode(const QString& name){
	if(_nodes.contains(name)){
		agdelete(_graph, _nodes[name]);
		_nodes.remove(name);
	}
}

bool GVSkeletonGraph::hasNode(const QString& name){
	if(_nodes.contains(name)) return true;
	return false;
}

Agnode_t* GVSkeletonGraph::getNode(const QString& name){
	if(_nodes.contains(name)) return _nodes[name];
	return NULL;
}

void GVSkeletonGraph::clearNodes(){
	QList<QString> keys = _nodes.keys();
	for(int i=0;i<keys.size();++i){
		removeNode(keys.at(i));
	}
}

qreal GVSkeletonGraph::getDPI(){
	return _agget(_graph, "dpi","96,0").replace(',',".").toDouble();
}

Agraph_t* GVSkeletonGraph::graph() {return this->_graph;}