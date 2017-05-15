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

#ifndef PLAYER_H
#define PLAYER_H

#include <QPushButton>
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

#include "pipeline.h"
#include <QEventLoop>
#include "inspectionWidget.h"
#include <QMainWindow>

class Player : public QObject{
    Q_OBJECT
    
public:
    /* Constructor */
    Player(Pipeline * p, QPushButton * b);
    //--------------------------------------------------------------------------
    
    QPushButton* btn;   // button used as a previewing screen
    
    /* Destructor */
    virtual ~Player();
    //--------------------------------------------------------------------------
    
public slots:
    /**
     * This slot is called when previewing button is pressed, to pause preview.
     */
    void stopedSlot();
    //--------------------------------------------------------------------------
    
    /**
     * This slot is called when thread for player is ready to work,
     * Starts reading and processing frames from AVFormatCtx container and
     * sends them to previw QPushButton instance.
     */
    void runPlaying();
    //--------------------------------------------------------------------------
    
    /**
     * This slot is called when main thread wants to break previewing.
     */
    void finishIt(){ emit playFinished();}
    //--------------------------------------------------------------------------
    
    /**
     * This slot is called when dynamic inspector finished updating and do not
     * uses filter graph anymore.
     */
    void inspectionUpdated(){ emit procDone();};
    //--------------------------------------------------------------------------
signals:
    void sendingSize(const QSize &);
    void sendingImage(const QImage &);
    /**
     *  Signal, signalises that player can continue processing frames.
     */
    void procDone();
    //--------------------------------------------------------------------------

    /**
     * Signal, emited when player finished previewing.
     * @param 
     */
    void playFinished();
    //--------------------------------------------------------------------------
   
    /**
     * Signal, emited when one frame is processed.
     */
    void frameFinishedSig();
    //--------------------------------------------------------------------------
    
    /**
     * Signal, emited when player finished previewing.
     * @param 
     */
    void playFinished2(Player*);
    //--------------------------------------------------------------------------
    
private:
    QIcon picture;
    bool stoped;         // flag, is true when previewing stoped 
    Pipeline * pipeline; // pipeline to be previewed
    bool opened;         // flag, is true when first frame was already displayed
    
    /**
     * Initializes preview button defaults.
     * @param w width of a button
     * @param h height of a button
     */
    void initializeButton(int w, int h);
    //--------------------------------------------------------------------------
    
    /**
     * This function converts AVFrame to QImage.
     * @param filt_frame reference to AVFrame to be converted
     * @return QImage
     */
    QImage frameToQImage(AVFrame * filt_frame);
    //--------------------------------------------------------------------------
};

#endif /* PLAYER_H */

