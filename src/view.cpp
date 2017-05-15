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



#include <QGraphicsView>

#include "view.h"
#include <QMimeData>
#include <QDrag>
#include <QDebug>
#include <QTimeLine>

/** 
 * Constructors and destructor.
 */

View::View() {
    setAcceptDrops(true);
    scaling=0;
    this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

View::View(const View& orig) {}
View::~View() {}
//------------------------------------------------------------------------------

/**
 * This function is mouse wheel movement event handler.
 * If Ctrl button is also pressed, runs view zooming.
 * @param e
 */

void View::wheelEvent ( QWheelEvent * e)
{
    if ((e->modifiers() & Qt::ControlModifier)){ 
        
        int numSteps = e->delta() / 60;
        
        scaling += numSteps;
        
        if (scaling * numSteps < 0) 
                    scaling = numSteps;

        QTimeLine *anim = new QTimeLine(350, this);
        anim->setUpdateInterval(10);

        connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
        connect(anim, SIGNAL (finished()), SLOT (animFinished()));
        anim->start();
    }
    else{
        QGraphicsView::wheelEvent(e);
    }
}
//------------------------------------------------------------------------------

/**
 * This function zooms view. 
 * Sets new center of a view.
 * @param factor - says how much to zoom
 * @param centerPoint - is point to be new view center
 */

void View::zoom(qreal factor, QPointF centerPoint)
{
    scale(factor, factor);
    centerOn(centerPoint);
}
//------------------------------------------------------------------------------

/**
 * This is mouse moved event handler.
 * @param mouseEvent
 */

void View::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    this->update();
}
//------------------------------------------------------------------------------

/**
 * This is mouse pressed event handler.
 * @param mouseEvent
 */

void View::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->scroll(500,500);
    event->accept();
    this->update();
}
//------------------------------------------------------------------------------

/**
 * This is mouse released event handler.
 * @param mouseEvent
 */

void View::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->update();
}
//------------------------------------------------------------------------------

/**
 * Slot. Is called on each tick of animation timer.
 * Scales a little bit.
 * @param x - say how much to scale
 */

void View::scalingTime(qreal x)
{
    qreal factor = 1.0 + qreal(scaling) / 300.0;
    scale(factor, factor);
}
//------------------------------------------------------------------------------

/**
 * This function is called when zooming is finished.
 * Releases zoom animation timer.
 */

void View::animFinished()
{
    if (scaling > 0)
        scaling--;
    else
        scaling++;
    
    sender()->~QObject();
}
//------------------------------------------------------------------------------
