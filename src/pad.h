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
#include <QGraphicsItem>
#include <QRectF>
#include <QGraphicsSceneMouseEvent>
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

#include "filter.h"

#ifndef PAD_H
#define PAD_H

class Filter;
class Connectivity;

class Pad : public QGraphicsItem{
public:
    /**
     * Constructors and destructor.
     * @param r - pad bounding rect
     */
    Pad(const QRectF &r);
    Pad(const Pad& orig);
    virtual ~Pad();
    //--------------------------------------------------------------------------
    
    /**
     * Mouse pressed event handler.
     * @param event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //--------------------------------------------------------------------------
    
    /**
     * Sets pads owner filter.
     * @param f - filter with this pad
     */
    void setParent(Filter * f);
    //--------------------------------------------------------------------------
    
    /**
     * Returns pad, that is connected to this pad.
     * @return 
     */
    inline Pad * connectedTo() const {return this->to;}
    //--------------------------------------------------------------------------
    
    /**
     * Sets this pads connected pad.
     * @param p pad to connect
     */
    void connectTo(Pad * p){ this->to = p;}
    //--------------------------------------------------------------------------
    
    /**
     * Returns true if pad is selected.
     */
    inline bool isSelected(){ return this->selected;}
    //--------------------------------------------------------------------------
    
    /**
     * Sets if pad has to be selected or not.
     */
    inline bool setIsSelected(bool sel){ this->selected=false;}
    //--------------------------------------------------------------------------
    
    /**
     * Sets Connectivity of this pad.
     */
    inline void setConnectivity(Connectivity * c){connectivity=c;}
    //--------------------------------------------------------------------------
    
    /**
     * Returns this pads unique name.
     * @return 
     */
    inline QString getName() const {return this->name;};
    //--------------------------------------------------------------------------
    
    /**
     * Sets this pad a name.
     * @param n
     */
    inline void setName(QString n) { this->name=n; };
    //--------------------------------------------------------------------------
    
    /**
     * Returns owner filter of this pad.
     * @return 
     */
    inline Filter * getParent() const { return parent; };
    //--------------------------------------------------------------------------
    
    /**
     * Returns name of a filter that owns this pad.
     * @return 
     */
    QString getParentName() const ;
    //--------------------------------------------------------------------------
    
    /**
     * Handler of changed activity. Called on pads selection.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    //--------------------------------------------------------------------------
    
    /**
     * Return pads bounding rect.
     * @return 
     */
    QRectF boundingRect() const;
    //--------------------------------------------------------------------------
    
    /**
     * Paitner function. Paints pad on a scene.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    //--------------------------------------------------------------------------
    
    Connectivity * connectivity; // Reference to connectivity 
private:
    Pad * to;           // Reference to pad which is connected to this pad
    QString name;       // Unique pad name
    Filter * parent;    // Reference to filter which owns this pad
    bool selected;      // Flag, says if this pad is selected
    QRectF padRect;     // Pads bounding rect
    
};

#endif /* PAD_H */

