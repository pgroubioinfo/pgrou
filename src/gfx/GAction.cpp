#include <iostream>
#include <QMatrix>
#include <QPointF>
#include <QVector>
#include <QColor>
#include <Qt>
#include <QVector2D>
#include "GAction.h"


GAction::GAction(ActionPtr a, GVEdge e, GVEdge f, PHScene* sc) : scene(sc), action(a) {

    edges = std::make_pair (e, f);
    display = new QGraphicsItemGroup();

    // Gsort where this action starts
    GSortPtr owner = scene->getGSort(action->getSource()->getSort()->getName());
    QColor color = *(owner->color);

    // arrow tails
    arrowTails.first 	= new QGraphicsPathItem(edges.first.path, display);
    arrowTails.first->setPen(QPen(color));
    arrowTails.second 	= new QGraphicsPathItem(edges.second.path, display);
    arrowTails.second->setPen(QPen(color, 0, Qt::DashLine));

    // arrow heads
    arrowHeads.first 	= makeArrowHead(edges.first, color);
    arrowHeads.second 	= makeArrowHead(edges.second, color);
}

GAction::GAction(ActionPtr a, PHScene* sc) : scene(sc), action(a) {
    display = new QGraphicsItemGroup();

    GProcessPtr source = action->getSource()->getGProcess();
    GProcessPtr target = action->getTarget()->getGProcess();
    GProcessPtr result = action->getResult()->getGProcess();


    QVector2D* hitVector = new QVector2D(*(target->getCenterPoint()) - *(source->getCenterPoint()));
    hitVector->normalize();

    QSizeF* sizeSource = source->getSizeEllipse();
    QSizeF* sizeTarget = source->getSizeEllipse();


    sourcePointLine = new QPointF(sizeSource->width()*hitVector->x()/2 + source->getCenterPoint()->x(),sizeSource->height()*hitVector->y()/2 + source->getCenterPoint()->y());

    targetPointLine = new QPointF(-sizeTarget->width()*hitVector->x()/2 + target->getCenterPoint()->x(),-sizeTarget->height()*hitVector->y()/2 + target->getCenterPoint()->y());

    resultPointLine = new QPointF(-sizeTarget->width()*hitVector->x()/2 + result->getCenterPoint()->x(),sizeTarget->height()*hitVector->y()/2 + result->getCenterPoint()->y());

 //   hitLine = new QGraphicsLineItem(QLineF(*targetPointLine,*sourcePointLine),display);
 //   hitLine->setPen(QPen(QColor(0,0,0)));


    qreal rectCornerX;
    qreal rectCornerY;
    qreal widthRect;
    qreal heightRect;
    qreal centerArcY = (targetPointLine->y()+resultPointLine->y())/2;
    qreal sweepAngle;

    if(targetPointLine->y()<resultPointLine->y()){
	rectCornerY = targetPointLine->y();
        heightRect = resultPointLine->y()-targetPointLine->y();
    } else{
	rectCornerY = resultPointLine->y();
	heightRect = targetPointLine->y()-resultPointLine->y();
    }

    if(resultPointLine->x()<result->getCenterPoint()->x()){
	sweepAngle = 180;
    }else{
	sweepAngle = -180;
    }

    rectCornerX = resultPointLine->x()- GProcess::sizeDefault/2;
    widthRect = GProcess::sizeDefault;

    QPainterPath path(QPointF(rectCornerX+widthRect/2,rectCornerY));

    path.arcTo(QRectF(rectCornerX,rectCornerY,widthRect,heightRect),90,sweepAngle);
    resultLine = new QGraphicsPathItem(path,display);


}

void GAction::update() {

    GProcessPtr source = action->getSource()->getGProcess();
    GProcessPtr target = action->getTarget()->getGProcess();

    QVector2D* hitVector = new QVector2D(*(target->getCenterPoint()) - *(source->getCenterPoint()));
    hitVector->normalize();

    QSizeF* sizeSource = source->getSizeEllipse();
    QSizeF* sizeTarget = source->getSizeEllipse();

    sourcePointLine->setX(sizeSource->width()*hitVector->x()/2 + source->getCenterPoint()->x());
    sourcePointLine->setY(sizeSource->height()*hitVector->y()/2 + source->getCenterPoint()->y());
    targetPointLine->setX(-sizeTarget->width()*hitVector->x()/2 + target->getCenterPoint()->x());
    targetPointLine->setY(-sizeTarget->height()*hitVector->y()/2 + target->getCenterPoint()->y());

    hitLine->setLine(QLineF(*targetPointLine,*sourcePointLine));

}

GAction::GAction(){
}

// Important for the destructor : do not delete scene, it's owned by a shared pointer in PH.h
GAction::~GAction() {
    delete display;
}


// draw an arrowhead
QGraphicsPolygonItem* GAction::makeArrowHead(const GVEdge& e, const QColor& color) {

    // arrow pointing to the right
    QPointF p = e.path.pointAtPercent(1);
    QPointF* q = new QPointF(p.x() - 8, p.y() - 5);
    QPointF* r = new QPointF(p.x() - 8, p.y() + 5);
    QPointF* s = new QPointF(p.x() - 1, p.y());
    QPointF* t = new QPointF(p.x() - 7.5, p.y() + 4);
    QPointF* u = new QPointF(p.x() - 7.5, p.y() - 4);
    QPolygonF polygon;
    polygon.push_back(p);
    polygon.push_back(*q);
    polygon.push_back(*u);
    polygon.push_back(*s);
    polygon.push_back(*t);
    polygon.push_back(*r);

    // rotate arrow
    QMatrix matrix;
    matrix.translate(p.x(), p.y());
    matrix.rotate(-e.path.angleAtPercent(1));
    matrix.translate(-p.x(), -p.y());
    polygon = matrix.map(polygon);

    // turn into QGraphicsPolygonItem
    QGraphicsPolygonItem* res = new QGraphicsPolygonItem (polygon, display);
    res->setPen(QPen(color));
    res->setBrush(QBrush(color));
    return res;
}

// getters

QGraphicsItem* GAction::getDisplayItem (void) {
    return display;
}

ActionPtr GAction::getAction() {
    return action;
}

GSortPtr GAction::getSourceSort() {
    return scene->getGSort(action->getSource()->getSort()->getName());
}

GSortPtr GAction::getTargetSort() {
    return scene->getGSort(action->getTarget()->getSort()->getName());
}

GSortPtr GAction::getResultSort() {
    return scene->getGSort(action->getResult()->getSort()->getName());
}
