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

#include "player.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <exception>
#include <QThread>

/**
 * This slot is called when previewing button is pressed, 
 * to pause preview.
 */
void Player::stopedSlot(){
        stoped=!stoped;
}
//------------------------------------------------------------------------------

/**
 *  Constructor 
 */

Player::Player(Pipeline * p, QPushButton * b) : pipeline(p), btn(b){
   
    av_register_all();
    avfilter_register_all();
    avcodec_register_all();
    stoped=false;
    opened=false; 
    connect( btn, SIGNAL(pressed()), this, SLOT(stopedSlot()) );
}

//------------------------------------------------------------------------------

/**
 * Destructor 
 */

Player::~Player() {
    delete btn;
}
//------------------------------------------------------------------------------

/**
 * This slot is called when thread for player is ready to work,
 * Starts reading and processing frames from AVFormatCtx container and
 * sends them to previw QPushButton instance.
 */

void Player::runPlaying(){
    
    AVFrame * frame = av_frame_alloc();
    AVFrame * filt_frame = av_frame_alloc();
    AVPacket packet;
    int ret=0;
    
    while(1)
    {
        if(av_read_frame(pipeline->getFormat(), &packet) < 0){
            av_free_packet(&packet);
            break;
        }
        if(packet.stream_index==pipeline->getVideoStream())
        {
            av_frame_unref(frame);
            int frameFinished;
            if(!(avcodec_decode_video2( pipeline->getCodec(), frame, 
                                        &frameFinished, &packet) > 0))
            {
                av_free_packet(&packet);
                continue;
            }
            
            if(frame->format == -1){
                 av_free_packet(&packet);
                continue;
            }
            
            if(frameFinished >= 0){
                frame->pts = av_frame_get_best_effort_timestamp(frame);
               
                if (av_buffersrc_add_frame_flags(pipeline->getBSrc(), frame, 
                                            AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                        throw std::runtime_error("Failed to create graph");
                         av_free_packet(&packet);
                    continue   ; 
                }
                if(stoped){
                    QEventLoop l;
                    connect( btn, SIGNAL( pressed() ), &l, SLOT(quit()) );
                    l.exec();
                }
                QApplication::processEvents();
                emit frameFinishedSig();
                
                QEventLoop l2;
                connect( this, SIGNAL( procDone() ), &l2, SLOT( quit() ) );
                l2.exec();
                
                ret = av_buffersink_get_frame(pipeline->getBSink(), filt_frame);
               
                QTimer t;
                t.start((int) (pipeline->getCodec()->time_base.num*1000/
                                          pipeline->getCodec()->time_base.den));
                QEventLoop l;
                connect( &t, SIGNAL( timeout() ), &l, SLOT( quit() ) );
                l.exec();
                
                if(!opened){
                    emit sendingSize(QSize(filt_frame->width, filt_frame->height));
                  //  initializeButton(filt_frame->width, filt_frame->height);
                    opened = true;
                }
                else if(!btn->isVisible()) break;
               // else if(!btn->isVisible()) break;
                emit sendingImage(frameToQImage(filt_frame));
              //  btn->setIconSize(QSize(btn->width()-15, btn->height()-15));
               // picture = QIcon (QPixmap::fromImage(frameToQImage(filt_frame)
                                              //   ).scaledToWidth(btn->width()));
               // btn->setIcon(picture);
                
                av_frame_unref(filt_frame);
                av_frame_unref(frame);
            }
        }
        av_free_packet(&packet);
    }

    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    emit playFinished();
    emit playFinished2(this);
}
//------------------------------------------------------------------------------

/**
 * Initializes preview button defaults.
 * @param w width of a button
 * @param h height of a button
 */

void Player::initializeButton(int w, int h){
    
    btn->show();
    int minWidth=300>w?w:300;
    btn->setGeometry(btn->x(),btn->y(), minWidth,minWidth*h/w);
    btn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    btn->setMinimumSize(minWidth, minWidth*h/w);
    btn->setMaximumSize(w,h);
    btn->setFlat(true);
    
}
//------------------------------------------------------------------------------

/**
 * This function converts AVFrame to QImage.
 * @param filt_frame reference to AVFrame to be converted
 * @return QImage
 */

QImage Player::frameToQImage(AVFrame * filt_frame){
    
    SwsContext   *sws_ctx = NULL;
    AVFrame * frameRGB = av_frame_alloc();
    int w = filt_frame->width;
    int h = filt_frame->height;

    uint8_t pomUi[avpicture_get_size(AVPixelFormat::AV_PIX_FMT_RGB24, 
        w,h)];
    avpicture_fill((AVPicture *)frameRGB, pomUi, AV_PIX_FMT_RGB24,
        w, h);

    sws_ctx = sws_getCachedContext(sws_ctx,w, h, pipeline->getCodec()->pix_fmt,
            w, h, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

    sws_scale(sws_ctx, filt_frame->data, filt_frame->linesize, 0, 
        h, frameRGB->data, frameRGB->linesize);


    QImage im = QImage(w,h,QImage::Format_RGB888);

    for(int y=0;y<h;y++)
       memcpy(im.scanLine(y),frameRGB->data[0]+y*frameRGB->linesize[0],w*3);
    
    sws_freeContext(sws_ctx);
    av_frame_free(&frameRGB);
    
    return im;
}
//------------------------------------------------------------------------------
