#include <iostream>
#include <QPen>
#include <QColor>
#include <QBrush>
#include <QTextDocument>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QDebug>
#include <QApplication>
#include <QMenu>
#include <QPoint>
#include <QSize>
#include <cmath>
#include "GSort.h"


const int GSort::marginDefault = 10;
const int GSort::defaultDistance = 25;

GSort::GSort(SortPtr s, GVNode n, qreal width, qreal height) : QGraphicsRectItem(n.centerPos.x()-width/2, n.centerPos.y()-height/2, width, height),sort(s), node(n) {

    // graphic items set and Actions color
    color = makeColor();
    sizeRect = new QSize(width, height);
    vertical = true;

    simpleDisplay = true;
    leftTopCorner = new QPoint(n.centerPos.x()-sizeRect->width()/2,n.centerPos.y()-sizeRect->height()/2);

    // rectangle
    _rect = new QGraphicsRectItem(QRectF(*leftTopCorner, *sizeRect),this);
    _rect->setPen(QPen(QColor(0,0,0)));
    _rect->setBrush(QBrush(QColor(255,255,255)));

    // label
    text = new QGraphicsTextItem (QString(), this);
    text->setHtml(QString::fromStdString("<u>sort " + sort->getName() + "</u>"));
    text->setDefaultTextColor(*color);
    text->setPos(leftTopCorner->x()+sizeRect->width()/2, leftTopCorner->y());
    QSizeF textSize = text->document()->size();
    text->setPos(text->x() - textSize.width()/2, text->y() - textSize.height());

    setCursor(QCursor(Qt::OpenHandCursor));
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);

    // set related GProcesses as children (so they move with this GSort)
    vector<ProcessPtr> processes = sort->getProcesses();
    int currPosYProcess = marginDefault+GProcess::sizeDefault/2;

    for(ProcessPtr &p : processes){
	gProcesses.push_back(make_shared<GProcess>(p,leftTopCorner->x() + GProcess::sizeDefault/2+ marginDefault, leftTopCorner->y()+ currPosYProcess));
	currPosYProcess+= 2*marginDefault + GProcess::sizeDefault;
    }

    for(GProcessPtr &gp: gProcesses){
	gp->getDisplayItem()->setParentItem(this);
	ProcessPtr* p = gp->getProcess();
	(*p)->setGProcess(gp);
    }

}

GSort::~GSort() {
    gProcesses.clear();
    delete _rect;
    delete leftTopCorner;
    delete sizeRect;
    delete text;   
}


