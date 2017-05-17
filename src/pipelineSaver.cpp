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

#include "pipelineSaver.h"
#include "pipelineParser.h"

/**
 *  Constructors and destructor
 */

PipelineSaver::PipelineSaver(Pipeline *p,QString out, QString e) : pipe(p), 
                                                ofilename(out), encoder_name(e) {
}
PipelineSaver::PipelineSaver(){};
PipelineSaver::PipelineSaver(const PipelineSaver& orig) {
}

PipelineSaver::~PipelineSaver() {
}
//------------------------------------------------------------------------------

/**
 *  Opens output video container.
 *  Creates its context.
 *  Copies audio and video stream to it.
 *  Setups encoders for each stream.
 */

int PipelineSaver::open_output_file()
{
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx,* enc_ctx;
    av_log_set_level(AV_LOG_DEBUG);
    AVCodec *encoder;
    int ret;
    unsigned int i;
    ofmt_ctx = NULL;
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, ofilename.toStdString().c_str());
    if (!ofmt_ctx) {
        throw std::runtime_error("Could not create output context");
    }
    
    for (i = 0; i < pipe->getFormat()->nb_streams; i++) {
        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            throw std::runtime_error("Failed allocating output stream");
            return AVERROR_UNKNOWN;
        }
        in_stream = pipe->getFormat()->streams[i];
        dec_ctx = pipe->getFormat()->streams[i]->codec;
       
        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            
            encoder = NULL;
            while((encoder=av_codec_next(encoder))){
                if(av_codec_is_encoder(encoder)){
                    if(QString(encoder->long_name).compare(encoder_name)==0){
                        break;
                    }
                }
            }
            
            if (!encoder) {
                throw std::runtime_error("Necessary encoder not found");
                return AVERROR_INVALIDDATA;
            }
            
            encoder_ctx = avcodec_alloc_context3(encoder);
            
            if (!encoder_ctx) {
                throw std::runtime_error("Failed to allocate the encoder context");
                return AVERROR(ENOMEM);
            }
            
            AVFilterContext * flt;
            for(int i = 0; i < pipe->getGraph()->nb_filters; i++){
                if(QString(pipe->getGraph()->filters[i]->name).contains("out")){
                    flt=pipe->getGraph()->filters[i];
                    break;
                }
            }
            
            encoder_ctx->height = flt->inputs[0]->h;
            encoder_ctx->width = flt->inputs[0]->w;
            encoder_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
            encoder_ctx->pix_fmt = encoder->pix_fmts[0];
            encoder_ctx->time_base = av_inv_q(dec_ctx->framerate);

            if (avcodec_open2(encoder_ctx, encoder, NULL) < 0) {
                throw std::runtime_error("Cannot open video encoder for stream");
                return ret;
            }
            
            ret = avcodec_copy_context(out_stream->codec, encoder_ctx);
            if (ret < 0) {
                throw std::runtime_error("Failed to copy encoder parameters to output stream0");
                return ret;
            }
            
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                encoder_ctx->flags |= (1 << 22);
            out_stream->time_base = encoder_ctx->time_base;
            
        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            throw std::runtime_error("Elementary stream is of unknown type, cannot proceed");
            return AVERROR_INVALIDDATA;
        } else {
            ret = avcodec_copy_context(ofmt_ctx->streams[i]->codec, in_stream->codec);
            if (ret < 0) {
                throw std::runtime_error("Copying parameters for stream failed");
                return ret;
            }
            out_stream->time_base = in_stream->time_base;
        }
    }
    
    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, ofilename.toUtf8(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            throw std::runtime_error("Could not open output file");
            return ret;
        }
    }
    
    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        throw std::runtime_error("Error occurred when opening output file");
        return ret;
    }
    return 0;
}
//------------------------------------------------------------------------------

/**
 * Encodes and writes filtered frame to output container
 * @param filt_frame
 * @param stream_index
 * @param got_frame - indicates errors.
 * @return 
 */

