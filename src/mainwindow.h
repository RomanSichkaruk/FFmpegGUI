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
    /**
     * Mouse pressed event handler.
     * @param event
     */
    void mousePressEvent(QMouseEvent *event);
     //-------------------------------------------------------------------------
    
    /**
     * Window resized event handler.
     */
    void resizeEvent(QResizeEvent* event);
     //-------------------------------------------------------------------------
    
public:
    /**
     * Construrs and destructor
     */
    MainWindow();
    MainWindow(const MainWindow& orig);
    virtual ~MainWindow();
    //--------------------------------------------------------------------------

private:
    FiltersList *filterBar;     // List of all filters
    Pipeline * actualPipeline;  // Current pipeline
    QList<Player*> players;     // List of all players
    
    /**
     * Returns name of an ouput file.
     * @return 
     */
    QString outputName();
    //-------------------------------------------------------------------------
    
    /**
     * Returns used ecnoder.
     * @return 
     */
    QString getEncoder();
    //-------------------------------------------------------------------------
    
    /**
     *  Starts saving of a pipeline to TXT file
     */
    void parseTxt(const QString &filename);
    //-------------------------------------------------------------------------
    
    /**
     * Setups QActions in a toolbar.
     */
    void createActions();
    //-------------------------------------------------------------------------
    
    /**
     * Creates scene and initializes its size.
     */
    void prepareScene();
    //-------------------------------------------------------------------------
    
    /**
     * Initializes help labels.
     */
    void initializeHelp();
    //-------------------------------------------------------------------------
    
    /**
     * Creates new QAction
     * @param info  - description of an action
     * @param icon  - icon of an action
     * @param key   - shortcut for an action
     * @return reference to action     
     */
    QAction * createNewAction(const QString &info, const QString &icon, Qt::Key key);
    //-------------------------------------------------------------------------
    
    /**
     * Returns closest pad to pad sent as an argument
     */
    Pad * closestPad(Pad * p);
    //--------------------------------------------------------------------------
    
    /**
     * Returns output pad of an input filter
     * @return 
     */
    Pad * inputPad();
    //--------------------------------------------------------------------------
       
    QLabel *unicorn;                // Label with helper (penguin).
    QLabel * helpText;              // Label with helper text (buble).
    Scene * scene;                  // Scene.
    View * view;                    // View on scene.
    InspectionWidget * insWidget;   // Inspection dock widget.
    Connectivity *c;                // Connectivity, holds wires.
    QToolBar *fileToolBar;          // Toolbar for quick actions.
    QAction *newAct;                // Quick action to load graph from file.
    QAction *saveXml;               // Quick action to save graph to XML file. 
    QAction *saveTxt;               // Quick action to save graph to TXT file. 
    QAction *wire;                  // Quick action to connect two pads. 
    QAction *save;                  // Quick action to save processed video. 
    QAction *curs;                  // Quick action to change cursor to arrow. 
    QAction *clear;                 // Quick action to clear canvas (scene). 
    QAction *remove;                // Quick action to remove graphics element. 
    QAction *hand;                  // Quick action to change cursor to hand. 
    QAction *play;                  // Quick action to play preview. 
    QAction *text;                  // Quick action to show pipeline text representation. 
    QAction *help;                  // Quick action to show help on selected item. 
    QAction *automatic;             // Quick action to automatically connect pads. 
    QAction *exitAll;               // Quick action to end application. 
    QAction *showFList;             // Quick action to show/hide filters. 
    QAction *showPList;             // Quick action to show/hide parameters. 
    QMenu *fileMenu;                // Main menu. 
    FilterParamsList * filterList;  // Widget with parameters table. 
    QPushButton * shower;           // Button on which preview is displayed. 
    
public slots:
    /**
     * Called when size of frame is received.
     * Initializes preview button size.
     * @param size
     */
    void receivedSize(const QSize & size);
    //--------------------------------------------------------------------------
    
    /**
     * Called when imageis received.
     * Initializes preview button icon.
     * @param size
     */
    void receivedImage(const QImage & im);
    //--------------------------------------------------------------------------
    
    /**
     *  Connects two pads
     */
    void connectWire();
    //--------------------------------------------------------------------------
    
    /**
     *  Initializes widget with parameters table.
     */
    void createFilterBar();
    //--------------------------------------------------------------------------
    
    /**
     * Loads graph from file.
     */
    void addInput();
    //--------------------------------------------------------------------------
    
    /**
     * Clears canvas (scene).
     */
    void clearAll();
    //--------------------------------------------------------------------------
    
    /**
     *  Plays preview.
     */
    void playVideo();
    //--------------------------------------------------------------------------
    
    /**
     * Removes dock widget when previewing finished.
     * @param p
     */
    void removeDock(Player * p);
    //--------------------------------------------------------------------------
    
    /**
     * Updates dynamic inspector data.
     */
    void updateInspector();
    //--------------------------------------------------------------------------
    
    /**
     *  Continues playing after stop.
     */
    void justContinue();
    //--------------------------------------------------------------------------
    
    /**
     * Saves pipeline to XML file.
     */
    void saveXML();
    //--------------------------------------------------------------------------
    
    /**
     * Saves pipeline to TXT file.
     */
    void saveTXT();
    //--------------------------------------------------------------------------
    
    /**
     * Signalises that hand cursor was toogled.
     */
    void handTogled();
    //--------------------------------------------------------------------------
    
    /**
     * Signalises that arrow cursor was toogled.
     */
    void cursTogled();
    //--------------------------------------------------------------------------
    
    /**
     * Autoconnects filters in pipeline
     */
    void autoConnect();
    //--------------------------------------------------------------------------
    
    /**
     * Saves processed video.
     */
    void saveVideo();
    //--------------------------------------------------------------------------
    
    /**
     * Shows/hides filters.
     */
    void showF();
    //--------------------------------------------------------------------------
    
    /**
     * Shows/hides parameters.
     */
    void showP();
    //--------------------------------------------------------------------------
    
    /**
     * Exits from application
     */
    void exitP();
    //--------------------------------------------------------------------------
    
    /**
     *  Removes item from canvas (scene).
     */
    void removeItem();
    //--------------------------------------------------------------------------
    
    /**
     * Shows string with text representation
     * of a pipeline.
     */
    void showText();
    //--------------------------------------------------------------------------
    
    /**
     * Deprecated
     */
    void runInspection();
    //--------------------------------------------------------------------------
    
    /**
     * Shows help for selected item.
     */
    void showHelp();
    //--------------------------------------------------------------------------
    
signals:
    void doFinish();        // Signalises that previewing finished
    void insUpdated();      // Signalises that inspection data can be updated
    void continuePlaying(); // Signalises that inspection was updated
    
};

//! [17] //! [18]

#endif /* MAINWINDOW_H */

