// Copyright 2018
// Jaroslav Černi Institute for the Development of Water Resources, Serbia
//
// This file is part of qGrains.
//
//    qGrains is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    qGrains is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with qGrains. If not, see <http://www.gnu.org/licenses/>.
//
// Authors: Milan Dotlić (milandotlic@gmail.com)
//          Milenko Pušić, Goran Jevtić, Milan Dimkić
//
#include "inputHandler.h"
//#include "newCode.h"
#include "model/modelNodeBase.h"
#include "model/pointSet.h"
#include "model/intPointSet.h"
#include "model/modelEnums.h"
#include <QMessageBox>
#include <QDebug>
#include <assert.h>

InputHandler::InputHandler(ModelNodeBase * model): _model(model)
{
   //_model = new ModelNodeBase();
   initializeModel();
   connect(model, SIGNAL(loadDrill(const std::vector<double> &, const QString &)), this, SLOT(loadDrillSlot(const std::vector<double> &, const QString &)));
   connect(model, 
         SIGNAL(loadDepth(const double &, const double &, const QModelIndex &)), this,
         SLOT(loadDepthSlot(const double &, const double &, const QModelIndex &)));
   connect(model, SIGNAL(loadCoor(const double *, const QModelIndex &)), this,
         SLOT(loadCoorSlot(const double *, const QModelIndex &)));
}