int PipelineSaver::encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame)
{
    int ret;
    int got_frame_local=1;
    AVPacket enc_pkt;
    
  
    if (!got_frame)
        got_frame = &got_frame_local;
    
    av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
    /* encode filtered frame */
    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);
    ret = avcodec_encode_video2(encoder_ctx, &enc_pkt, filt_frame, got_frame);
    
    av_frame_free(&filt_frame);
    
    if (ret < 0)
        return ret;
    if (!(*got_frame))
        return 0;
    /* prepare packet for muxing */
    enc_pkt.stream_index = stream_index;
    av_packet_rescale_ts(&enc_pkt,
                         encoder_ctx->time_base,
                         ofmt_ctx->streams[stream_index]->time_base);
    av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
    
    ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
    return ret;
}
//------------------------------------------------------------------------------

/**
 * Encodes and writes frame to output container
 * @param frame
 * @param stream_index
 * @return 
 */

int PipelineSaver::filter_encode_write_frame(AVFrame *frame, unsigned int stream_index)
{
    int ret=0;
    AVFrame *filt_frame;
    av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
    
    ret = av_buffersrc_add_frame_flags(pipe->getBSrc(), frame, 0);
    if (ret < 0) {
        throw std::runtime_error("Failed to copy encoder parameters to output stream5");
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }
    
    while (1) {
        filt_frame = av_frame_alloc();
        if (!filt_frame) {
            throw std::runtime_error("Failed to copy encoder parameters to output stream6");
            ret = AVERROR(ENOMEM);
            break;
        }
        av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(pipe->getBSink(),
                filt_frame);
        
        if (ret < 0) {
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            
            av_frame_free(&filt_frame);
            break;
        }
        
        filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
        
        ret = encode_write_frame(filt_frame, stream_index, NULL);
        
        if (ret < 0){
            throw std::runtime_error("Failed to copy encoder parameters to output stream7");
            break;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------

/**
 * Flushes stream encoder
 * @param stream_index
 * @return 
 */

int PipelineSaver::flush_encoder(unsigned int stream_index)
{
    int ret;
    int got_frame=1;
    if (!(encoder_ctx->codec->capabilities &
                (1 <<  5)))
        return 0;
    while (1) {
        av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        ret = encode_write_frame(NULL, stream_index, &got_frame);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}
//------------------------------------------------------------------------------

/**
 * Saves pipeline to TXT file.
 * @param c         - connectivity with used wires
 * @param filename  - name of an output file
 */

void PipelineSaver::saveTxt(Connectivity *c,const QString& filename) {
        QString streamString("-i ");
        streamString += c->inputPath();
        streamString += " -vf '";
        streamString += c->computeString();
        streamString += "'";
        
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        QTextStream stream(&file);
        stream << streamString;
        file.close();
}
//------------------------------------------------------------------------------

/**
 * Saves pipeline to XML file.
 * @param scene     - current scene
 * @param c         - connectivity with used wires
 * @param filename  - name of an output file
 */

void PipelineSaver::saveXml(Scene * scene, Connectivity *c,const QString& filename) {
    
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("FILTERS");
    for(auto f : scene->filters){
        xmlWriter.writeStartElement("FILTER");
        xmlWriter.writeTextElement("name", f->getName());
        xmlWriter.writeTextElement("xpos", QString("%1").arg(f->sceneBoundingRect().topLeft().x()));
        xmlWriter.writeTextElement("ypos", QString("%1").arg(f->sceneBoundingRect().topLeft().y()));

        if(f->getInPad()){
            xmlWriter.writeStartElement("PAD");
            xmlWriter.writeTextElement("which", "1");
            xmlWriter.writeTextElement("name", QString("%1").arg(f->getInPad()->getName()));
            xmlWriter.writeEndElement();
        }
        if(f->getInPad2()){
            xmlWriter.writeStartElement("PAD");
            xmlWriter.writeTextElement("which", "2");
            xmlWriter.writeTextElement("name", QString("%1").arg(f->getInPad2()->getName()));
            xmlWriter.writeEndElement();
        }
        if(f->getOutPad()){
            xmlWriter.writeStartElement("PAD");
            xmlWriter.writeTextElement("which", "3");
            xmlWriter.writeTextElement("name", QString("%1").arg(f->getOutPad()->getName()));
            xmlWriter.writeEndElement();
        }
        if(f->getOutPad2()){
            xmlWriter.writeStartElement("PAD");
            xmlWriter.writeTextElement("which", "4");
            xmlWriter.writeTextElement("name", f->getOutPad2()->getName());
            xmlWriter.writeEndElement();
        }
        for(auto p : f->params){
            if(!p->value.isEmpty()){
                xmlWriter.writeStartElement("PARAMETER");
                xmlWriter.writeTextElement("name", p->name);
                xmlWriter.writeTextElement("value", p->value);
                xmlWriter.writeEndElement();
            }
        }
        xmlWriter.writeEndElement();
    }
    for(auto wire : c->getWires()){
        xmlWriter.writeStartElement("WIRE");
        xmlWriter.writeTextElement("pad1", wire->getIn()->getName());
        xmlWriter.writeTextElement("pad2", wire->getOut()->getName());
        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();

    file.close();
}
//------------------------------------------------------------------------------

/**
  * Saves pipeline processed video to file.
  * Needs to be improved.
  * Works just with some special formats and coders
  * @return 
  */

int PipelineSaver::save()
{
    int ret;
    AVPacket packet;
    packet.data = NULL;
    packet.size = 0;
    AVFrame *frame = NULL;
    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int got_frame;
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);
    
    av_register_all();
    avfilter_register_all();
    
    if ((ret = open_output_file()) < 0)
        throw std::runtime_error("Failed to copy encoder parameters to output stream8");
    
    while (1) {
        if ((ret = av_read_frame(pipe->getFormat(), &packet)) < 0){
            break;
        }
        
        stream_index = packet.stream_index;
        type = pipe->getFormat()->streams[packet.stream_index]->codec->codec_type;
        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
                stream_index);
        if (type == AVMEDIA_TYPE_VIDEO) {
            av_log(NULL, AV_LOG_DEBUG, "Going to reencode&filter the frame\n");
            frame = av_frame_alloc();
            if (!frame) {
                ret = AVERROR(ENOMEM);
                break;
            }
            av_packet_rescale_ts(&packet,
                                 pipe->getFormat()->streams[stream_index]->time_base,
                                 pipe->getFormat()->streams[stream_index]->codec->time_base);
            dec_func = avcodec_decode_video2;
            ret = dec_func(pipe->getFormat()->streams[stream_index]->codec, frame,
                    &got_frame, &packet);
            if (ret < 0) {
                av_frame_free(&frame);
                throw std::runtime_error("Failed to copy encoder parameters to output stream10");
                av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
                break;
            }
            if (got_frame) {
                frame->pts = av_frame_get_best_effort_timestamp(frame);
                
                ret = filter_encode_write_frame(frame, stream_index);
                
                av_frame_free(&frame);
                if (ret < 0)
                    goto end;
            } else {
                av_frame_free(&frame);
            }
        } 
        else {
            av_packet_rescale_ts(&packet,
                                 pipe->getFormat()->streams[stream_index]->time_base,
                                 ofmt_ctx->streams[stream_index]->time_base);
            ret = av_interleaved_write_frame(ofmt_ctx, &packet);
            if (ret < 0)
                goto end;
        }
        av_packet_unref(&packet);
    }
    
    for (i = 0; i < pipe->getFormat()->nb_streams; i++) {
        av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
        if (pipe->getFormat()->streams[i]->codec->codec_type!= AVMEDIA_TYPE_VIDEO)
            continue;
        ret = filter_encode_write_frame(NULL, i);
        
        if (ret < 0) {
            throw std::runtime_error("Failed to copy encoder parameters to output stream11");
            av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
            goto end;
        }
        
        ret = flush_encoder(i);
        if (ret < 0) {
            throw std::runtime_error("Failed to copy encoder parameters to output stream12");
            av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
            goto end;
        }
    }
    av_write_trailer(ofmt_ctx);
end:
    av_packet_unref(&packet);
    av_frame_free(&frame);
    if (ofmt_ctx && !(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    return ret ? 1 : 0;
}
//------------------------------------------------------------------------------