#include <iostream>
#include <QMatrix>
#include <QPointF>
#include <QVector>
#include <QColor>
#include <Qt>
#include <QVector2D>
#include "GAction.h"

GAction::GAction(ActionPtr a, PHScene* sc) : scene(sc), action(a) {
    display = new QGraphicsItemGroup();

    initContactPoints();

    actionPath = new QGraphicsPathItem(createPath(),display);
}

void GAction::update() {

    updateContactPoints();

    actionPath->setPath(createPath());

}

GAction::GAction(){
}

// Important for the destructor : do not delete scene, it's owned by a shared pointer in PH.h
GAction::~GAction() {
    delete display;
}

void GAction::initContactPoints(){
    GProcessPtr source = getSource();
    GProcessPtr target = getTarget();
    GProcessPtr result = getResult();

    if(source!=target){
	    QVector2D* hitVector = new QVector2D(*(target->getCenterPoint()) - *(source->getCenterPoint()));
	    hitVector->normalize();

	    QSizeF* sizeSource = source->getSizeEllipse();
	    QSizeF* sizeTarget = source->getSizeEllipse();


	    sourcePoint = new QPointF(sizeSource->width()*hitVector->x()/2 + source->getCenterPoint()->x(),sizeSource->height()*hitVector->y()/2 + source->getCenterPoint()->y());

	    targetPoint = new QPointF(-sizeTarget->width()*hitVector->x()/2 + target->getCenterPoint()->x(),-sizeTarget->height()*hitVector->y()/2 + target->getCenterPoint()->y());

	    if(target!=result){
    	    	resultPoint = new QPointF(-sizeTarget->width()*hitVector->x()/2 + result->getCenterPoint()->x(),sizeTarget->height()*hitVector->y()/2 + result->getCenterPoint()->y());
	    }else{
	        resultPoint = targetPoint;
	    }
     }else{
	    sourcePoint = new QPointF(GProcess::sizeDefault/2 + result->getCenterPoint()->x(),result->getCenterPoint()->y());
            targetPoint = sourcePoint;
            resultPoint = new QPointF(GProcess::sizeDefault/2 + result->getCenterPoint()->x(),result->getCenterPoint()->y());
     }

}

void GAction::updateContactPoints(){
    GProcessPtr source = getSource();
    GProcessPtr target = getTarget();
    GProcessPtr result = getResult();

    if(source!=target){
    	QVector2D* hitVector = new QVector2D(*(target->getCenterPoint()) - *(source->getCenterPoint()));
    	hitVector->normalize();

    	QSizeF* sizeSource = source->getSizeEllipse();
    	QSizeF* sizeTarget = source->getSizeEllipse();

    	sourcePoint->setX(sizeSource->width()*hitVector->x()/2 + source->getCenterPoint()->x());
    	sourcePoint->setY(sizeSource->height()*hitVector->y()/2 + source->getCenterPoint()->y());
    	targetPoint->setX(-sizeTarget->width()*hitVector->x()/2 + target->getCenterPoint()->x());
    	targetPoint->setY(-sizeTarget->height()*hitVector->y()/2 + target->getCenterPoint()->y());
	if(target!=result){
    		resultPoint->setX(-sizeTarget->width()*hitVector->x()/2 + target->getCenterPoint()->x());
    		resultPoint->setY(sizeTarget->height()*hitVector->y()/2 + target->getCenterPoint()->y());
	}
     }else{
    	sourcePoint->setX(GProcess::sizeDefault/2 + result->getCenterPoint()->x());
    	sourcePoint->setY(result->getCenterPoint()->y());
        resultPoint->setX(GProcess::sizeDefault/2 + result->getCenterPoint()->x());
        resultPoint->setY(result->getCenterPoint()->y());
     }
}

QPainterPath GAction::createPath(){
    QPainterPath path(*sourcePoint);

    if(sourcePoint!=targetPoint){
	path.lineTo(*targetPoint);
    }else{
    	path.arcTo(QRectF(sourcePoint->x(),sourcePoint->y()-25,20,50),90,360);
    }
    
    path.addPolygon(makeArrowHead(path));

    qreal rectCornerX;
    qreal rectCornerY;
    qreal widthRect;
    qreal heightRect;
    qreal sweepAngle;
    qreal startAngle;
    QLineF* targetToResult = new QLineF(targetPoint->x(), targetPoint->y(), resultPoint->x(), resultPoint->y());

    if(targetPoint->y()<resultPoint->y()){
	rectCornerY = targetPoint->y();
        heightRect = resultPoint->y()-targetPoint->y();
	startAngle = 90;
    } else{
	rectCornerY = resultPoint->y();
	heightRect = targetPoint->y()-resultPoint->y();
	startAngle = -90;
    }

    if(resultPoint->x()<getResult()->getCenterPoint()->x()){
	sweepAngle = 180;
    }else{
	sweepAngle = -180;
    }

   if (targetToResult->length()> GProcess::sizeDefault)
   {
         rectCornerX = resultPoint->x()- GProcess::sizeDefault/2;
         widthRect = GProcess::sizeDefault;
    }else{

        rectCornerX = resultPoint->x()- targetToResult->length()/2;
        widthRect = targetToResult->length()/2 ;
    }

    path.arcTo(QRectF(rectCornerX,rectCornerY,widthRect,heightRect),startAngle,sweepAngle);

    path.addPolygon(makeArrowHead(path));

    return path;
}

// draw an arrowhead
QPolygonF GAction::makeArrowHead(QPainterPath path) {

    // arrow pointing to the right
    QPointF p = path.pointAtPercent(1);
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
    matrix.rotate(-path.angleAtPercent(1));
    matrix.translate(-p.x(), -p.y());
    polygon = matrix.map(polygon);

    // turn into QGraphicsPolygonItem
    return polygon;
}

// getters

QGraphicsItem* GAction::getDisplayItem (void) {
    return display;
}

ActionPtr GAction::getAction() {
    return action;
}

GProcessPtr GAction::getSource() {
    return action->getSource()->getGProcess();
}

GProcessPtr GAction::getTarget() {
    return action->getTarget()->getGProcess();
}

GProcessPtr GAction::getResult() {
    return action->getResult()->getGProcess();
}
