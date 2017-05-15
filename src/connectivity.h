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
    Connectivity();
    Connectivity(const Connectivity& orig);
    void addWire(Wire * w);
    void updateWires(Pad * p);
    inline QList<Wire*> getWires(){return wires;}
    QString computeString();
    QString inputPath();
    Pad * getInput();
    void removeWire(Wire * w);
    QList <Pad *> parseOnePart(Pad* pad, QString * cmd);
    void removeWires(Pad * p1, Pad * p2);
    virtual ~Connectivity();
private:
    QList<Wire*> wires;
};

#endif /* CONNECTIVITY_H */

