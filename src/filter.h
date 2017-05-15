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
#ifndef FILTER_H
#define FILTER_H

#include "pad.h"
#include <QString>
#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QFont>
#include "player.h"
#include <QTableWidget>
#include "filtersWidget.h"
#include <QLineEdit>
#include <QTextBrowser>

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



typedef enum {
    STRING,
    INT,
    DOUBLE,
    RATIONAL
}ParametersType;


class FilterParameter{
    
public:
    
    FilterParameter( ParametersType t,
                            double mn,
                            double mx,
                            QString n,
                            QString d,
                            QString v )
    {
        this->type=t;
        this->min=mn;
        this->max=mx;
        this->name=n;
        this->descr=d;
        this->value=v;
    };
    
    FilterParameter(const FilterParameter & fp){
        this->type=fp.type;
        this->min=fp.min;
        this->max=fp.max;
        this->name=fp.name;
        this->descr=fp.descr;
        this->value=fp.value;
    };
    
    ~FilterParameter(){};
    
    ParametersType type;
    double min;
    double max;
    QString name;
    QString descr;
    QString value;
};

class Connectivity;
class Filter : public QGraphicsItem{
public:
    Filter(QString name, QSize inOut, QPointF pos);
    Filter(const Filter& orig);
    virtual ~Filter();
    QRectF boundingRect() const;
    void setConnectivity(Connectivity *c);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    
    inline QString getName() { return this->name;} 
    inline QString getPath() { return params[0]->value;} 
    inline void setName(const QString & n) { this->name = n;} 
    inline Pad * getInPad() { return this->in;}
    inline Pad * getInPad2() { return this->in2;}
    inline Pad * getOutPad() { return this->out;}
    inline Pad * getOutPad2() { return this->out2;}
    
    void initializeParams();
    void setCtx(AVFilterContext * context);
    
    QList<FilterParameter*> params;
    AVFilterContext * ctx;
    
public slots:     
    void changeParameters(int r, int c, QTableWidget * widg);
private:
    bool selected;
    Pad * in,*in2,*out,*out2;
    QString name;
    QRectF filterRect;
    QString description;
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) ;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) ;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) ;
    

};

#endif /* FILTER_H */

