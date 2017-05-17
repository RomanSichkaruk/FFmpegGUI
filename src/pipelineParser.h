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

#ifndef PIPELINEPARSER_H
#define PIPELINEPARSER_H

#include "pipeline.h"
#include "scene.h"
#include "wire.h"
#include <QXmlStreamWriter>

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

#include <QDebug>

class PipelineParser {
public:
    /**
     *  Constructors and destructor
     */
    PipelineParser(Scene * s);
    PipelineParser(Pipeline * p, Scene * s);
    PipelineParser(Pipeline * p, Scene * s, bool aply);
    PipelineParser(const PipelineParser& orig);
    virtual ~PipelineParser();
    //--------------------------------------------------------------------------
    
    /**
     * Parses pipeline created in GUI.
     */
    void parse();
    //--------------------------------------------------------------------------
    
    /**
     * Loads pipeline from TXT file.
     * @param filename
     */
    void parseTxt(const QString &filename);
    //--------------------------------------------------------------------------
    
    /**
     * Loads pipeline from XML file.
     * @param filename
     */
    void parseXml(const QString &filename);
    //--------------------------------------------------------------------------
    
    
private:
    bool wasCreated;     // Flag, says if pipeline was given, or read from file.
    Pipeline * pipeline; // Current pipeline
    Scene * scene;       // Scene, where graphics items are
    
    /**
     *  Reads WIRE node from XML file.
     *  Creates corresponding wire on scene.
     */
    void setupWire(QXmlStreamReader * Rxml);
    //--------------------------------------------------------------------------
    
    /**
     *  Reads FILTER node from XML file.
     *  Creates corresponding filter on scene.
     */
    void setupFilter(Filter * f, QHash<int, QString> * padNames, 
                     QHash<QString, QString> *parameters);
    //--------------------------------------------------------------------------
    
    /**
     * Updates pads indexes.
     * So new unique index is max index from TXT file + 1
     * @param str
     */
    void updateIndexes(const QString & str);
    //--------------------------------------------------------------------------
    
    /**
     * Creates graphical representation of a pipeline.
     */
    void aplyOnScene();
    //--------------------------------------------------------------------------
    
    /**
     * Setups created filter and sets its context.
     */
    void setFilter(Filter * previous, Pad* pad, AVFilterContext*ctx);
    //--------------------------------------------------------------------------
    
    /**
     * Create new graphics filter item on scene.
     * Uses info from AVFilterContext.
     * @param ctx - all info about filter to be created.
     * @param pos - position to place filter.
     */
    void createFilter(AVFilterContext*ctx, QPointF pos);
    //--------------------------------------------------------------------------
    
    /**
     * Sets filter on scene his filter context from built pipeline.
     * @param fc
     */
    void setFiltContext(AVFilterContext * fc);
    //--------------------------------------------------------------------------
    
};

#endif /* PIPELINEPARSER_H */

