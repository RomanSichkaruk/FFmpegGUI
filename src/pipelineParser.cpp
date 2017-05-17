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

#include "pipelineParser.h"

/**
 *  Constructors and destructor
 */

PipelineParser::PipelineParser(Scene * s) {
    scene=s;
    wasCreated=false;
}
PipelineParser::PipelineParser(Pipeline * p, Scene * s) : pipeline(p), scene(s) {
    
    wasCreated=false;
    
    parse();
    
}
PipelineParser::PipelineParser(Pipeline * p, Scene * s, bool aply) : pipeline(p), scene(s) {
    wasCreated=false;
    aplyOnScene();
    
}
PipelineParser::PipelineParser(const PipelineParser& orig) {
}
//------------------------------------------------------------------------------

PipelineParser::~PipelineParser() {
    if(pipeline && wasCreated){
        delete pipeline;
    }
}
//------------------------------------------------------------------------------

/**
 * Loads pipeline from TXT file.
 * @param filename
 */

void PipelineParser::parseTxt(const QString& filename){
    
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        #warning do errors
    }
    
    QTextStream in(&file);
    QString text;
    
    while(!in.atEnd()) {
        text += in.readLine();  
    }
    
    text.replace("\n", " ");
    QString input = text.mid(text.indexOf("-i ")+2, text.indexOf("-vf ")- text.indexOf("-i ")-2);
    QString filtStr = text.mid( text.indexOf("-vf ")+4);
    filtStr=filtStr.replace("'", "");
    updateIndexes(filtStr);
    pipeline = new Pipeline(input, filtStr);
    wasCreated=true;
    parse();
    file.close();
    
}
//------------------------------------------------------------------------------

/**
 * Loads pipeline from XML file.
 * @param filename
 */

void PipelineParser::parseXml(const QString& filename){
   
        QFile file(filename);
        
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
                    #warning do errors
	}
        
        QXmlStreamReader Rxml;
	Rxml.setDevice(&file);
        
        QString filtName;
        QPointF pos;
        QHash<int, QString> padNames;
        QHash<QString, QString> parameters;
        
	while(!Rxml.atEnd() && !Rxml.hasError())
	{
            Rxml.readNext();
            if(Rxml.name().toString().compare("FILTERS") == 0) continue;
            else if(Rxml.isEndElement()){
                if(Rxml.name().toString()=="FILTER"){
                    scene->createNewFilter(filtName, pos);
                    setupFilter(scene->filters.back(), &padNames, &parameters);
                }
                continue;
            }
            else if(Rxml.name().toString() == "FILTER"){
                Rxml.readNextStartElement();
                filtName =  Rxml.readElementText();
                Rxml.readNextStartElement();
                pos.setX(Rxml.readElementText().toDouble());
                Rxml.readNextStartElement();
                pos.setY(Rxml.readElementText().toDouble());
            }
            else if(Rxml.name().toString() == "PAD"){                
                Rxml.readNextStartElement();
                int which =  Rxml.readElementText().toInt();
                Rxml.readNextStartElement();
                QString padName = Rxml.readElementText();
                indexPads = indexPads < padName.mid(2,padName.size()-3).toInt()?
                    padName.mid(2,padName.size()-3).toInt() + 1 :indexPads;
                padNames[which]=padName;
            }
            else if(Rxml.name().toString() == "PARAMETER"){
                Rxml.readNextStartElement();
                QString name =  Rxml.readElementText();
                Rxml.readNextStartElement();
                QString value = Rxml.readElementText();
                parameters[name]=value;
            }
            else if(Rxml.name().toString() == "WIRE"){
                setupWire(&Rxml);
            }
            
        }
        if (Rxml.hasError())
        {
             qDebug() << "XML error: " << Rxml.errorString();
        }
	file.close();
}
//------------------------------------------------------------------------------

/**
 *  Reads WIRE node from XML file.
 *  Creates corresponding wire on scene.
 */

