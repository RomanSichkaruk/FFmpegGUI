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

#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QDrag>
#include "filter.h"
#include "connectivity.h"
#include "filtersWidget.h"

class Scene : public QGraphicsScene {
    
public:
    static QMap<QString, QSize> filtersSize; // map with filter input/output pads
                                             // count depending on its name
    
    QList<Filter*> filters;                  // list with filters
    
    /**
     *  Constructors and destructor
     */
    Scene(Connectivity * c);
    Scene(const Scene& orig);
    virtual ~Scene();
    //--------------------------------------------------------------------------
    
    /**
     * This function sets table in which filter parameters are displayed.
     * @param f - table to be set
     */
    inline void setFilterParamList(FilterParamsList *f){fw=f;};
    //--------------------------------------------------------------------------
    
    /**
     * This function returns widget with table in which 
     * filter parameters are displayed.
     */
    inline FilterParamsList * getFilterParamList() const { return fw;};
    //--------------------------------------------------------------------------
    
    /**
     * This function returns scene connectivity object.
     */
    inline Connectivity * getConnectivity() const { return connectivity;};
    //--------------------------------------------------------------------------
    
    /**
     *  Creates new filter and adds it to scene.
     */
    void createNewFilter(const QString & name, QPointF position);
    //--------------------------------------------------------------------------
private:
    FilterParamsList *fw;
    Connectivity *  connectivity;
protected:
    /**
     * Drag entered event handler
     */
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) ;
    //--------------------------------------------------------------------------
    
    /**
     * Drag left event handler.
     */
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) ;
    //--------------------------------------------------------------------------
    
    /**
     * Drag droped event handler.
     * Creates filter depending on MimeData with filter name.
     */
    void dropEvent(QGraphicsSceneDragDropEvent *event) ;
    //--------------------------------------------------------------------------
    
    /**
     * Drag moved event handler.
     */
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event)
    {
        event->setAccepted(true);
        update();
    }
    //--------------------------------------------------------------------------
};

#endif /* SCENE_H */

