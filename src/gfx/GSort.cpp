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
#include "GSort.h"
#include "Process.h"

const int GSort::marginDefault = 10;

GSort::GSort(SortPtr s, GVNode n, qreal width, qreal height) : QGraphicsRectItem(n.centerPos.x()-width/2, n.centerPos.y()-height/2, width, height),sort(s), node(n) {

    // graphic items set and Actions color
    color = makeColor();
    sizeRect = new QSize(width, height);

    leftTopCorner = new QPoint(n.centerPos.x()-sizeRect->width()/2,n.centerPos.y()-sizeRect->height()/2);

    // rectangle
    _rect = new QGraphicsRectItem(QRectF(*leftTopCorner, *sizeRect),this);
    _rect->setPen(QPen(QColor(7,54,66)));
    _rect->setBrush(QBrush(QColor(7,54,66)));

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
	gProcesses.push_back(make_shared<GProcess>(p, leftTopCorner->x() + GProcess::sizeDefault/2+ marginDefault, leftTopCorner->y()+ currPosYProcess));
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

// mouse press event handler: start "drag"
void GSort::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    // ignore right click
    if (event->button() == Qt::RightButton) {
        event->ignore();
        return;
    }

    setCursor(QCursor(Qt::ClosedHandCursor));


    // record coordinates for drawing item when mouse is moved/released
    initPosPoint.setX(pos().x());
    initPosPoint.setY(pos().y());
    eventPressPoint.setX(event->scenePos().x());
    eventPressPoint.setY(event->scenePos().y());

    event->accept();
}


// mouse move event handler: process "drag"
void GSort::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    // update item position
    setX(initPosPoint.x() + event->scenePos().x() - eventPressPoint.x());
    setY(initPosPoint.y() + event->scenePos().y() - eventPressPoint.y());


    event->accept();
}


// mouse release event handler: process "drop"
void GSort::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

    setCursor(QCursor(Qt::OpenHandCursor));

    // update item position
    setX(initPosPoint.x() + event->scenePos().x() - eventPressPoint.x());
    setY(initPosPoint.y() + event->scenePos().y() - eventPressPoint.y());

    leftTopCorner->setX(x());
    leftTopCorner->setY(y());

    for(GProcessPtr &p: gProcesses){
        p->getCenterPoint()->setX(p->getCenterPoint()->x() + event->scenePos().x() - eventPressPoint.x());
        p->getCenterPoint()->setY(p->getCenterPoint()->y() + event->scenePos().y() - eventPressPoint.y());
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


// palette management

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