void PipelineParser::setupWire(QXmlStreamReader * Rxml){
    
    Rxml->readNextStartElement();
    QString inName =  Rxml->readElementText();
    Rxml->readNextStartElement();
    QString outName = Rxml->readElementText();
    
    Pad * in = NULL, * out = NULL;
    for(auto f : scene->filters){
        if(f->getInPad() && f->getInPad()->getName()==inName)
            in = f->getInPad();
        if(f->getInPad2() && f->getInPad2()->getName()==inName)
            in = f->getInPad2();
        if(f->getOutPad() && f->getOutPad()->getName()==inName)
            in = f->getOutPad();
        if(f->getOutPad2() && f->getOutPad2()->getName()==inName)
            in = f->getOutPad2();
        if(f->getInPad() && f->getInPad()->getName()==outName)
            out = f->getInPad();
        if(f->getInPad2() && f->getInPad2()->getName()==outName)
            out = f->getInPad2();
        if(f->getOutPad() && f->getOutPad()->getName()==outName)
            out = f->getOutPad();
        if(f->getOutPad2() && f->getOutPad2()->getName()==outName)
            out = f->getOutPad2();
    }
    
    Wire * w = new Wire(scene);
    w->setConnection(in,out, scene->getConnectivity());
    scene->getConnectivity()->updateWires(in);
}
//------------------------------------------------------------------------------
    
/**
 *  Reads FILTER node from XML file.
 *  Creates corresponding filter on scene.
 */

void PipelineParser::setupFilter(Filter * f, QHash<int, QString> * padNames,
                                           QHash<QString, QString> *parameters){
    
    for(auto pad : padNames->keys()){
        switch (pad){
            case 1: f->getInPad()->setName(padNames->value(pad));
                break;
            case 2: f->getInPad2()->setName(padNames->value(pad));
                break;
            case 3: f->getOutPad()->setName(padNames->value(pad));
                break;
            case 4: f->getOutPad2()->setName(padNames->value(pad));
                break;
            default: 
                break;
        }
    }
    padNames->clear();
    
    for(auto param : parameters->keys()){
        for(auto p : f->params){
            if(p->name.compare(param) == 0){
                p->value=parameters->value(param);
            }
        }
    }
    parameters->clear();                
}
//------------------------------------------------------------------------------

/**
 * Updates pads indexes.
 * So new unique index is max index from TXT file + 1
 * @param str
 */

