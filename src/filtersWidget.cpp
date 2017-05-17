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

#include "filtersWidget.h"
#include <QTextEdit>
#include <libavfilter/avfilter.h>
#include <QLineEdit>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>

/**
 * Constructors and destructor
 */

FilterParamsList::FilterParamsList() {
    ui.setupUi(this);
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
    
    ui.tableWidget->setRowCount(0); 
}

FilterParamsList::FilterParamsList(const FilterParamsList& orig) {
}

FilterParamsList::~FilterParamsList() {
}
//------------------------------------------------------------------------------

/**
 * This function takes filter parameters and creates 
 * corresponding cells in table widget. 
 * @param f filter which parameters are represented
 */

void FilterParamsList::initializeWidget(Filter* f){
    
    disconnect(ui.tableWidget, SIGNAL(cellChanged(int, int)), 
                        (FilterParamsList*)this, SLOT(changedCell(int, int)));
    
    lastFilter=f;
    
    ui.tableWidget->clear();
    QTableWidgetItem * h = new QTableWidgetItem("name");
    QTableWidgetItem * h2 = new QTableWidgetItem("value");
    ui.tableWidget->setHorizontalHeaderItem(0, h);
    ui.tableWidget->setHorizontalHeaderItem(1, h2);
    ui.tableWidget->setRowCount(f->params.size());
    
    int i = 0;
    int j = 0;
    for(auto k : f->params){
        
        QTableWidgetItem * item = new QTableWidgetItem(k->name);
        item->setFlags(item->flags() &  ~Qt::ItemIsEditable);
        ui.tableWidget->setItem(i,j,item);
        j++;
        
        if(f->getName() == "output" && k->name == "Encoder"){
            
            createComboBox(k, i,j);
        }
        else{
            createLineEdit(k, i,j);
        }
        
        i++;
        j=0;
    }
    
    connect(ui.tableWidget, SIGNAL(cellChanged(int, int)), (FilterParamsList*)this,
            SLOT(changedCell(int, int)));
    
    connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), (FilterParamsList*)this,
            SLOT(changeDescription(int, int)));
}
//------------------------------------------------------------------------------

/**
 *  Creates QComboBox object and places it 
 *  as a cell widget.
 *  @param k - filter parameter of a cell
 *  @param r - row to place combo box
 *  @param c - col to place combo box
 */

void FilterParamsList::createComboBox(FilterParameter * k, int r,int c){
    QComboBox * item3 = new QComboBox;
    AVCodec * tmp=NULL;

    while((tmp=av_codec_next(tmp))){
        if(av_codec_is_encoder(tmp) && tmp->type == AVMEDIA_TYPE_VIDEO){
            item3->addItem(QString(tmp->long_name));
        }
    }
    
    item3->setCurrentIndex(item3->findText(k->value));

    connect(item3, SIGNAL(currentIndexChanged(int)), 
                        (FilterParamsList*)this, SLOT(changedBox(int)));

    ui.tableWidget->setCellWidget(r,c,item3);
}
//------------------------------------------------------------------------------

/**
 *  Creates QLineEdit object and places it 
 *  as a cell widget.
 *  @param k - filter parameter of a cell
 *  @param r - row to place line edit
 *  @param c - col to place line edit
 */

void FilterParamsList::createLineEdit(FilterParameter * k, int r,int c){
    QLineEdit * item2 = new QLineEdit;
    item2->setProperty("row", r);
    item2->setProperty("column", c);
    item2->setText(k->value);
    if(lastFilter->getName() == "input" || lastFilter->getName() == "output")
        item2->installEventFilter(this);

    connect(item2, SIGNAL(textChanged(QString)), (FilterParamsList*)this,
            SLOT(changedEdit(QString)));

    connect(item2, SIGNAL(textChanged(QString)), (FilterParamsList*)this,
            SLOT(changedEdit(QString)));

    if(k->type==ParametersType::DOUBLE){
        item2->setValidator(new QRegExpValidator(QRegExp(
                           "[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?"), this));
    }

    else if(k->type==ParametersType::INT)
        item2->setValidator(new QIntValidator((int) k->min, (int) k->max, this));
    ui.tableWidget->setCellWidget(r,c,item2);
}
//------------------------------------------------------------------------------

/**
 * Slot, called when user changes value of a parameter in table widget.
 * Cell with value is QLineEdit instance.
 * @param text
 */

void FilterParamsList::changedEdit(QString text){
    lastFilter->changeParameters(((QLineEdit*)sender())->property("row").toInt(),
            ((QLineEdit*)sender())->property("column").toInt(),ui.tableWidget);
    
}
//------------------------------------------------------------------------------

/**
 * Slot, called when user changes value of a parameter in table widget.
 * Cell with value is QComboBox instance.
 * @param idx
 */

void FilterParamsList::changedBox(int idx){
    lastFilter->changeParameters(1,1,ui.tableWidget);
}
//------------------------------------------------------------------------------

/**
 * Slot, changes value of parameter in a filter.
 * Called when user changed value in a table.
 * @param r - row of a changed cell
 * @param c - coll of a changed cell
 */

void FilterParamsList::changedCell(int r, int c){
    lastFilter->changeParameters(r,c,ui.tableWidget);
}
//------------------------------------------------------------------------------

/**
 * Slot, changes description string, when table cell was selected.
 */

void FilterParamsList::changeDescription(int r, int c){
   
    for(auto f : lastFilter->params){
        if(f->name.compare(ui.tableWidget->item(r,0)->text())==0){
            helpStr=f->descr;
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Event handler. 
 * Called when something is changed in a table.
 * @param object
 * @param event
 * @return 
 */

bool FilterParamsList::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::MouseButtonPress) {
        QString filter = "(*.*)";
        
        QString file = lastFilter->getName() == "output" ? 
                       QFileDialog::getSaveFileName(this, tr("Save video"), ".",
                       tr("All files (*.*)")) : QFileDialog::getOpenFileName(
                       this, "Select a file...", QDir::currentPath(), filter);
        
        if(!file.isEmpty())
            ((QLineEdit*)object)->setText(file);
        return false; 
    }
    return false;
}
//------------------------------------------------------------------------------
