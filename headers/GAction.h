#pragma once
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <list>
#include <utility>
#include "GVEdge.h"
#include "PHScene.h"
#include "Action.h"
#include "GSort.h"


/**
  * @file GAction.h
  * @brief header for the GAction class
  * @author PAPPL_2012
  *
  */

class GAction;
typedef boost::shared_ptr<GAction> GActionPtr;
class Action;
typedef boost::shared_ptr<Action> ActionPtr;
class PHScene;

using std::pair;


/**
  * @class GAction
  * @brief contains style and layout info to draw an action
  *
  */
class GAction {

	public:

    /**
      * @brief constructor
      *
      * @param ActionPtr the related Action object
      * @param GVEdge the object that contains style and layout info for hit arrow
      * @param GVEdge the object that contains style and layout info for bounce (result) arrow
      * @param PHScene the related scene
      */
	GAction(ActionPtr a, GVEdge e, GVEdge f, PHScene* sc);

    /**
      * @brief constructor
      *
      * @param ActionPtr the related Action object
      * @param PHScene the related scene
      */
	GAction(ActionPtr a, PHScene* sc);

	GAction();

        ~GAction();

     /**
       * @brief update position of GAction
       *
       */
        void update();

     /**
       * @brief gets the display
       *
       * @return QGraphicsItem the graphical item representing the Action
       */
		QGraphicsItem* getDisplayItem (void);

        /**
          * @brief gets the action
          *
          */
        ActionPtr getAction();

        /**
          * @brief gets the source GProcess item
          *
          * @preturn GProcessPtr a pointer to the source GProcess item
          */
        GProcessPtr getSource();

        /**
          * @brief gets the target GProcess item
          *
          * @return GProcessPtr a pointer to the target GProcess item
          */
        GProcessPtr getTarget();

        /**
          * @brief gets the result GProcess item
          *
          * @param GProcessPtr a pointer to the result GProcess item
          */
        GProcessPtr getResult();

	
	protected:
	
        /**
          * @brief the PHScene related to the Action
          *
          */
	PHScene* scene;

        /**
          * @brief the graphical item representing the Action
          *
          */
	QGraphicsItem* display;

        /**
          * @brief line representing the first part of the action
          *
          */
        QGraphicsLineItem* hitLine;

        /**
          * @brief path representing the second part of the action
          *
          */
        QGraphicsPathItem* actionPath;

        /**
          * @brief target Point of the line representing the first part of the action
          *
          */
        QPointF* targetPoint;

        /**
          * @brief source Point of the line representing the first part of the action
          *
          */
        QPointF* sourcePoint;


        /**
          * @brief source Point of the arc representing the second part of the action
          *
          */
        QPointF* resultPoint;

        /**
          * @brief the related Action
          *
          */
        ActionPtr action;

        /**
          * @brief the pair of graphical items representing the tails of the arrows of the Action
          *
          */
	pair<QGraphicsPathItem*, QGraphicsPathItem*> 		arrowTails;

        /**
          * @brief the pair of graphical items representing the heads of the arrows of the Action
          *
          */
        pair<QGraphicsPolygonItem* , QGraphicsPolygonItem*>	arrowHeads;

        /**
          * @brief the edges related to the arrows
          *
          */
	pair<GVEdge, GVEdge> edges;

        /**
          * @brief init the position of sourcePointLine, targetPointLine and resultPointLine
          */
	void initContactPoints();

        /**
          * @brief update the position of sourcePointLine, targetPointLine and resultPointLine
          */
	void updateContactPoints();
        
	/**
          * @brief build the arc corresponding to the second part of the action
	  *
	  * @return QPainterPath the arc representing the second part of the action
          */
	QPainterPath createPath();

        /**
          * @brief draws the head of the arrow
          *
          * @param GVEdge the edge related to the arrow
          * @param Qcolor the color of the arrow
          *
          * @return QGraphicsPolygonItem* the graphical item representing the head of the arrow
          *
          */
	QPolygonF makeArrowHead(QPainterPath path);		
	
};
