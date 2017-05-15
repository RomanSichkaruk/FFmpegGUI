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

#ifndef FILTERBUTTON_H
#define FILTERBUTTON_H

#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QDrag>
#include <QMimeData>

class FilterButton : public QPushButton {
    
public:
    /**
     *  Constructors and destructors
     */
    FilterButton(const QString &name);
    FilterButton(const FilterButton& orig);
    virtual ~FilterButton();
    //--------------------------------------------------------------------------
    
private:
    /**
     * This function handles mouse press event on FilterButton
     * instances. Creates new QDrag and sets its QMimeData to 
     * FilterButton instance's name, so it can be used to reconstruct 
     * in Drop event handler.
     * @param e mouse event to be handled
     */
    void mousePressEvent(QMouseEvent *e);
    //--------------------------------------------------------------------------

    /**
     * This function draws filter pads' icons on image that
     * represents filter.
     * @param painter is reference to QPainter object that provides painting
     * @param size is QSize containing count of input and output pads
     */
    void drawPixmap(QPainter * painter, const QSize & size);
    //--------------------------------------------------------------------------
    
    /**
     * This function creates QPixmap image representing filter
     * @return 
     */
    QPixmap createPixmap();
    //--------------------------------------------------------------------------
};

#endif /* FILTERBUTTON_H */

