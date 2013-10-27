#include <iostream>
#include <QPen>
#include <QColor>
#include <QBrush>
#include <QSizeF>
#include <QTextDocument>
#include <Qt>
#include "GSimpleProcess.h"
#include <math.h>


const int GSimpleProcess::marginZone  = 10;
const int GSimpleProcess::sortName    = 11;

GSimpleProcess::GSimpleProcess(ProcessPtr p, double centerX, double centerY, double width, double height): process(p) {

    display = new QGraphicsItemGroup();

    // ellipse
    ellipse = new QGraphicsEllipseItem (centerX, centerY,
                                        width, height, display);
	ellipse->setPen(QPen(QColor(238,232,213)));
	ellipse->setBrush(QBrush(QColor(238,232,213)));

/* 
    //int margin( (int) ceil(GVSubGraph::sepValue * graphDPI / GVGraph::DotDefaultDPI) );
    int margin = 30;
    marginRect = new QGraphicsRectItem(
                centerX -width/2 - margin,
                centerY - height/2 - margin,
                width + 2*margin,
                height + 2*margin,
                display);
                
    marginRect->setBrush(Qt::NoBrush);
    marginRect->setPen(Qt::NoPen);
    marginRect->setData(marginZone, true);
    marginRect->setData(sortName, process->getSort()->getName().c_str());
    
    // text
    text = new QGraphicsTextItem (QString("%1").arg(process->getNumber()), ellipse);
    text->setDefaultTextColor(QColor(7,54,66));
    text->setPos(centerX, centerY);

    // position the text
	QSizeF textSize = text->document()->size();
	text->setPos(text->x() - textSize.width()/2, text->y() - textSize.height()/2);
*/
}


GSimpleProcess::~GSimpleProcess() {
    delete ellipse;
    delete text;
	delete display;
}


// getters

QGraphicsItem* GSimpleProcess::getDisplayItem (void) { return display; }

QGraphicsEllipseItem* GSimpleProcess::getEllipseItem(){ return ellipse; }

QGraphicsRectItem* GSimpleProcess::getMarginRect() { return this->marginRect; }


bool GSimpleProcess::checkCollisions() {

    QList<QGraphicsItem*> collidItems = marginRect->collidingItems();
    foreach (QGraphicsItem *item, collidItems) {
        // detect margins of GProcess items from other sorts
        if (item->data(marginZone).toBool()) {
            if (item->data(sortName).toString() != process->getSort()->getName().c_str()) {
                return true;
            }
        }
    }

    return false;
}

ProcessPtr* GSimpleProcess::getProcess() { return &(this->process); }
