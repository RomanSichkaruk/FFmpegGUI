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

#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QWheelEvent>
#include <QGraphicsSceneEvent>
#include <math.h>

extern "C"{
#define __STDC_FORMAT_MACROS
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/timestamp.h"    
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"    
#include "libswscale/swscale.h"
}

class View : public QGraphicsView{
    Q_OBJECT
public:
    /** 
     * Constructors and destructor.
     */
    View();
    View(const View& orig);
    virtual ~View();
    //--------------------------------------------------------------------------
    
    /**
     * This function zooms view. 
     * Sets new center of a view.
     * @param factor - says how much to zoom
     * @param centerPoint - is point to be new view center
     */
    void zoom(qreal factor, QPointF centerPoint);
    //--------------------------------------------------------------------------
protected:
    /**
     * This function is mouse wheel movement event handler.
     * If Ctrl button is also pressed, runs view zooming.
     * @param e
     */
    void wheelEvent ( QWheelEvent * event );
    //--------------------------------------------------------------------------
    
private:
    int scaling;
    
public slots:
    /**
     * This is mouse moved event handler.
     * @param mouseEvent
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    //--------------------------------------------------------------------------
    
    /**
     * This is mouse pressed event handler.
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //--------------------------------------------------------------------------
    
    /**
     * This is mouse released event handler.
     * @param mouseEvent
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    //--------------------------------------------------------------------------
    
    /**
     * Slot. Is called on each tick of animation timer.
     * Scales a little bit.
     * @param x - say how much to scale
     */
    void scalingTime(qreal x);
    //--------------------------------------------------------------------------
    
    /**
     * This function is called when zooming is finished.
     * Releases zoom animation timer.
     */
    void animFinished();
    //--------------------------------------------------------------------------
};

#endif /* VIEW_H */

