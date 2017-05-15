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

#include "scene.h"

/**
 *  Map with filter input/output pads count depending on its name.
 */

QMap<QString, QSize> Scene::filtersSize;
//------------------------------------------------------------------------------

/**
 *  Constructors and destructor
 */

Scene::Scene(Connectivity * c) : connectivity(c){}
Scene::Scene(const Scene& orig){}
Scene::~Scene(){}
//------------------------------------------------------------------------------

/**
 * Drag entered event handler
 */

void Scene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(true);
    update();
}
//------------------------------------------------------------------------------

/**
 * Drag left event handler.
 */

void Scene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    update();
}
//------------------------------------------------------------------------------

/**
 * Drag droped event handler.
 * Creates filter depending on MimeData with filter name.
 */

void Scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QString filterName(event->mimeData()->text());
    
    if ( filterName == "input"  || filterName == "output" )
    {
        for(auto f : filters)
            if(f->getName().compare(filterName) == 0) return;
    }
    
    QPointF pos(event->scenePos()-QPointF(100,25));
    createNewFilter(filterName, pos);
    
}
//------------------------------------------------------------------------------

/**
 *  Creates new filter and adds it to scene.
 */

void Scene::createNewFilter(const QString & name, QPointF position){
    Filter * f = new Filter(name, filtersSize[name], position);
    f->setConnectivity(connectivity);
    filters.push_back(f);
    this->addItem(f);  
    this->update();
}
//------------------------------------------------------------------------------