void PipelineParser::updateIndexes(const QString & str){
    bool read=false;
    bool startT = false;
    int idx=0;
    for(auto c : str){
        if(c == '['){
            read=true;
        }
        else if(c == ']'){
            read=false;
            if(indexPads < idx)
                indexPads = idx;
            idx=0;
        }
        else if (read && c == 'T'){
            startT=true;
        }
        else if(isdigit(c.toLatin1()) && startT){
            idx=c.toLatin1()-'0' + 10*idx;
        }
        else{
            startT=false;
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Creates graphical representation of a pipeline.
 */

void PipelineParser::aplyOnScene(){
    for(int i = 0; i < pipeline->getGraph()->nb_filters; i++){
        if(QString(pipeline->getGraph()->filters[i]->name)=="in"){
            setFilter(NULL, NULL, pipeline->getGraph()->filters[i]);
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Setups created filter and sets its context.
 */

void PipelineParser::setFilter(Filter * previous, Pad* pad, AVFilterContext*ctx){

    if(QString(ctx->name).compare("out") == 0) return;
    
    if(!previous){
        AVFilterContext *next= ctx->outputs[0]->dst;
        while(!QString(next->name).startsWith("Parsed") &&
              !QString(next->name).startsWith("out")){
            next=next->outputs[0]->dst;
        }
        for(auto f : scene->filters)
            if(f->getName().compare("input") == 0){
                f->setCtx(ctx);
                setFilter(f, f->getOutPad(), next);
            }
    }
    else{
        Filter * flt;
        for(auto f : scene->filters){
            if(f->getInPad())
            {
                if(f->getInPad()->connectedTo() == pad){
                    f->setCtx(ctx);
                    flt=f;
                    break;
                }
            }
            if(f->getInPad2()){
                if(f->getInPad2()->connectedTo()  == pad){
                    f->setCtx(ctx);
                    flt=f;
                    break;
                }
            }
        }
        
        for(int i = 0; i < ctx->nb_outputs; i++){
            AVFilterContext *next= ctx->outputs[i]->dst;
            while(!QString(next->name).startsWith("Parsed") &&
              !QString(next->name).startsWith("out")){
                next=next->outputs[0]->dst;
            }
            if(i==0)
               setFilter(flt, flt->getOutPad(), next);
            else
               setFilter(flt, flt->getOutPad2(), next); 
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Parses pipeline created in GUI.
 */

void PipelineParser::parse(){
    QPointF pos(0,300);
    for(int i = 0; i < pipeline->getGraph()->nb_filters; i++){
        if(QString(pipeline->getGraph()->filters[i]->name)=="in"){
            createFilter(pipeline->getGraph()->filters[i], pos);
        }
    }
    for(auto f : scene->filters){
        if(f->getName().compare("output")==0) continue;
        
        for(int i = 0; i < f->ctx->nb_outputs; i++){
            AVFilterContext *next=f->ctx->outputs[i]->dst;
            AVFilterLink * link=f->ctx->outputs[i]; 
            while(!QString(next->name).startsWith("Parsed") &&
                  !QString(next->name).startsWith("out")){
                link=next->outputs[0];
                next=next->outputs[0]->dst;
            }
            for(auto f2 : scene->filters){
                if((AVFilterContext*) next == (AVFilterContext*) f2->ctx){
                    for(int j = 0; j < f2->ctx->nb_inputs; j++){
                        if((AVFilterPad*)  f2->ctx->inputs[j]->dstpad == 
                           (AVFilterPad*) link->dstpad){
                            
                            Pad * ip, * op;
                            if(i==0)
                                ip=f->getOutPad(); 
                            else
                                ip=f->getOutPad2();
                            if(j==0)
                                op=f2->getInPad();
                            else
                                op=f2->getInPad2();
                            
                            Wire * w = new Wire(scene);
                            w->setConnection(ip,op, scene->getConnectivity());
                            scene->getConnectivity()->updateWires(ip);
                        }
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Create new graphics filter item on scene.
 * Uses info from AVFilterContext.
 * @param ctx - all info about filter to be created.
 * @param pos - position to place filter.
 */

void PipelineParser::createFilter(AVFilterContext*ctx, QPointF pos){
    
    QString name=ctx->name;
    if(name=="in") name="input";
    else if(name=="out") name="output";
    else name=ctx->filter->name;
  
    scene->createNewFilter(name, pos);
    if(scene->filters.back()->getName().compare("input")==0)
        scene->filters.back()->params[0]->value = pipeline->getInPath().trimmed();
    bool y=false;
    while(!y){
        y=true;
        for(auto fl:scene->filters){
            if(fl==scene->filters.back())continue;
            if(fl->sceneBoundingRect().intersects(scene->filters.back()->boundingRect())){
                scene->filters.back()->moveBy(0,70);
                y=true;
            }
        }
    }
    scene->filters.back()->setCtx(ctx);
    
    for(int i = 0; i < ctx->nb_outputs; i++){
        AVFilterContext *next=ctx->outputs[i]->dst;
        while(!QString(next->name).startsWith("Parsed") &&
              !QString(next->name).startsWith("out")){
            next=next->outputs[0]->dst;
        }
        bool added=false;
        
        for(auto fltr:scene->filters){
            if(fltr->ctx == NULL) continue;
            if((AVFilterContext*)fltr->ctx == (AVFilterContext*) next){
                added=true;
            }
        }
        if(!added)
           createFilter(next, pos+QPointF(220, i*70));
    }
}
//------------------------------------------------------------------------------

/**
 * Sets filter on scene his filter context from built pipeline.
 * @param fc
 */

void PipelineParser::setFiltContext(AVFilterContext * fc){
    
    AVFilterContext * next= fc->inputs[0]->src;
    QStringList path;
    while(QString(next->name) != "in"){
        if(QString(next->name).contains("Parsed")){
            path << next->name;
        }
        next=next->inputs[0]->src;
    }
    for(auto f : scene->filters){
        if(QString(fc->name).contains(f->getName())){
            Filter * next=f->getInPad()->connectedTo()->getParent();
            int i =0;
            while(next->getName().compare("input") != 0 && i < path.size()){
                if(!path[i].contains(next->getName())){
                    break;
                }
                
                next=next->getInPad()->connectedTo()->getParent();
                i++;
            }
            if(i == path.size() && next->getName().compare("input") == 0)
                f->setCtx(fc);
        }
    }
}
//------------------------------------------------------------------------------