void InputHandler::initializeModel()
{
   _model->setName("Model");
   ModelNodeBase * drills = new ModelNodeBase(_model, ModelEnums::Drills::Drills_size);
   drills->setName("Drills");
   drills->setProperty(2.0, ModelEnums::Drills::EPS);
   drills->setProperty(10.0, ModelEnums::Drills::TEMP);
   drills->setProperty(1.31444677, ModelEnums::Drills::VISC);
   drills->setProperty(8e-3, ModelEnums::Drills::TERC);
   drills->setProperty(2e-3, ModelEnums::Drills::CUNK);
   _model->appendChild(drills);

   ModelNodeBase * dChecked = new ModelNodeBase(_model);
   dChecked->setName("Checked for d");
   ModelNodeBase * d0 = new ModelNodeBase(dChecked, 
         ModelEnums::DProp::DCheck_Size);
   d0->setName("d10");
   d0->setProperty(false, 0);
   d0->setProperty(0, 1);
   d0->setProperty(0, 2);
   //d0->setProperty(0, 3);
   dChecked->appendChild(d0);
   ModelNodeBase * d1 = new ModelNodeBase(dChecked, ModelEnums::DProp::DCheck_Size);
   d1->setName("d15");
   d1->setProperty(false, 0);
   d1->setProperty(0, 1);
   d1->setProperty(0, 2);
  // d1->setProperty(0, 3);
   dChecked->appendChild(d1);
   ModelNodeBase * d2 = new ModelNodeBase(dChecked, ModelEnums::DProp::DCheck_Size);
   d2->setName("d20");
   d2->setProperty(false, 0);
   d2->setProperty(0, 1);
   d2->setProperty(0, 2);
  // d2->setProperty(0, 3);
   dChecked->appendChild(d2);
   ModelNodeBase * d3 = new ModelNodeBase(dChecked, ModelEnums::DProp::DCheck_Size);
   d3->setName("d50");
   d3->setProperty(false, 0);
   d3->setProperty(0, 1);
   d3->setProperty(0, 2);
   //d3->setProperty(0, 3);
   dChecked->appendChild(d3);
   ModelNodeBase * d4 = new ModelNodeBase(dChecked, ModelEnums::DProp::DCheck_Size);
   d4->setName("d60");
   d4->setProperty(false, 0);
   d4->setProperty(0, 1);
   d4->setProperty(0, 2);
  // d4->setProperty(0, 3);
   dChecked->appendChild(d4);
   ModelNodeBase * d5 = new ModelNodeBase(dChecked, ModelEnums::DProp::DCheck_Size);
   d5->setName("d85");
   d5->setProperty(false, 0);
   d5->setProperty(0, 1);
   d5->setProperty(0, 2);
  // d5->setProperty(0, 3);
   dChecked->appendChild(d5);
   
   _model->appendChild(dChecked);

   ModelNodeBase * percCheck = new ModelNodeBase(_model);
   percCheck->setName("Size checks");
   _model->appendChild(percCheck);

   ModelNodeBase * drillsChecks = new ModelNodeBase(_model);
   drillsChecks->setName("Checks");
   _model->appendChild(drillsChecks);

   ModelNodeBase * conductivity = new ModelNodeBase(_model, ModelEnums::CondProp::Coductivity_Size);
   conductivity->setName("CondProp");
   _model->appendChild(conductivity);

   ModelNodeBase * profileTab = new ModelNodeBase(_model, ModelEnums::ProfileTabProp::Profile_Size);
   profileTab->setName("Profile Tab");
   profileTab->setProperty(0,0);//by default type is set to d10
   profileTab->setProperty(0.0,1);//grain size
   profileTab->setProperty(0.0,2);
   ModelNodeBase * map =new ModelNodeBase(profileTab, 
         ModelEnums::MapProp::MapProp_Size);
   map->setName("MapName");
   map->setProperty(0.0, 0);
   map->setProperty(150.0, 1);
   map->setProperty(0.0, 2);
   map->setProperty(150.0, 3);
   map->setProperty(0.0, 4);
   map->setProperty(150.0, 5);
   map->setProperty(0.0, 6);
   map->setProperty(150.0, 7); 
   map->setProperty(1.0, 8);
   profileTab->appendChild(map);

   ModelNodeBase * profiles = new ModelNodeBase(profileTab, 1);
   profiles->setName("Profiles");
   profiles->setProperty(0, 0);
   
   ModelNodeBase * profile = new ModelNodeBase(profiles);
   profile->setName("Profile 1");


   IntPointSet * drillsID = new IntPointSet(1, profile);
   drillsID->setName("array Drill Ids");//remove comma if some properties are added to drillsID
   profile->appendChild(drillsID);

   profiles->appendChild(profile);

   profileTab->appendChild(profiles);

   _model->appendChild(profileTab);

   ModelNodeBase * surfaces = new ModelNodeBase(_model);
   surfaces->setName("Surfaces");
   _model->appendChild(surfaces);

   ModelNodeBase * plan = new ModelNodeBase(_model, 
         ModelEnums::PlanProp::PlanProp_Size);
   plan->setName("Plan Tab");
   plan->setProperty(0.0, ModelEnums::PlanProp::PlanType);
   plan->setProperty(0.0, ModelEnums::PlanProp::PlanFrom);
   plan->setProperty(0.0, ModelEnums::PlanProp::PlanTo);
   plan->setProperty(0.0, ModelEnums::PlanProp::SurfaceID);
   plan->setProperty(0.0, ModelEnums::PlanProp::CalcType); 
   plan->setProperty(0.0, ModelEnums::PlanProp::PlanFromSize);
   plan->setProperty(0.0, ModelEnums::PlanProp::PlanToSize);

   ModelNodeBase * isoline = new ModelNodeBase(plan, 
         ModelEnums::IsolineProp::IsolineProp_Size);
   isoline->setName("Isoline");
   isoline->setProperty(0.0, ModelEnums::IsolineProp::Xmin);
   isoline->setProperty(150.0, ModelEnums::IsolineProp::Xmax);
   isoline->setProperty(0.0, ModelEnums::IsolineProp::Ymin);
   isoline->setProperty(150.0, ModelEnums::IsolineProp::Ymax);
   isoline->setProperty(10, ModelEnums::IsolineProp::Xdiv);
   isoline->setProperty(10, ModelEnums::IsolineProp::Ydiv);
   PointSet * isolineVal = new PointSet(1, isoline);
   isolineVal->setName("isoline value");
   isoline->appendChild(isolineVal);
   plan->appendChild(isoline);

   _model->appendChild(plan);
}

int InputHandler::rowCount(const QModelIndex & parent) const
{
   ModelNodeBase * pNode = indexToPointer(parent);
   if(pNode->name() == "Points")
   {
      return static_cast<PointSet*> (pNode)->rowCount()+1;
   }
   return pNode->rowCount()+1;
}

int InputHandler::columnCount(const QModelIndex & parent) const
{
   if(indexToPointer(parent)->name() == "Drills")
   {//drills have four arguments name, x, y, z
      return 5;
   }
   else
   {//for points %, d and for samples depth1, depth2
      return 2;
   }

   return 1;
}

