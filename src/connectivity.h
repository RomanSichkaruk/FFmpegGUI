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

#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <QList>
#include <QHash>
#include <QPointF>
#include <QTextStream>
#include "externs.h"
#include <QGraphicsScene>
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
class Wire;
class Pad;

class Connectivity {
public:
    /**
     *  Constructors
     */
    Connectivity();
    Connectivity(const Connectivity& orig);
    //--------------------------------------------------------------------------
    
    /**
     * This function adds new wire to list of all wires.
     * @param w is a wire to be added
     */
    void addWire(Wire * w);
    //--------------------------------------------------------------------------
    
    /** 
     * This funcction updaptes location of a wire on scene.
     * Wire to be updated is a wire that has one of pads corresponding 
     * to pad sent as an argument.
     * @param p that is connected by wire
     */
    void updateWires(Pad * p);
    //--------------------------------------------------------------------------
    
    /**
     * Returns list of wires holden by this connectivity.
     * @return 
     */
    inline QList<Wire*> getWires(){return wires;}
    //--------------------------------------------------------------------------
    
    /**
     * Computes string for ffplay.
     * @return 
     */
    QString computeString();
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns path to input file.
     */
    QString inputPath();
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns pad of an input buffer.
     */
    Pad * getInput();
    //--------------------------------------------------------------------------
    
    /**
     * Deletes wire from a list of all wires.
     * @param w wire to be deleted.
     */
    void removeWire(Wire * w);
    //--------------------------------------------------------------------------
    
    /**
     * Parses one part - sequension of filters that dont have 
     * spliting or overlaying filters (or ends with them)
     * and are connected one by one.
     * @param pad - first input pad of a sequension.
     * @param cmd - string with command to be appended
     * @return list of all output pads
     */
    QList <Pad *> parseOnePart(Pad* pad, QString * cmd);
    //--------------------------------------------------------------------------
    
    /**
     * Removes all wires, that have of of pads sent as arguments in
     * their connected pads. Called on filter deletion.
     * @param p1 - first (input) pad of a filter.
     * @param p2 - second (output) pad of a filter.
     */
    void removeWires(Pad * p1, Pad * p2);
    //--------------------------------------------------------------------------
    
    /**
     *  Destructor
     */
    virtual ~Connectivity();
    //--------------------------------------------------------------------------
private:
    QList<Wire*> wires; // List of actual wires
};

#endif /* CONNECTIVITY_H */

