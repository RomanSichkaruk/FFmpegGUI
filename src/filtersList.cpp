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

#include "filtersList.h"
#include "filter.h"
#include "player.h"
#include "externs.h"
#include "scene.h"

#include <QTextEdit>
#include <libavfilter/avfilter.h>
#include <QLineEdit>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>

FiltersList::FiltersList() {
    ui.setupUi(this);
    
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
    vert=true;
    toVerticalChange();
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(changedLocation(Qt::DockWidgetArea)));
    
    connect(ui.lineEdit, SIGNAL(textChanged(QString)), 
            (FiltersList*)this, SLOT(changedEdit(QString)));
}

FiltersList::FiltersList(const FiltersList& orig) {
}

FiltersList::~FiltersList() {
}

void FiltersList::changedLocation(Qt::DockWidgetArea area){
    if(area==Qt::LeftDockWidgetArea || area==Qt::RightDockWidgetArea){
        vert=true;
        toVerticalChange();
    }
    else if(area==Qt::BottomDockWidgetArea || area==Qt::TopDockWidgetArea){
        vert=false;
        toHorizontalChange();
    }
}
void FiltersList::changedEdit(QString text){
    for(int i = 0; i < ui.tableWidget->rowCount(); i++){
        QString txt(((QPushButton*) 
                ui.tableWidget->cellWidget(i,0))->property("name").toString());
        if(txt.contains(text, Qt::CaseInsensitive) == false)
            ui.tableWidget->hideRow(i);
        else
            ui.tableWidget->showRow(i);
    }
    
    ui.tableWidget->resizeColumnsToContents();
}

void FiltersList::toHorizontalChange(){
    
    this->setGeometry(this->x(), this->y(), this->width(), 100);
    
    ui.tableWidget->setColumnCount(0); 
    ui.tableWidget->setColumnCount(Scene::filtersSize.size()+2); 
    ui.tableWidget->setRowCount(1); 
    QTableWidgetItem * h2 = new QTableWidgetItem("Name");
    ui.tableWidget->setVerticalHeaderItem(0, h2);
    ui.tableWidget->setCellWidget(0,0, new FilterButton("input"));
    ui.tableWidget->setCellWidget(0,1, new FilterButton("output"));
    
    int i = 2;
    for(auto fkey : Scene::filtersSize.keys()){
        ui.tableWidget->setCellWidget(0, i++, new FilterButton(fkey));
    }
    
    ui.tableWidget->resizeRowsToContents();
    ui.tableWidget->resizeColumnsToContents();
    ui.tableWidget->setMaximumHeight(88);
    ui.tableWidget->setMaximumWidth(5000);
}

void FiltersList::toVerticalChange(){
    
    ui.tableWidget->setRowCount(0); 
    ui.tableWidget->setColumnCount(1); 
    ui.tableWidget->setRowCount(Scene::filtersSize.size()+2); 
    QTableWidgetItem * h2 = new QTableWidgetItem("Name");
    ui.tableWidget->setHorizontalHeaderItem(0, h2);
    ui.tableWidget->setCellWidget(0,0, new FilterButton("input"));
    ui.tableWidget->setCellWidget(1,0, new FilterButton("output"));
    
    int i = 2;
    for(auto fkey : Scene::filtersSize.keys()){
        ui.tableWidget->setCellWidget(i++,0, new FilterButton(fkey));
    }
    
    ui.tableWidget->resizeRowsToContents();
    ui.tableWidget->resizeColumnsToContents();
    ui.tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui.tableWidget->setColumnWidth(0,220);
    ui.tableWidget->setMaximumWidth(247);
    ui.tableWidget->setMaximumHeight(5000);
}