QVariant InputHandler::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
       if(index.row()<rowCount(parent(index))-1)
       {//if this data exist in model show it
          if(indexToPointer(parent(parent(index))) == 
              _model->child(ModelEnums::Root::DRILLS))
          {  //for depths dont take names
             return QString::number((static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), index.column()+1)).toDouble(), 'f', 2);
          }
          else if(indexToPointer(parent(index)) == 
              _model->child(ModelEnums::Root::DRILLS) && index.column()!=0)
          {//drill properties, without names
             return QString::number((static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), index.column())).toDouble(), 'f', 2);
          }
          else if(indexToPointer(parent(index)) == 
              _model->child(ModelEnums::Root::DRILLS) && index.column()==0)
          {
             return static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), index.column());//for drill names
          }
          else if(indexToPointer(parent(index))->name() == "Points")
          {//Points
             return QString::number((static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), index.column())).toDouble());
          }
       }
       else
       {//if data is additional line
          ModelNodeBase * parentNode = indexToPointer(parent(index));
          if(parentNode == _model->child(ModelEnums::Root::DRILLS))
          {//than this is drills
             if(index.column()==0)
                return _drillName;
             else
             {
                if(_values[index.column()-1] != 
                      std::numeric_limits<size_t>::max())
                {
                   return QString::number(_values[index.column()-1]);
                }
             }
          }
          else if(indexToPointer(parent(parent(index))) == 
              _model->child(ModelEnums::Root::DRILLS))
          {//than this is depths
             if(_depthValues[index.column()] != 
                      std::numeric_limits<size_t>::max())
             {
                return QString::number(_depthValues[index.column()]);
             }
          }

          if(columnCount(parent(index))==2 && 
                _otherValue != std::numeric_limits<size_t>::max() &&
                ((_hasFirst && index.column()==0) || 
                 (!_hasFirst && index.column()==1)))
          {//set to first value
             return QString::number(_otherValue);
          }
          return QString(" ");
       }
    }
    return QVariant();
}

bool InputHandler::setData (const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || role != Qt::EditRole || !(Qt::ItemIsEditable & 
            static_cast<ModelNodeBase*>
            (index.internalPointer())->flags (index.row(), index.column())))
      return false;
  else
  {
     ModelNodeBase * parentNode = indexToPointer(parent(index));
     if(index.row() != parentNode->rowCount())
     {//only change value
        if(parentNode->name()=="Points")
        {//change value of coordinates
           PointSet * points = static_cast<PointSet*> (parentNode);
           double pntOld[2];
           pntOld[0]=points->data(index.row(),0).toDouble();
           pntOld[1]=points->data(index.row(),1).toDouble();
           double pnt[2];
           pnt[0]=index.column()==0 ? value.toDouble() : pntOld[0];
           pnt[1]=index.column()==1 ? value.toDouble() : pntOld[1];

           points->removePoint(index.row());
           int row = points->insertNewPoint(pnt);
           if(row<0)
           {
              points->insertNewPoint(pntOld);
              emit coorNotAdded();
           }
        }
        else//change drills or depths names and properties
        {
           if(parentNode == _model->child(ModelEnums::Root::DRILLS))
           {
              if(index.column()==0)//change name
                 parentNode->child(index.row())->setName(value.toString());
              else//change prop
                 parentNode->child(index.row())->setProperty(value, index.column()-1);
           }else{
              parentNode->child(index.row())->setProperty(value, index.column());
           }
        }
     }
     else
     {//append new value
        if(parentNode == _model->child(ModelEnums::Root::DRILLS))
        {
           if(index.column()==0)
           {
              _drillName = value.toString();
              emit drillDataAppend(index);
           }
           else
           {
              bool ok;
              double val= value.toDouble(&ok);
              if(ok)
              {
                 _values[index.column()-1]=val;
              }
              if(_values[0]!=std::numeric_limits<size_t>::max() &&
                  _values[1]!=std::numeric_limits<size_t>::max() &&
                  _values[2]!=std::numeric_limits<size_t>::max() &&
                  _values[3]!=std::numeric_limits<size_t>::max())
              {  
                 commandDrillAppend();    
              }
              else
                 emit drillDataAppend(index);
           }
        }
        else if(indexToPointer(parent(parent(index))) == 
              _model->child(ModelEnums::Root::DRILLS))
        {
           bool ok;
           double val= value.toDouble(&ok);
           if(ok)
           {
              _depthValues[index.column()]=val;
           }
           if(_depthValues[0]!=std::numeric_limits<size_t>::max() &&
                 _depthValues[1]!=std::numeric_limits<size_t>::max())
           {  
              commandDepthAppend(index);
           }
           else
              emit depthDataAppend(index);
        }
        else//coors
        {
           bool ok;
           double val= value.toDouble(&ok);
           if(ok)
           {
              if(_otherValue==std::numeric_limits<size_t>::max())
              {//set first value
                 _hasFirst = index.column()==0 ? true : false;
                 _otherValue = val;
                 emit coorChanged(index);
              }else //append whole coor
                 commandCoorAppend(index, val);
           }          
        }
     }
  }
  return true;
}

