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
    Pad(const QRectF &r);
    Pad(const Pad& orig);
    virtual ~Pad();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void setParent(Filter * f);
    inline Pad * connectedTo() const {return this->to;}
    void connectTo(Pad * p){ this->to = p;}
    inline bool isSelected(){ return this->selected;}
    inline bool setIsSelected(bool sel){ this->selected=false;}
    inline void setConnectivity(Connectivity * c){connectivity=c;}
    
    inline QString getName() const {return this->name;};
    inline void setName(QString n) { this->name=n; };
    
    inline Filter * getParent() const { return parent; };
    
    QString getParentName() const ;
    
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    Connectivity * connectivity;
    

private:
    Pad * to;
    QString name;
    Filter * parent;
    bool selected;
    QRectF padRect;
    
};

#endif /* PAD_H */