void GSort::changeOrientation(){

    qreal topLeftX=0;
    qreal topLeftY=0;
    qreal bottomRightX=0;
    qreal bottomRightY=0;
    _rect->rect().getCoords(&topLeftX,&topLeftY,&bottomRightX,&bottomRightY);
  

    //Swap Height and Width
    qreal oldWidth = sizeRect->width();
    qreal oldHeight = sizeRect->height();
    sizeRect->transpose();


    leftTopCorner->setX(leftTopCorner->x() + oldWidth/2.0 - sizeRect->width()/2.0);
    leftTopCorner->setY(leftTopCorner->y() + oldHeight/2.0 - sizeRect->height()/2.0);

    QPoint sceneLeftTopCorner(topLeftX + oldWidth/2.0 - sizeRect->width()/2.0,topLeftY + oldHeight/2.0 - sizeRect->height()/2.0);

    _rect->setRect(QRectF(sceneLeftTopCorner, *sizeRect));
    setRect(_rect->rect());

    _rect->rect().getCoords(&topLeftX,&topLeftY,&bottomRightX,&bottomRightY);


    text->setPos(topLeftX+sizeRect->width()/2, topLeftY);
    QSizeF textSize = text->document()->size();
    text->setPos(text->x() - textSize.width()/2, text->y() - textSize.height());


    if(vertical){
        vertical = false;

        int currPosXProcess = marginDefault+GProcess::sizeDefault/2;
        int i = 0;
	int currCenterX = 0;
        int currCenterY = 0;

        for(GProcessPtr &p: gProcesses){
            p->getCenterPoint()->setX(leftTopCorner->x() + currPosXProcess);
            p->getCenterPoint()->setY(leftTopCorner->y()+ GProcess::sizeDefault/2+ marginDefault);

	    currCenterX = topLeftX +  currPosXProcess;
            currCenterY = topLeftY + GProcess::sizeDefault/2 + marginDefault;

            int margin(GSort::marginDefault);

            p->getMarginRect()->setPos(currCenterX -p->getSizeEllipse()->width()/2,currCenterY-p->getSizeEllipse()->height()/2);

            p->getEllipseItem()->setRect(currCenterX -p->getSizeEllipse()->width()/2,currCenterY -p->getSizeEllipse()->height()/2, p->getSizeEllipse()->width(), p->getSizeEllipse()->height());
            textSize = p->getText()->document()->size();
            p->getText()->setPos(currCenterX - textSize.width()/2,currCenterY- textSize.height()/2);

            currPosXProcess+= 2*marginDefault + GProcess::sizeDefault;
        }

    }
    else{
        vertical=true;
        int currPosYProcess = marginDefault+GProcess::sizeDefault/2;
	int currCenterX = 0;
        int currCenterY = 0;

        for(GProcessPtr &p: gProcesses){
            p->getCenterPoint()->setX(leftTopCorner->x() + GProcess::sizeDefault/2+ marginDefault);
            p->getCenterPoint()->setY(leftTopCorner->y()+ currPosYProcess);

	    currCenterX = topLeftX + GProcess::sizeDefault/2 + marginDefault;
            currCenterY = topLeftY +  currPosYProcess;

            int margin(GSort::marginDefault);
            p->getMarginRect()->setPos(currCenterX -p->getSizeEllipse()->width()/2,currCenterY-p->getSizeEllipse()->height()/2);
            p->getEllipseItem()->setRect(currCenterX -p->getSizeEllipse()->width()/2,currCenterY -p->getSizeEllipse()->height()/2, p->getSizeEllipse()->width(), p->getSizeEllipse()->height());
            textSize = p->getText()->document()->size();
            p->getText()->setPos(currCenterX - textSize.width()/2,currCenterY- textSize.height()/2);
            currPosYProcess+= 2*marginDefault + GProcess::sizeDefault;
        }
    }
    dynamic_cast<PHScene*>(scene())->updateActions();

}

// mouse press event handler: start "drag"
void GSort::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    // ignore right click
    if (event->button() == Qt::RightButton) {
        changeOrientation();
        dynamic_cast<PHScene*>(scene())->updateActions();
        event->accept();
    }else if (event->button() == Qt::LeftButton) {
	setCursor(QCursor(Qt::ClosedHandCursor));
	// record coordinates for drawing item when mouse is moved/released
    	initPosPoint.setX(pos().x());
    	initPosPoint.setY(pos().y());
    	eventPressPoint.setX(event->scenePos().x());
    	eventPressPoint.setY(event->scenePos().y());
    	event->accept();
    }
}


// mouse move event handler: process "drag"
void GSort::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    // update item position
    setX(x() + event->scenePos().x() - eventPressPoint.x());
    setY(y() + event->scenePos().y() - eventPressPoint.y());
    leftTopCorner->setX(leftTopCorner->x() + event->scenePos().x() - eventPressPoint.x());
    leftTopCorner->setY(leftTopCorner->y() + event->scenePos().y() - eventPressPoint.y());
    for(GProcessPtr &p: gProcesses){
	    qreal prevPosX = p->getCenterPoint()->x();
	    qreal prevPosY = p->getCenterPoint()->y();
	    p->getCenterPoint()->setX(prevPosX + event->scenePos().x()- eventPressPoint.x());
	    p->getCenterPoint()->setY(prevPosY + event->scenePos().y()- eventPressPoint.y());
    }

    dynamic_cast<PHScene*>(scene())->updateActions();
    eventPressPoint.setX(event->scenePos().x());
    eventPressPoint.setY(event->scenePos().y());
    event->accept();
}