Qt::ItemFlags InputHandler::flags(const QModelIndex &index) const
{
   if (!index.isValid())
        return Qt::ItemIsEnabled;
   return static_cast<ModelNodeBase*>(index.internalPointer())->flags
      (index.row(), index.column());
}

QModelIndex InputHandler::index (int row, int column, const QModelIndex & parent) const
{
   if (!hasIndex (row, column, parent)) return QModelIndex();
   ModelNodeBase * child = (parent.isValid() ? static_cast<ModelNodeBase*>
         (parent.internalPointer())->child (parent.row()) : _model);
   assert (child);
   QModelIndex ind = createIndex (row, column, child);
   return ind;
}


QModelIndex InputHandler::parent (const QModelIndex & index) const
{
    if (!index.isValid()) return QModelIndex();
    ModelNodeBase *parentPtr = static_cast<ModelNodeBase*>
       (index.internalPointer());
    if (parentPtr == NULL) return QModelIndex();
    ModelNodeBase *granParentPtr = parentPtr->parent();
    if (granParentPtr == NULL) return QModelIndex();
    QModelIndex ind = createIndex (parentPtr->row(), 0, granParentPtr);
    return ind;
}

inline ModelNodeBase * InputHandler::indexToPointer (const QModelIndex & parent) const
{
   return (parent.isValid() && parent.internalPointer() ?
         static_cast<ModelNodeBase*> (parent.internalPointer())->child
         (parent.row()) : _model);
}

void InputHandler::deleteNode(const QModelIndex & index) 
{
   ModelNodeBase * parentNode = indexToPointer(parent(index));
   if(index.row() == parentNode->rowCount())//trying to delete non existing
      return;
   QModelIndex parentIndex = parent(index);
   beginRemoveRows(parentIndex, index.row(), index.row());
   if(parentNode->name() == "Points")
   {
      static_cast<PointSet*> (parentNode)->removePoint(index.row());  
      emit pointChanged();
   }else
   {
      parentNode->removeChildren(index.row(),1);
   }
   endRemoveRows();
   emit rowDeleted(index);
   if(parentNode == _model->child(ModelEnums::Root::DRILLS))      
   {
      emit _model->deletedDrill(index.row());
      ModelNodeBase * checks = _model->child(ModelEnums::Root::CHECKS);
      checks->removeChildren(index.row(), 1);
   }
   else if(parentNode->name() != "Points")
   {//delete node in drill sample checks
      ModelNodeBase * checksDrill = _model->child(ModelEnums::Root::CHECKS)->child(parent(index).row());
      emit deleteSpline(parent(index).row(), index.row());
      emit _model->deletedSample(parent(index).row(), index.row());
      checksDrill->removeChildren(index.row(), 1);
   }
}

void InputHandler::commandDrillAppend()
{//append new drill
   ModelNodeBase * drills = _model->child(ModelEnums::Root::DRILLS);
   QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex());
   ModelNodeBase * drill = new ModelNodeBase(drills, 
         ModelEnums::DrillProp::DrillProp_Size);
   drill->setName(_drillName);//seting the name for new drill
   drill->setProperty(_values[0],0);
   drill->setProperty(_values[1],1);
   drill->setProperty(_values[2],2);
   drill->setProperty(_values[3],3);
   _values = std::vector<double> (4, std::numeric_limits<size_t>::max());
   const int row = drills->children()->size();
   beginInsertRows (drillsIndex, row, row);
   drills->appendChild (drill);
   endInsertRows();
   emit addedDrill(index(row+1, 0, drillsIndex));

   ModelNodeBase * drillsChecks = _model->child(
         ModelEnums::Root::CHECKS);
   ModelNodeBase * drillCheck = new ModelNodeBase(drillsChecks, 
         ModelEnums::DrillChecksProp::DrillChecksProp_Size);
   drillCheck->setName(_drillName);
   drillCheck->setProperty(0,0);
   drillCheck->setProperty(0,1);
   drillCheck->setProperty(0,2);
   drillCheck->setProperty(0,3);
   drillCheck->setProperty(0,4);
   drillCheck->setProperty(0,5);
   drillsChecks->appendChild (drillCheck);
   
   _drillName="";
   emit _model->addedDrill(row);
}

