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
#include "filter.h"
#include "filtersWidget.h"
#include "scene.h"
#include "connectivity.h"
#include "filtersWidget.h"
#include <QDebug>
#include <QBitmap>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <libavutil/opt.h>
#include <QComboBox>

ParametersType avfilToMine(AVOptionType t){
    switch(t){
        case AV_OPT_TYPE_DOUBLE:
        case AV_OPT_TYPE_FLOAT:  return ParametersType::DOUBLE; break;
        case AV_OPT_TYPE_INT:
        case AV_OPT_TYPE_INT64:   return ParametersType::INT; break;        
        case AV_OPT_TYPE_RATIONAL:   return ParametersType::RATIONAL; break;      
        default:  return ParametersType::STRING; break;
    }    
}

Filter::Filter(QString name, QSize inOut, QPointF pos) {
    selected = false;
    
    this->setFlag(QGraphicsItem::ItemIsMovable);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    this->setFlag(QGraphicsItem::ItemIsSelectable);
    
    in=out=in2=out2=NULL;    
    
    
    if(inOut.height()==2)
    {
        out = new Pad(QRectF(pos.x()+182, pos.y()+2,18.0, 18.0));
        out->setParent(this);
        out2 = new Pad(QRectF(pos.x()+182, pos.y()+30,18.0, 18.0));
        out2->setParent(this);
    }  
    else{
        if(name != "output"){
            out = new Pad(QRectF(pos.x()+182, pos.y()+16,18.0, 18.0));
            out->setParent(this);
        }
    }
    if(inOut.width()==2)
    {
        in = new Pad(QRectF(pos.x(), pos.y()+2,18.0, 18.0));
        in->setParent(this);
        in2 = new Pad(QRectF(pos.x(), pos.y()+30,18.0, 18.0));
        in2->setParent(this);
    }  
    else
    {
        if(name != "input"){
            in = new Pad(QRectF(pos.x(), pos.y()+16,18.0, 18.0));
            in->setParent(this);
        }
    }
    
    setName(name); 
    filterRect = QRectF(pos.x(), pos.y(),200.0, 50.0);
    ctx=NULL;
    initializeParams();
}


Filter::Filter(const Filter& orig) {}

Filter::~Filter() {}

void Filter::changeParameters(int r, int c, QTableWidget * widg){
    for(auto f : params){
        if(f->name == widg->item(r,c-1)->text()){
            if(name=="output" && r == 1)
                f->value=((QComboBox*)widg->cellWidget(r,c))->currentText();
            else
                f->value=((QLineEdit*)widg->cellWidget(r,c))->text();
        }
    }
}

QRectF Filter::boundingRect() const{
    return filterRect;
}

void Filter::setConnectivity(Connectivity *c){
    if(this->in != NULL)
        this->in->setConnectivity(c);
    if(this->in2 != NULL)
        this->in2->setConnectivity(c);
    if(this->out != NULL)
        this->out->setConnectivity(c);
    if(this->out2 != NULL)
        this->out2->setConnectivity(c);
}

void Filter::setCtx(AVFilterContext * context){
    this->ctx=context;
    if(QString(context->name).compare("in") == 0 || QString(context->name).compare("out") == 0 )
        return;
    
    AVFilter *avfil = ctx->filter;
    if(avfil->priv_class != NULL){ 
        if(avfil->priv_class->category == AV_CLASS_CATEGORY_FILTER ){
            const AVOption * opt = avfil->priv_class->option;
            while(opt!=NULL){
                for(auto p : params){
                    if(p->name.compare(QString(opt->name))==0){
                        uint8_t * val=NULL;
                        av_opt_get(ctx, opt->name, AV_OPT_SEARCH_CHILDREN, &val);
                        
                        p->value = QString(val).replace(",", ".");
                    }
                }
                opt=  av_opt_next(avfil->priv_class,opt);
            }
        }
    }
    
}

const AVOption *av_opt_next_mine(void *obj, const AVOption *last)
{
    AVClass *cl = *(AVClass**)obj;
    
    if (!last && cl->option && cl->option[0].name)
        return cl->option;
    if (last && last[1].name)
        return ++last;
    
    return NULL;
}

void Filter::initializeParams(){
    
    if(name == "input" || name == "output"){
        params.push_back(new FilterParameter(ParametersType::STRING, 0,
                        0, QString("File"), QString("File to read/write"), 
                        QString("")));
    }
    if(name == "output"){
        params.push_back(new FilterParameter(ParametersType::STRING, 0,
                        0, QString("Encoder"), QString("Encoder"), 
                        QString("")));
    }
    AVFilter *avfil = NULL;
    while((avfil = avfilter_next(avfil))){
        if(QString(avfil->name).compare(name)==0){
            description=avfil->description;
            if(avfil->priv_class != NULL){ 
                
                if(avfil->priv_class->category == AV_CLASS_CATEGORY_FILTER ){
                    
                    AVOption * opt = avfil->priv_class->option;
                    
                    while(opt!=NULL){
                        if(QString(opt->name).isEmpty()) break;
                        params.push_back(new FilterParameter(avfilToMine(opt->type), opt->min,
                                opt->max, QString(opt->name), QString(opt->help), QString("")));
                        
                        opt=av_opt_next(&(avfil->priv_class),opt);
                    }
                }
            }
            break;
        }
    }
}

QVariant Filter::itemChange(GraphicsItemChange change, const QVariant & value){
    if(change == ItemPositionChange){
        if(this->in != NULL)
            if(this->in->connectivity != NULL)
                this->in->connectivity->updateWires(this->in);
        if(this->out != NULL)
            if(this->out->connectivity != NULL)
                this->out->connectivity->updateWires(this->out);
        if(this->out2 != NULL)
            if(this->out2->connectivity != NULL)
                this->out2->connectivity->updateWires(this->out2);
        if(this->in2 != NULL)
            if(this->in2->connectivity != NULL)
                this->in2->connectivity->updateWires(this->in2);
        
    }
    if(change == ItemSelectedChange){
        if(value == true){
            selected = true;
        }
        else{
            selected = false;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void Filter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect = boundingRect();
    QPixmap p(":/filt1.svg");
    painter->drawImage(rect, p.toImage(), p.rect());
    if(this->selected){
        QPixmap p(":/filt1S.svg");
        painter->drawImage(rect, p.toImage(), p.rect()); 
    }
    
    QFont font=painter->font();
    font.setWeight(QFont::Bold);
    font.setPointSize ( 14 );
    painter->setFont(font); 
    QPen pen2(Qt::black, 3);
    painter->setPen(pen2);

    QRect r(boundingRect().x() + 20,
            boundingRect().y(),
            boundingRect().width()-40,
            boundingRect().height());
    
    painter->drawText(r, Qt::AlignCenter | Qt::TextWrapAnywhere, name);
}

void Filter::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    helpStr=description;
    setCursor(Qt::ClosedHandCursor);
    
    ((Scene*)this->scene())->getFilterParamList()->initializeWidget(this);
    
    QGraphicsItem::mousePressEvent(event);
}

void Filter::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    QGraphicsItem::mouseMoveEvent(event);
}

void Filter::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}
       
