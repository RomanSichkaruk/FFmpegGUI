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

#include "pipeline.h"

/** 
 * Constructors and destructor
 */

Pipeline::Pipeline() {
    format=NULL;
    format=avformat_alloc_context();
}

Pipeline::Pipeline(const Pipeline& orig) {
    
}

Pipeline::~Pipeline() {
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    
    avfilter_graph_free(&filter_graph);
    avcodec_close(decoder_ctx);
    avformat_close_input(&format);
}

Pipeline::Pipeline(QString p, QString f) : path(p), filter(f){
    av_register_all();
    avfilter_register_all();
    avcodec_register_all();
    format=avformat_alloc_context();
    loadContext();
    initializeFilterGraph();
}
//------------------------------------------------------------------------------

/**
 *  Opens input file.
 *  Creates and setups format context.
 *  Initializes decoder.
 */

void Pipeline::loadContext() {
    AVCodec *dec;
    if(avformat_open_input(&format, path.trimmed().toStdString().c_str(), NULL, NULL) < 0)
        throw std::runtime_error("Failed to open video file.");
    
    avformat_find_stream_info(format, NULL);
    int ret = av_find_best_stream(format, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        throw std::runtime_error("Cannot find a video stream in the input file");
        return ret;
    }
    
    for(int i=0; i < format->nb_streams; i++){
        if(format->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            video_stream = i;
            break;
        }
        video_stream = -1;
    }
    if(video_stream==-1)
        throw std::runtime_error("Cannot find a video stream in the input file");
    
    decoder_ctx = format->streams[video_stream]->codec;
 
    if ((ret = avcodec_open2(decoder_ctx, dec, NULL)) < 0) {
        throw std::runtime_error("Cannot open a video decoder");
        return ret;
    }
}
//------------------------------------------------------------------------------

/**
 * Slot for safe destruction of pipeline.
 */

void Pipeline::deleteLater2(){
    this->~Pipeline();
}
//------------------------------------------------------------------------------

/**
 * Initialize filter graph inputs and ouputs.
 */

void Pipeline::createEndPoints(){
    char args[512];
    int ret = 0;
    
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    enum AVPixelFormat pix_fmts[] = { decoder_ctx->pix_fmt, AV_PIX_FMT_NONE};
    snprintf(args, sizeof(args),
            "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
            decoder_ctx->width, decoder_ctx->height, decoder_ctx->pix_fmt,
            decoder_ctx->time_base.num, decoder_ctx->time_base.den,
            decoder_ctx->sample_aspect_ratio.num, decoder_ctx->sample_aspect_ratio.den);
    
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        throw std::runtime_error("Cannot create buffer source");
    }
  
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        throw std::runtime_error("Cannot create buffer sink");
    }
    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        throw std::runtime_error("Cannot set output pixel format");
    }
}
//------------------------------------------------------------------------------
/**
 * Initializes filter graph depending on filter string.
 * Connects its outputs and inputs.
 */
void Pipeline::initializeFilterGraph(){
    int ret = 0;
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    outputs = avfilter_inout_alloc();
    inputs  = avfilter_inout_alloc();
    
    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        throw std::runtime_error("Cannot open a video decoder");
    }
    
    createEndPoints();
    
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;
    
    if ((ret = avfilter_graph_parse_ptr(filter_graph, 
            filter.toStdString().c_str(),
                                    &inputs, &outputs, NULL)) < 0)
        throw std::runtime_error("Failed to create graph");
    
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        throw std::runtime_error("Failed to create graph");
}
//------------------------------------------------------------------------------