void GSort::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

    setCursor(QCursor(Qt::OpenHandCursor));

    //save new position of inital point
    int centerX1=leftTopCorner->x();
    int centerY1=leftTopCorner->y();

    int centerX2;
    int centerY2;
    int distanceHeightMin=0;

    map<string, GSortPtr> listGSorts = dynamic_cast<PHScene*>(scene())->getGSorts();

    bool resetPosition = false;
    for(auto &s : listGSorts){

        centerX2=s.second.get()->getLeftTopCornerPoint()->x();
        centerY2=s.second.get()->getLeftTopCornerPoint()->y();

	

       if(s.second.get()->getSort()->getName()!=sort->getName()){
           distanceHeightMin=getSizeRect()->height()/2.0 + s.second.get()->getSizeRect()->height()/2.0 + defaultDistance;
	   bool xCond = abs(centerX1-centerX2)<getSizeRect()->width()+defaultDistance;
           bool yCondInf = (centerY1 > centerY2 - defaultDistance) && (centerY1 < centerY2 + s.second.get()->getSizeRect()->height()+defaultDistance);
           bool yCondSup = (centerY2 > centerY1 - defaultDistance) && (centerY2 < centerY1 + getSizeRect()->height() + defaultDistance);
           bool yCond = yCondInf || yCondSup;
           
	   if(xCond && yCond ){
               resetPosition = true;
           }
        }
    }

        //coordinates have the initial value
    if(resetPosition){
	for(GProcessPtr &p: gProcesses){
	    p->getCenterPoint()->setX(p->getCenterPoint()->x() + initPosPoint.x() - x());
	    p->getCenterPoint()->setY(p->getCenterPoint()->y() + initPosPoint.y() - y());
   	}
        leftTopCorner->setX(leftTopCorner->x() + initPosPoint.x() - x());
        leftTopCorner->setY(leftTopCorner->y() + initPosPoint.y() - y());
        setX(initPosPoint.x());
        setY(initPosPoint.y());
	dynamic_cast<PHScene*>(scene())->updateActions();
    }

    event->accept();
}

void GSort::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    if(simpleDisplay){
    	simpleDisplay = false;
    }else{
	simpleDisplay = true;
    }
    dynamic_cast<PHScene*>(scene())->updateActions();
    event->accept();
}

// context menu event handler
void GSort::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {

    // if other mouse buttons are pressed, do nothing
    if (QApplication::mouseButtons() != Qt::RightButton) {
        event->ignore();
        return;
    }
}

// palette managementsizeRect

int GSort::paletteIndex = 0;

// TODO see QPalette class?
std::vector<QColor> GSort::palette = 	{	QColor(181,137,0)
										, 	QColor(220,50,47)
										, 	QColor(211,54,130)
										, 	QColor(108,113,196)
										, 	QColor(38,139,210)
										, 	QColor(42,161,152)
										, 	QColor(133,153,0)
										};

QColor* GSort::makeColor () {
	paletteIndex = (paletteIndex + 1) % palette.size();
	return &(palette[paletteIndex]);
}


// getters

QGraphicsRectItem* GSort::getRect() { return this->_rect; }

SortPtr GSort::getSort() { return this->sort; }

GVNode GSort::getNode() { return this->node; }

QGraphicsTextItem* GSort::getText() { return this->text; }

QPoint GSort::geteventPressPoint() { return this->eventPressPoint; }

QPoint* GSort::getLeftTopCornerPoint() {return this->leftTopCorner;}

QSize* GSort::getSizeRect() { return this->sizeRect;}

void GSort::updatePosition() {

}


void GSort::hide() {
    this->setOpacity(0);
}

void GSort::show() {
    this->setOpacity(1);
}

bool GSort::isVisible() {
    return (this->opacity() == 1);
}

void GSort::setSimpleDisplay(bool isSimpleDisplay){
	this->simpleDisplay = isSimpleDisplay;
}

bool GSort::getSimpleDisplay(){
    return this->simpleDisplay;
}
