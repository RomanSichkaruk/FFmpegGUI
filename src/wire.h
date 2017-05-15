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

#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsLineItem>
#include <QTableWidget>

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



class Pad;
class Connectivity;
class Scene;

class Wire : public QObject{
    Q_OBJECT
public:
    /**
     * Constructors and destructor.
     * @param s
     */
    Wire(Scene * s);
    Wire(const Wire& orig);
    virtual ~Wire();
    //--------------------------------------------------------------------------
    
    /**
     * This function returns first of connected pads.
     */
    inline Pad * getIn(){ return padIn;}
    
    /**
     * This function returns second of connected pads. 
     */
    inline Pad * getOut(){ return padOut;}
    //--------------------------------------------------------------------------

    /**
     * This function returns line graphic object.
     */
    inline QGraphicsLineItem * getLine(){ return line;}
    //--------------------------------------------------------------------------
    
    /**
     * This function updates info in dynamic inspector table.
     * @param w
     */
    void updateInspector(QTableWidget * w);
    //--------------------------------------------------------------------------
    
    /**
     * This function connects two pads.
     * Creates graphical representation of line and places it on scene.
     * @param p1 is first connected pad
     * @param p2 is second connected pad
     * @param c  is connectivity, that holds this connection
     */
    void setConnection(Pad*p1, Pad*p2, Connectivity * c);
    //--------------------------------------------------------------------------
private:
    QGraphicsLineItem *line;  // Graphic object representing line on scene.
    Pad * padIn;              // First connected pad.
    Pad * padOut;             // Second connected pad.
    Scene * scene;            // Scene to place line on. 
    
public:
    /**
     * This function updates line. 
     * Called if position of connected pad is changed. 
     */
    void updateLine();
    //--------------------------------------------------------------------------
};

#endif /* WIRE_H */