void InputHandler::commandDepthAppend(const QModelIndex & index)
{//append new depth

   if(_depthValues[0]>_depthValues[1])
   {
      QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
            QString("ERROR"),
            QString("Depth at first cell must be smaller than depth at second cell!"));
      newError->exec();
      _depthValues[1]=std::numeric_limits<size_t>::max();
      return;
   }
   ModelNodeBase * parentNode = indexToPointer(parent(index));

   ModelNodeBase * depth = new ModelNodeBase(parentNode, 2);
   depth->setName("Sample at ");
   depth->setProperty(_depthValues[0],0);
   depth->setProperty(_depthValues[1],1);
   _depthValues = std::vector<double> (2, std::numeric_limits<size_t>::max());
   PointSet * coors = new PointSet(2, depth);//coors has two coordinates
   coors->setName("Points");//all coors have name Points
   depth->appendChild(coors);
   

   int row=-1;
   for(int i = 0; i<parentNode->rowCount(); i++)
   {
      if(parentNode->child(i)->properties()[0].toDouble() > depth->properties()[0].toDouble())
      {
         if(i!=0 && parentNode->child(i-1)->properties()[1].toDouble() > depth->properties()[0].toDouble())
         {//in conflict with previous interval 
            QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
            QString("ERROR"),
            QString("Sample depth in conflict with existing depth "+parentNode->child(i-1)->properties()[0].toString()+" - "+parentNode->child(i-1)->properties()[1].toString()));
            newError->exec();
            delete depth;
            return;
         }
         else if(parentNode->child(i)->properties()[0].toDouble() < depth->properties()[1].toDouble())
         {//in conflict with next interval
            QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
            QString("ERROR"), QString("Sample depth in conflict with existing depth "+parentNode->child(i)->properties()[0].toString()+" - "+parentNode->child(i)->properties()[1].toString()));
            newError->exec();
            delete depth;
            return;
         }
         else
         {
            row = i;
            break;
         }
      }
      else if(i==parentNode->rowCount()-1 && parentNode->child(i)->properties()[1].toDouble() > depth->properties()[0].toDouble())
      {
         QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
               QString("ERROR"), QString("Sample depth in conflict with existing depth "+parentNode->child(i)->properties()[0].toString()+" - "+parentNode->child(i)->properties()[1].toString()));
         newError->exec();
         delete depth;
         return;
      }
   }


   QModelIndex parentIndex = parent(index);
   int row1 = row==-1 ? index.row() : row;
   beginInsertRows(parentIndex, row1, row1);
   row==-1 ? parentNode->appendChild(depth) : parentNode->insertChild(depth, row);
   endInsertRows();

   QModelIndex sss = this->index(row1+1, 0, parentIndex);
   emit addedDepth(sss);

   ModelNodeBase * drillCheck = _model->child(
         ModelEnums::Root::CHECKS)->child(parent(index).row());

   ModelNodeBase * depthCheck = new ModelNodeBase(drillCheck, ModelEnums::SampleProp::SampleProp_Size);

   depthCheck->setName(" Sample at "+QString::number(depth->properties()[0].toDouble())+" "+QString::number(depth->properties()[1].toDouble()));
   for(size_t i=0;i<ModelEnums::SampleProp::SampleProp_Size; i++)
      depthCheck->setProperty(0,i);

   row == -1 ? drillCheck->appendChild (depthCheck) : drillCheck->insertChild(depth, row);
}

