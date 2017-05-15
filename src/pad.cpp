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

void Pad::setParent(Filter* f){
    this->setParentItem(f);
    parent = f;
}

void Pad::mousePressEvent(QGraphicsSceneMouseEvent *event){
    selected = !selected;
    this->scene()->update();
    QGraphicsItem::mousePressEvent(event);
}
QRectF Pad::boundingRect() const{
    return padRect;
}

QVariant Pad::itemChange(GraphicsItemChange change, const QVariant & value){
    if(change == ItemPositionChange){
        padRect = QRectF(value.toPointF(), padRect.size());
        if(connectivity != NULL)
            connectivity->updateWires(this);
    }
    return QGraphicsItem::itemChange(change, value);
}

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

QString Pad::getParentName() const {
    return this->getParent()->getName();
}
