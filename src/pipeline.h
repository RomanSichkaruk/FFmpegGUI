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

#ifndef PIPELINE_H
#define PIPELINE_H

#include <QString>
#include <QObject>

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

class Pipeline : public QObject {
    Q_OBJECT
public:
    /** 
     * Constructors and destructor
     */
    Pipeline();
    //--------------------------------------------------------------------------
    
    Pipeline(const Pipeline& orig);
    //--------------------------------------------------------------------------
    
    virtual ~Pipeline();
    //--------------------------------------------------------------------------
    
    Pipeline(QString p, QString f);
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns filter graph.
     */
    inline AVFilterGraph   * getGraph() {return this->filter_graph;};
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns path to input file.
     */
    inline const QString & getInPath() const { return this->path;};
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns input AVFormatContext.
     */
    inline AVFormatContext *  getFormat() const { return this->format;};
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns input AVCodecContext.
     */
    inline AVCodecContext *  getCodec() const { return this->decoder_ctx;};
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns number of video stream in input video.
     */
    inline int getVideoStream() const { return this->video_stream;};
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns buffer context from where
     *  frames are read by filter graph.
     */
    inline AVFilterContext* getBSrc() const { return this->buffersrc_ctx;};
    //--------------------------------------------------------------------------
    
    /**
     *  This function returns buffer context where
     *  frames are holden after processing by filter graph.
     */
    inline AVFilterContext* getBSink() const { return this->buffersink_ctx;};
    //--------------------------------------------------------------------------
    
public slots:
    /**
     * Slot for safe destruction of pipeline.
     */
    void deleteLater2();
    //--------------------------------------------------------------------------
    
private:
    QString path;       // Path to input file
    QString filter;     // Computed text representation of filter graph
    int video_stream;   // Number of video stream
    AVFormatContext *   format;     // Input format context
    AVCodecContext  *   decoder_ctx;// Decoder context
    
    AVFilterContext *   buffersink_ctx; // Buffer from which filter graph takes frames
    AVFilterContext *   buffersrc_ctx;  // Buffer in which filter graph returns frames
    AVFilterGraph   *   filter_graph;   // Filter graph
    AVFilterInOut   *   inputs;         // Filter graph inputs
    AVFilterInOut   *   outputs;        // Filter graph outputs
    
    /**
     *  Opens input file.
     *  Creates and setups format context.
     *  Initializes decoder.
     */
    void loadContext();
    //--------------------------------------------------------------------------
    
    /**
     * Initializes filter graph depending on filter string.
     * Connects its outputs and inputs.
     */
    void initializeFilterGraph();
    //--------------------------------------------------------------------------
    
    /**
     * Initialize filter graph inputs and ouputs.
     */
    void createEndPoints();
    //--------------------------------------------------------------------------
};

#endif /* PIPELINE_H */

