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

#include "wire.h"
#include "pad.h"
#include "connectivity.h"
#include "scene.h"
#include "filter.h"
#include <QDebug>

/**
 * Constructors and destructor.
 * @param s
 */
Wire::Wire(Scene * s) : scene(s){}

Wire::Wire(const Wire& orig) {}

Wire::~Wire() {
    delete line;
}
//------------------------------------------------------------------------------

/**
 * This function connects two pads.
 * Creates graphical representation of line and places it on scene.
 * @param p1 is first connected pad
 * @param p2 is second connected pad
 * @param c  is connectivity, that holds this connection
 */

void Wire::setConnection(Pad*p1, Pad*p2, Connectivity * c){
    line = new QGraphicsLineItem(QLineF(p1->sceneBoundingRect().center(),
                                        p2->sceneBoundingRect().center()));
    
    line->setPen(QPen(Qt::blue, 5, Qt::SolidLine));
    
    padIn = p1;
    padOut = p2;
    
    padIn->connectTo(padOut);
    padOut->connectTo(padIn);
    
    scene->addItem(line);
    line->setFlag(QGraphicsItem::ItemIsSelectable);
    c->addWire(this);
}
//------------------------------------------------------------------------------

/**
 * This function updates line. 
 * Called if position of connected pad is changed. 
 */

void Wire::updateLine(){
    line->setLine(QLineF(padIn->sceneBoundingRect().center(), padOut->sceneBoundingRect().center()));
    line->scene()->update();
}
//------------------------------------------------------------------------------

/**
 * This function updates info in dynamic inspector table.
 * @param w
 */

void Wire::updateInspector(QTableWidget * w){
    w->setRowCount(6);
    w->setColumnCount(2);
    AVFilterContext * ctx=padIn->getParent()->ctx;
    
    if(!ctx) throw std::runtime_error("Preview filter graf first.");
    
    int idx=0;
    if(padIn == padIn->getParent()->getOutPad2()){
        idx=1;
    }
    
    w->setItem(0,0, new QTableWidgetItem("Current pts"));
    w->setItem(0,1, new QTableWidgetItem(QString::number(ctx->outputs[idx]->current_pts)));
    w->setItem(1,0, new QTableWidgetItem("Format"));
    w->setItem(1,1, new QTableWidgetItem(QString(av_get_pix_fmt_name(
                                                    ctx->outputs[idx]->format))));
    w->setItem(2,0, new QTableWidgetItem("Frames passed"));
    w->setItem(2,1, new QTableWidgetItem(QString::number(ctx->outputs[idx]->frame_count)));
    w->setItem(3,0, new QTableWidgetItem("Width"));
    w->setItem(3,1, new QTableWidgetItem(QString::number(ctx->outputs[idx]->w)));
    w->setItem(4,0, new QTableWidgetItem("Height"));
    w->setItem(4,1, new QTableWidgetItem(QString::number(ctx->outputs[idx]->h)));
    w->setItem(5,0, new QTableWidgetItem("Sample rate"));
    w->setItem(5,1, new QTableWidgetItem(QString::number(ctx->outputs[idx]->sample_rate)));
    w->setItem(6,0, new QTableWidgetItem("Time base"));
    w->setItem(6,1, new QTableWidgetItem(QString("%1/%2").arg(
        ctx->outputs[idx]->time_base.num).arg(ctx->outputs[idx]->time_base.den)));
}
//------------------------------------------------------------------------------
