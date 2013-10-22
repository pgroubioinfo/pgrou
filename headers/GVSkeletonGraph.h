#pragma once
#include <graphviz/gvc.h>
#include <boost/shared_ptr.hpp>
#include <QFont>
#include <QMap>
#include <QPair>
#include <QString>
#include "GVNode.h"
#include "GVEdge.h"

/**
  * @file GVSkeletonGraph.h
  * @brief header for the GVSkeletonGraph class
  * @author PAPPL_2012
  *
  */

class GVSkeletonGraph;
typedef boost::shared_ptr<GVSkeletonGraph> GVSkeletonGraphPtr;


/**
  * @class GVSkeletonGraph
  * @brief the object containing a libraph subgraph and its associated nodes and edges
  *
  */
class GVSkeletonGraph {

	public:

		static const qreal DotDefaultDPI;

		static const qreal nodeSize;

        static const qreal sepValue;

		GVSkeletonGraph(QString name, QFont font);

		virtual ~GVSkeletonGraph();
		
		void  setGraphAttributes();
		
		int setGraphObjectAttributes(void *object, QString attr, QString value);
		
		void  applyLayout();
		
		QList<GVNode>  nodes();
		
		void addNode(const QString& name);
		
		void removeNode(const QString& name);
		
		bool hasNode(const QString& name);
		
		Agnode_t* getNode(const QString& name);
		
		void  clearNodes();
		
		qreal  getDPI();
		
		Agraph_t*  graph();
		
		protected:
		
		Agraph_t* _graph;
		QMap<QString, Agnode_t*> _nodes;
};