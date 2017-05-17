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


/**
 *  Enum with types of filter parameters.
 */

typedef enum {
    STRING,
    INT,
    DOUBLE,
    RATIONAL
}ParametersType;

//------------------------------------------------------------------------------

/**
 * Class representing filter parameter.
 * @param t  - type of prameter
 * @param mn - minimum supported value
 * @param mx - maximum supported value
 * @param n  - name of a parameter
 * @param d  - description of a parameter
 * @param v  - actual value of a parameter
 */

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
//------------------------------------------------------------------------------

class Connectivity;


class Filter : public QGraphicsItem{
public:
    /**
     * Constructor
     * @param name - is name of a filter
     * @param pos  - is position on a scene where filter will be placed
     */
    Filter(QString name, QSize inOut, QPointF pos);
    //--------------------------------------------------------------------------
    
    /**
     * Copy constructor
     * @param orig
     */
    Filter(const Filter& orig);
    //--------------------------------------------------------------------------
    
    /**
     *  Destructor
     */
    virtual ~Filter();
    //--------------------------------------------------------------------------
    
    /**
     * Returns bounding rect of a filter.
     * @return 
     */
    QRectF boundingRect() const;
    //--------------------------------------------------------------------------
    
    /**
     *  Sets connectivity of a filter.
     */
    void setConnectivity(Connectivity *c);
    //--------------------------------------------------------------------------
    
    /**
     * Overrided function for filter painting.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    //--------------------------------------------------------------------------
    
    /**
     * Called when something is changed with filter. f.e. location, selection.
     * @param change
     * @param value
     * @return 
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);
    //--------------------------------------------------------------------------
    
    /**
     *  Returns filter name.
     */
    inline QString getName() { return this->name;} 
    //--------------------------------------------------------------------------
    
    /**
     * If filter is input, or output buffer, returns path to file.
     * @return 
     */
    inline QString getPath() { return params[0]->value;} 
    //--------------------------------------------------------------------------
    
    /**
     * Sets name of a filter.
     * @param n - name to be a new name of a filter.
     */
    inline void setName(const QString & n) { this->name = n;} 
    //--------------------------------------------------------------------------
    
    /**
     * Returns first input pad. 
     * Null if filter doesnt have one.
     */
    inline Pad * getInPad() { return this->in;}
    //--------------------------------------------------------------------------
        
    /**
     * Returns second input pad. 
     * Null if filter doesnt have one.
     */
    inline Pad * getInPad2() { return this->in2;}
    //--------------------------------------------------------------------------
        
    /**
     * Returns first output pad. 
     * Null if filter doesnt have one.
     */
    inline Pad * getOutPad() { return this->out;}
    //--------------------------------------------------------------------------
        
    /**
     * Returns second output pad. 
     * Null if filter doesnt have one.
     */
    inline Pad * getOutPad2() { return this->out2;}
    //--------------------------------------------------------------------------
    
    /**
     * Finds filter parameters of AVFilter struct with the same name
     * as this filter and set its parameters to this filter.
     */
    void initializeParams();
    //--------------------------------------------------------------------------
    
    /**
     * Sets AVFilterContext of this filter from a current filter graph.
     * @param context
     */
    void setCtx(AVFilterContext * context);
    //--------------------------------------------------------------------------
    
    QList<FilterParameter*> params; // List with filter parameters.
    AVFilterContext * ctx;          // Context from current filter graph.
    
public slots:     
    /**
     *  Slot which is called when user changes one of a parameters.
     *  @param r - row in a table widget where user defined value is.
     *  @param c - col in a table widget where user defined value is.
     *  @param widg - table widget where user edit parameters.
     */
    void changeParameters(int r, int c, QTableWidget * widg);
    //--------------------------------------------------------------------------
private:
    bool selected;              // Flag, says if filter is selected or not
    Pad * in,*in2,*out,*out2;   // Filter pads.
    QString name;               // Name of a filter.
    QRectF filterRect;          // Filter bounding rect.
    QString description;        // Description of a filter.
    
protected:
    /**
     * Mouse pressed event handler.
     * @param event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) ;
    //--------------------------------------------------------------------------
    
    /**
     * Mouse moved event handler.
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) ;
    //--------------------------------------------------------------------------

    /**
     * Mouse released event handler.
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) ;
    //--------------------------------------------------------------------------
    

};

#endif /* FILTER_H */

