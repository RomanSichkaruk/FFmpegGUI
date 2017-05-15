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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scene.h"
#include "filter.h"
#include "filtersWidget.h"
#include "view.h"
#include <QList>
#include <QLabel>
#include "externs.h"
#include "filtersWidget.h"
#include "connectivity.h"
#include "wire.h"
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QLineF>
#include <QFileDialog>
#include <QDockWidget>
#include <QScrollArea>
#include "player.h"
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QStyle>
#include <QXmlStreamWriter>
#include <QMenu>
#include <QTimer>
#include <QThread>
#include <QScrollBar>
#include <QToolBar>
#include <QMenuBar>

#include <QMessageBox>
#include "inspectionWidget.h"

extern "C"{
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
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class FiltersList;

class MainWindow : public QMainWindow{
    Q_OBJECT
    
protected:
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent* event);
    
public:
    MainWindow();
    MainWindow(const MainWindow& orig);
    virtual ~MainWindow();
    

private:
    FiltersList *filterBar;
    Pipeline * actualPipeline;
    QList<Player*> players;
    QString outputName();
    QString getEncoder();
    void parseTxt(const QString &filename);
    void createActions();
    void prepareScene();
    void initializeHelp();
    QAction * createNewAction(const QString &info, const QString &icon, Qt::Key key);
    Pad * closestPad(Pad * p);
    Pad * inputPad();
   
    
    QLabel *unicorn;
    QLabel * helpText;
    Scene * scene;
    View * view;
    InspectionWidget * insWidget;
    Connectivity *c;
    
    
    QToolBar *fileToolBar;
    
    QAction *newAct;
    QAction *saveXml;
    QAction *saveTxt;
    QAction *stopPlay;
    QAction *wire;
    QAction *save;
    QAction *curs;
    QAction *clear;
    QAction *remove;
    QAction *hand;
    QAction *filter;
    QAction *play;
    QAction *text;
    QAction *help;
    QAction *automatic;
    QAction *exitAll;
    QAction *showFList;
    QAction *inspection;
    QAction *showPList;
    
    QMenu *fileMenu;
    FilterParamsList * filterList;
    QPushButton * shower;
public slots:
    void receivedSize(const QSize & size);
    void receivedImage(const QImage & im);
    void connectWire();
    void createFilterBar();
    void addInput();
    void clearAll();
    void playVideo();
    void removeDock(Player * p);
    void updateInspector();
    void justContinue();
    void saveXML();
    void saveTXT();
    void handTogled();
    void cursTogled();
    void autoConnect();
    void saveVideo();
    void showF();
    void showP();
    void exitP();
    void removeItem();
    void showText();
    void runInspection();
    void showHelp();
    
signals:
    void doFinish();
    void insUpdated();
    void continuePlaying();
    
    
};

//! [17] //! [18]

#endif /* MAINWINDOW_H */

