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
#include "inspectionWidget.h"

/**
 * Constructors and destructor
 */

InspectionWidget::InspectionWidget() {
    widget.setupUi(this);
    widget.tableWidget->setColumnCount(2);
    widget.tableWidget->setRowCount(1);
    QTableWidgetItem * h = new QTableWidgetItem("description");
    QTableWidgetItem * h2 = new QTableWidgetItem("value");
    widget.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    widget.tableWidget->setHorizontalHeaderItem(0, h);
    widget.tableWidget->setHorizontalHeaderItem(1, h2);
    this->hide();
}
InspectionWidget::~InspectionWidget() {}
//------------------------------------------------------------------------------

/**
 *  Returns table widget.
 */

QTableWidget * InspectionWidget::getTable(){
    return widget.tableWidget;
}
//------------------------------------------------------------------------------