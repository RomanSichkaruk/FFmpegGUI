/*********************************************************************************

    FFmpegGUI: filter graph editor based on Qt and FFmpeg
    Copyright (C) 2017 Roman Sichkaruk <romansichkaruk@gmail.com>

    This file is part of FFmpegGUI

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

***********************************************************************************/

#include "pad.h"
#include "filter.h"
#include "connectivity.h"
#include <QBrush>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

/**
 * Constructors and destructor.
 * @param r - pad bounding rect
 */

Pad::Pad(const QRectF &r) {
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    padRect = r;
    selected = false;
    name=QString("[T%1]").arg(++(indexPads));
    connectTo(NULL);
}

Pad::Pad(const Pad& orig) {
}

Pad::~Pad() {
}
//------------------------------------------------------------------------------

/**
 * Sets pads owner filter.
 * @param f - filter with this pad
 */

void Pad::setParent(Filter* f){
    this->setParentItem(f);
    parent = f;
}
//------------------------------------------------------------------------------

/**
 * Mouse pressed event handler.
 * @param event
 */

void Pad::mousePressEvent(QGraphicsSceneMouseEvent *event){
    selected = !selected;
    this->scene()->update();
    QGraphicsItem::mousePressEvent(event);
}
//------------------------------------------------------------------------------

/**
 * Return pads bounding rect.
 * @return 
 */

QRectF Pad::boundingRect() const{
    return padRect;
}
//------------------------------------------------------------------------------

/**
 * Handler of changed activity. Called on pads selection.
 */

QVariant Pad::itemChange(GraphicsItemChange change, const QVariant & value){
    if(change == ItemPositionChange){
        padRect = QRectF(value.toPointF(), padRect.size());
        if(connectivity != NULL)
            connectivity->updateWires(this);
    }
    return QGraphicsItem::itemChange(change, value);
}
//------------------------------------------------------------------------------

/**
 * Paitner function. Paints pad on a scene.
 */

void Pad::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPixmap p;
    
    if(!selected)
        p.load(":/pad1.svg");
    else
        p.load(":/padC.svg");
    
    painter->drawImage(boundingRect(), p.toImage(), p.rect());
    if(connectedTo()){
        painter->setBrush(QBrush(Qt::blue));
        painter->drawEllipse(boundingRect().center(),boundingRect().width()/2-2,
                                                    boundingRect().width()/2-2);
    }
}
//------------------------------------------------------------------------------

/**
 * Returns name of a filter that owns this pad.
 * @return 
 */

QString Pad::getParentName() const {
    return this->getParent()->getName();
}
//------------------------------------------------------------------------------

