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
    QString path;
    QString filter; 
    int video_stream;
    AVFormatContext *   format;
    AVCodecContext  *   decoder_ctx;
    
    AVFilterContext *   buffersink_ctx;
    AVFilterContext *   buffersrc_ctx;
    AVFilterGraph   *   filter_graph;
    AVFilterInOut   *   inputs;
    AVFilterInOut   *   outputs; 
    
    
    void loadContext();
    //--------------------------------------------------------------------------
    
    void initializeFilterGraph();
    //--------------------------------------------------------------------------
    
    void createEndPoints();
    //--------------------------------------------------------------------------
};

#endif /* PIPELINE_H */

