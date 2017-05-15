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

#include <QApplication>
#include <iostream>
#include "player.h"
#include "mainwindow.h"

void printHelp(){
    std::cout << "Usage: \n";
    std::cout << QApplication::applicationName().toStdString() << " -i [input] -vf 'filter string' [output]\n" ;
    std::cout << "If output is not specified, video is played.\n" ;
    exit(0);
}

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);
    
    if(argc > 1){
        if(argc == 5){
            
        }
        else{
            printHelp();
        }
    }
    else{
        MainWindow w;
        w.show();
        return app.exec();
    }
    // create and show your widgets here

    
}