void InputHandler::commandCoorAppend(const QModelIndex & index, 
      const double & v)
{//append new coor
   if(!index.isValid())
   {//maybe throw some error window
      return;
   }
   double pnt[2];
   pnt[0]=index.column()==1 ? _otherValue : v;
   pnt[1]=index.column()==1 ? v : _otherValue;
   
   int row = static_cast<PointSet*> (indexToPointer(parent(index)))->insertNewPoint(pnt);

   if(row<0)
   {
      emit coorNotAdded();
      return;
   }

   beginInsertRows(parent(index), row, row);
   endInsertRows();

   _otherValue=std::numeric_limits<size_t>::max();
   _hasFirst = false;
   emit addedCoor();
}

void InputHandler::commandCoorAppendPicture(const QModelIndex & index, 
      const double & x, const double & y)
{
   if(!index.isValid())
   {//maybe throw some error window
      return;
   }
   double pnt[2];
   pnt[0]=x;
   pnt[1]=y;
 /*  beginInsertRows(parent(index), index.row(), index.row());
   static_cast<PointSet*> (indexToPointer(parent(index)))->appendPoint(pnt);
   endInsertRows();*/
   
   int row = static_cast<PointSet*> (indexToPointer(parent(index)))->insertNewPoint(pnt);
   
   if(row<0)
   {
      emit coorNotAdded();
      return;
   }

   beginInsertRows(parent(index), row, row);
   endInsertRows();

   _otherValue=std::numeric_limits<size_t>::max();
   _hasFirst = false;
   emit addedCoor();
}

double InputHandler::pointValue(const QModelIndex & index, const int row, const int col) const
{
//   ModelNodeBase * node = indexToPointer(index);
   PointSet * points = static_cast<PointSet*> (indexToPointer(index));
   return points->getPoint(row)[col];
}

QVariant InputHandler::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
   if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return QVariant("Name");
 
            case 1:
                return QVariant("X");
            case 2:
                return QVariant("Y");
            case 3:
                return QVariant("elevation");
            default:
                return QVariant("depth");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

int InputHandler::unknownValue()
{
   for(int i=0;i<4;i++)
      if(_values[i] == std::numeric_limits<size_t>::max())
         return i;
   return 0;//should never happen because signal drillDataAppend is triggered only if one of _values is max 
}

int InputHandler::unknownDepthValue()
{
   return _values[0] == std::numeric_limits<size_t>::max() ? 0 : 1;
}

const QString InputHandler::name(const QModelIndex & index)
{
   return indexToPointer(index)->name();
}

void InputHandler::loadDrillSlot(const std::vector<double> & v, 
      const QString & name)
{
   ModelNodeBase * drills = _model->child(ModelEnums::Root::DRILLS);
   QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex());
   ModelNodeBase * drill = new ModelNodeBase(drills, ModelEnums::DrillProp::DrillProp_Size);
   drill->setName(name);//seting the name for new drill
   drill->setProperty(v[0],0);
   drill->setProperty(v[1],1);
   drill->setProperty(v[2],2);
   drill->setProperty(v[3],3);
   const int row = drills->children()->size();
   beginInsertRows (drillsIndex, row, row);
   drills->appendChild (drill);
   endInsertRows();
   emit addedDrill(index(row+1, 0, drillsIndex));
   //emit _model->addedDrill(row);

   ModelNodeBase * drillsChecks = _model->child(
         ModelEnums::Root::CHECKS);
   ModelNodeBase * drillCheck = new ModelNodeBase(drillsChecks, 
         ModelEnums::DrillChecksProp::DrillChecksProp_Size);
   drillCheck->setName(name);
   drillCheck->setProperty(0,0);
   drillCheck->setProperty(0,1);
   drillCheck->setProperty(0,2);
   drillCheck->setProperty(0,3);
   drillCheck->setProperty(0,4);
   drillCheck->setProperty(0,5);
   drillsChecks->appendChild (drillCheck);
   emit _model->addedDrill(row);
}

