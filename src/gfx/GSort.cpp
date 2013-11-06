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

GSort::GSort(SortPtr s, GVCluster c) :
    QGraphicsRectItem(c.topLeft.x(), c.topLeft.y(), c.width, c.height),
    sort(s), cluster(c) {

    // graphic items set and Actions color
    color = makeColor();

    // rectangle
    _rect = new QGraphicsRectItem(boundingRect(), this);
    _rect->setPen(QPen(QColor(7,54,66)));
    _rect->setBrush(QBrush(QColor(7,54,66)));

    // label
    text = new QGraphicsTextItem (QString(), this);
    text->setHtml(QString::fromStdString("<u>sort " + sort->getName() + "</u>"));
    text->setDefaultTextColor(*color);
    text->setPos(cluster.labelPos.x(), cluster.labelPos.y());
    QSizeF textSize = text->document()->size();
    text->setPos(text->x() - textSize.width()/2, text->y() - textSize.height()/2);

    setCursor(QCursor(Qt::OpenHandCursor));
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);

    // set related GProcesses as children (so they move with this GSort)
    vector<ProcessPtr> processes = sort->getProcesses();
    for (ProcessPtr &process : processes) {
        process->getGProcess()->getDisplayItem()->setParentItem(this);
    }
    paddingTop = processes.back()->getGProcess()->getEllipseItem()->rect().y() - c.topLeft.y();

}

GSort::GSort(SortPtr s, GVNode n) : QGraphicsRectItem(n.centerPos.x()-n.width/8, n.centerPos.y()-n.height/2, n.width/4, n.height),sort(s), node(n) {

    // graphic items set and Actions color
    color = makeColor();
    sizeRect = new QSize(n.width/4, n.height);
    sizeRect->width();
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
    text->setPos(text->x() - textSize.width()/2, text->y());

    setCursor(QCursor(Qt::OpenHandCursor));
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);

    // set related GProcesses as children (so they move with this GSort)
    vector<ProcessPtr> processes = sort->getProcesses();
    int nbProcess = processes.size();
    int currPosYProcess = sizeRect->height()/(1.5*nbProcess);
    for(ProcessPtr &p : processes){
	gProcesses.push_back(make_shared<GProcess>(p, leftTopCorner->x() + sizeRect->width()/2, leftTopCorner->y()+ currPosYProcess, sizeRect->width()-10, sizeRect->height()/(nbProcess+1)-30));
	currPosYProcess+=sizeRect->height()/(nbProcess+1);
    }
    for(GProcessPtr &gp: gProcesses){
	gp->getDisplayItem()->setParentItem(this);
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

GVCluster GSort::getCluster() { return this->cluster; }

QGraphicsTextItem* GSort::getText() { return this->text; }

QPoint GSort::geteventPressPoint() { return this->eventPressPoint; }

QPoint* GSort::getLeftTopCornerPoint() {return this->leftTopCorner;}

QSize* GSort::getSizeRect() { return this->sizeRect;}

void GSort::updatePosition() {

    // compute coords of process n°0 in this sort
    GVNode *gvNode0 = sort->getProcesses().back()->getGProcess()->getNode();
    qreal process0X = gvNode0->centerPos.x() - gvNode0->width/2;
    qreal process0Y = gvNode0->centerPos.y() - gvNode0->height/2;

    // GSort's new absolute coords
    qreal newX = process0X - (cluster.width - gvNode0->width)/2; // processes are centered in sort
    qreal newY = process0Y - paddingTop;

    // update GSort and cluster's coords
    setX(newX - boundingRect().x());
    setY(newY - boundingRect().y());
    cluster.topLeft.setX(boundingRect().x() + pos().x());
    cluster.topLeft.setY(boundingRect().y() + pos().y());

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
