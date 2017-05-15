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
#ifndef PIPELINESAVER_H
#define PIPELINESAVER_H

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

#include "pipeline.h"
#include "scene.h"
#include "connectivity.h"

class PipelineSaver {
public:
    PipelineSaver();
    PipelineSaver(Pipeline *p,QString out, QString e);
    PipelineSaver(const PipelineSaver& orig);
    virtual ~PipelineSaver();
    
    int open_output_file();
    int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame);

    int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index);
    int flush_encoder(unsigned int stream_index);
    int save();
    void saveXml(Scene * scene, Connectivity *c,const QString& filename);
    void saveTxt(Connectivity *c,const QString& filename);
    AVCodecContext *encoder_ctx;
    Pipeline *pipe;
    QString ofilename;
    QString encoder_name;
private:
    AVFormatContext *ofmt_ctx;
};

#endif /* PIPELINESAVER_H */