void InputHandler::loadDepthSlot(const double & v0, const double & v1, 
      const QModelIndex & depthIndex)
{
   //append new depth
   ModelNodeBase * drills = _model->child(ModelEnums::Root::DRILLS);
   const int idrill = depthIndex.isValid() ? parent(depthIndex).row() : drills->children()->size()-1;
   
   ModelNodeBase * parentNode = drills->child(idrill);

   ModelNodeBase * depth = new ModelNodeBase(parentNode, 2);
   depth->setName("Sample at ");
   depth->setProperty(v0,0);
   depth->setProperty(v1,1);
   PointSet * coors = new PointSet(2, depth);//coors has two coordinates
   coors->setName("Points");//all coors have name Points
   depth->appendChild(coors);

   int row=-1;
   for(int i = 0; i<parentNode->rowCount(); i++)
   {
      if(parentNode->child(i)->properties()[0].toDouble() > depth->properties()[0].toDouble())
      {
         if(i!=0 && parentNode->child(i-1)->properties()[1].toDouble() > depth->properties()[0].toDouble())
         {//in conflict with previous interval 
            QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
            QString("ERROR"),
            QString("Sample depth in conflict with existing depth "+parentNode->child(i-1)->properties()[0].toString()+" - "+parentNode->child(i-1)->properties()[1].toString()));
            newError->exec();
            delete depth;
            return;
         }
         else if(parentNode->child(i)->properties()[0].toDouble() < depth->properties()[1].toDouble())
         {//in conflict with next interval
            QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
            QString("ERROR"), QString("Sample depth in conflict with existing depth "+parentNode->child(i)->properties()[0].toString()+" - "+parentNode->child(i)->properties()[1].toString()));
            newError->exec();
            delete depth;
            return;
         }
         else
         {
            row = i;
            break;
         }
      }
      else if(i==parentNode->rowCount()-1 && parentNode->child(i)->properties()[1].toDouble() > depth->properties()[0].toDouble())
      {
         QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
               QString("ERROR"), QString("Sample depth in conflict with existing depth "+parentNode->child(i)->properties()[0].toString()+" - "+parentNode->child(i)->properties()[1].toString()));
         newError->exec();
         delete depth;
         return;
      }
   }

  QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex()); 
  QModelIndex drillIndex = index(idrill, 0, drillsIndex);
  int irow = row == -1 ? parentNode->rowCount(): depthIndex.row();
  beginInsertRows(drillIndex, irow, irow);
  row==-1 ? parentNode->appendChild(depth) : parentNode->insertChild(depth, row);
  endInsertRows();
   
  // QModelIndex sss = this->index(drillsIndex.row()+1, 0, parentIndex);
  // emit addedDepth(sss);

   ModelNodeBase * drillCheck = _model->child(ModelEnums::Root::CHECKS)->child(idrill);

   ModelNodeBase * depthCheck = new ModelNodeBase(drillCheck, ModelEnums::SampleProp::SampleProp_Size);

   depthCheck->setName(" Sample at "+QString::number(depth->properties()[0].toDouble())+" "+QString::number(depth->properties()[1].toDouble()));
   for(size_t i=0;i<ModelEnums::SampleProp::SampleProp_Size; i++)
      depthCheck->setProperty(0,i);

   //drillCheck->appendChild (depthCheck);
   row == -1 ? drillCheck->appendChild (depthCheck) : drillCheck->insertChild(depth, row);
}

void InputHandler::loadCoorSlot(const double * pnt, const QModelIndex & coorIndex)
{//append new coor
   ModelNodeBase * drills = _model->child(ModelEnums::Root::DRILLS);
   const int idrill = coorIndex.isValid() ? parent(parent(parent(coorIndex))).row() : drills->children()->size()-1;
   QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex());
   ModelNodeBase * drill = drills->child(idrill);
   QModelIndex drillIndex = index(idrill, 0, drillsIndex);
   const int idepth = coorIndex.isValid() ? parent(parent(coorIndex)).row() : drill->children()->size()-1;
   QModelIndex depthIndex = index(idepth, 0, drillIndex);
   ModelNodeBase * depth = drill->child(idepth);
   PointSet * points = static_cast<PointSet*> (depth->child(0));

   
   int irow = points->insertNewPoint(pnt);
   if(irow>=0)
   {      
      beginInsertRows(index(0,0,depthIndex), irow, irow);
      endInsertRows();
   }
}

void InputHandler::appendToNodeBase (const QModelIndex & parent, 
      ModelNodeBase* p)
{ // Appends a child to ModelNodeBase.
   ModelNodeBase* node = static_cast<ModelNodeBase*>(static_cast<ModelNodeBase*>
         (parent.internalPointer() ? parent.internalPointer() : _model)->child
         (parent.row()));
   const int row = node->children()->size();
   beginInsertRows (parent, row, row);
   node->appendChild (p);
   endInsertRows();
}



