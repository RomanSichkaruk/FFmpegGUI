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

#include "connectivity.h"
#include "wire.h"
#include "pad.h"

#include "filter.h"
#include <QDebug>

/**
 *  Constructors
 */

Connectivity::Connectivity() {}

Connectivity::Connectivity(const Connectivity& orig) {}
//------------------------------------------------------------------------------

/**
 *  Destructor
 */

Connectivity::~Connectivity() {}
//------------------------------------------------------------------------------

/**
 * This function adds new wire to list of all wires.
 * @param w is a wire to be added
 */

void Connectivity::addWire(Wire * w){
    wires.push_back(w);
}
//------------------------------------------------------------------------------

/** 
 * This funcction updaptes location of a wire on scene.
 * Wire to be updated is a wire that has one of pads corresponding 
 * to pad sent as an argument.
 * @param p that is connected by wire
 */

void Connectivity::updateWires(Pad * p){
    for(auto w : wires){
        if(w->getIn() == p || w->getOut() == p){
            w->updateLine();
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Computes string for ffplay.
 * @return 
 */

QString Connectivity::computeString(){
    
    QString cmd;
    QList<Filter*> parsed;
    Pad*toParse=getInput()->connectedTo();
    QList<Pad*> sorted;
    QList<Pad*> sorted2;
    sorted << toParse;
    while(!sorted.empty()){
        for(auto p : sorted){
            if(!parsed.contains(p->getParent())){
                sorted2 << parseOnePart(p,&cmd);
                parsed << p->getParent();
            }
        }
        for(int i =0; i < sorted2.size(); i++){
            for(int j =0; j < sorted2.size(); j++){
                if(i != j && sorted2[i]->getParent() == sorted2[j]->getParent())
                    sorted2.removeAt(i);
            }
        }
        sorted=sorted2;
        sorted2.clear();
    }
    if(cmd.endsWith(";")) cmd = cmd.mid(0, cmd.size()-1);
    return cmd;
}
//------------------------------------------------------------------------------

/**
 *  This function returns path to input file.
 */

QString Connectivity::inputPath(){
    for(auto w : wires){
        if(w->getIn()->getParent()->getInPad() == NULL){
            return w->getIn()->getParent()->getPath();
        }
    }
    return QString();
}
//------------------------------------------------------------------------------

/**
 *  This function returns pad of an input buffer.
 */

Pad* Connectivity::getInput(){
    Pad*in=NULL;
    for(auto w : wires){
        if(w->getIn()->getParent()->getInPad() == NULL){
            return w->getIn();
        }
    }
    if(!in)
        throw std::runtime_error("No input file specified.");
    return in;
}
//------------------------------------------------------------------------------

/**
 * Deletes wire from a list of all wires.
 * @param w wire to be deleted.
 */

void Connectivity::removeWire(Wire * w){
    for(int i = 0; i < wires.size(); i++){
        if(wires[i] == w){
            wires[i]->getLine()->scene()->removeItem(wires[i]->getLine());
            wires[i]->getIn()->connectTo(NULL); 
            wires[i]->getOut()->connectTo(NULL); 
            wires.removeAt(i);
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Removes all wires, that have of of pads sent as arguments in
 * their connected pads. Called on filter deletion.
 * @param p1 - first (input) pad of a filter.
 * @param p2 - second (output) pad of a filter.
 */

void Connectivity::removeWires(Pad * p1, Pad * p2){
    QList<Wire *> toRemove;
    for(int i = 0; i < wires.size(); i++){
            if(wires[i]->getIn() == p2){
                toRemove.push_back(wires[i]);
            }
            if(wires[i]->getIn() == p1){
                toRemove.push_back(wires[i]);
            }
            if(wires[i]->getOut() == p1){
                toRemove.push_back(wires[i]);
            }
            if(wires[i]->getOut() == p2){
                toRemove.push_back(wires[i]);
            }
    }
    for(auto w : toRemove)
        removeWire(w);
}
//------------------------------------------------------------------------------

/**
 * Parses one part - sequension of filters that dont have 
 * spliting or overlaying filters (or ends with them)
 * and are connected one by one.
 * @param pad - first input pad of a sequension.
 * @param cmd - string with command to be appended
 * @return list of all output pads
 */

QList <Pad *> Connectivity::parseOnePart(Pad* pad, QString * cmd){
    if(!pad) return QList <Pad *>();
    if(pad->getParentName().compare("output") == 0)  return QList <Pad *>();
    if(pad->getParent()->getOutPad2()){
        
        cmd->append(QString("%3 %1%2;").arg(
                pad->getParent()->getOutPad()->getName()).arg(
                pad->getParent()->getOutPad2()->getName()).arg(
                pad->getParentName()));
        
        cmd->append(QString("%1").arg(
                pad->getParent()->getOutPad()->getName()));
                QList <Pad *> p1 =parseOnePart(pad->getParent()->getOutPad()->connectedTo(),cmd);
                
        cmd->append(QString("%1").arg(
                pad->getParent()->getOutPad2()->getName()));
                QList <Pad *> p2=parseOnePart(pad->getParent()->getOutPad2()->connectedTo(),cmd);
                
        QList <Pad *> toReturn;
        for(auto p_1 : p1){
            bool dupl=false;
            for(auto p_2 : p2){
                if(p_1->getParent() == p_2->getParent())
                    dupl=true;
            }
            if(!dupl)
                toReturn << p_1;
        }
        toReturn << p2;
        return toReturn;   
    }
    else if(pad->getParent()->getInPad2()){
        cmd->append(QString("%1%2 %3").arg(pad->getParent()->getInPad()->getName()).arg(
                pad->getParent()->getInPad2()->getName()).arg(
                pad->getParentName()));
        
        QString parameters;
        for(auto key : pad->getParent()->params){
            if(!key->value.isEmpty()){
                if(!parameters.isEmpty())
                    parameters.append(":");
                QString valueParam(key->value);
                parameters.append(QString("%1=%2").arg(key->name).arg(
                            valueParam.replace(",", "\\,").replace(".", "\\,")));         
            }
        }
        
        if(!parameters.isEmpty())
                    cmd->append(QString("=%1").arg(parameters));       

        if(pad->getParent()->getOutPad()->connectedTo()!=NULL &&
           pad->getParent()->getOutPad()->connectedTo()->getParent()->getOutPad() != NULL){
                    cmd->append(QString(" %1;").arg(
                            pad->getParent()->getOutPad()->connectedTo()->getName()));
                    return (QList <Pad *>() <<
                            pad->getParent()->getOutPad()->connectedTo());
        }
        else{
            cmd->append(" [out];");
        }
        return QList <Pad *>();
    }
    else{
        Pad * tmp=pad;
        do{
            if(tmp->getParent()->getOutPad2()){
                return (QList <Pad *>() << tmp);
            }
            else if(tmp->getParent()->getInPad2()){
                cmd->append(tmp->getName());
                cmd->append(";");
                return (QList <Pad *>() << tmp);
            }
            else{
                if(!tmp->getParent()->getOutPad())
                    break;
                if(tmp->connectedTo()->getParent()->getInPad2())
                    cmd->append(tmp->getName());
                cmd->append(tmp->getParent()->getName());
                QString parameters;
                for(auto key : tmp->getParent()->params){
                    if(!key->value.isEmpty()){
                        if(!parameters.isEmpty())
                            parameters.append(":");
                        
                        QString valueParam(key->value);
                        parameters.append(QString("%1=%2").arg(key->name).arg(
                        valueParam.replace(",", "\\,").replace(".", "\\,")));         
                    }
                }
                if(!parameters.isEmpty())
                            cmd->append(QString("=%1").arg(parameters));  
                
                if(tmp->getParent()->getOutPad()->connectedTo()!=NULL &&
                   tmp->getParent()->getOutPad()->connectedTo()->getParent()->getOutPad()!=NULL &&     
                   tmp->getParent()->getOutPad()->connectedTo()->getParent()->getInPad2()==NULL){
                    cmd->append(",");
                }
            }
        }while((tmp=tmp->getParent()->getOutPad()->connectedTo())!=NULL );
    }
    return QList <Pad *>();
}
//------------------------------------------------------------------------------