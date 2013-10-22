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
		
		void GVSkeletonGraph::setGraphAttributes();
		
		void GVSkeletonGraph::applyLayout()
		
		QList<GVNode> GVSkeletonGraph::nodes();
		
		void GVSkeletonGraph::addNode(const QString& name);
		
		GVSkeletonGraph::removeNode(const QString& name);
		
		GVSkeletonGraph::hasNode(const QString& name);
		
		GVSkeletonGraph::getNode(const QString& name);
		
		void GVSkeletonGraph::clearNodes();
		
		qreal GVSkeletonGraph::getDPI();
		
		Agraph_t* GVSkeletonGraph::graph();

};