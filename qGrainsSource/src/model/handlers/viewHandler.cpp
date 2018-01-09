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
#include "viewHandler.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/pointSet.h"
#include "model/intPointSet.h"

#include <assert.h>
#include <QDebug>

ViewHandler::ViewHandler(ModelNodeBase * model, const int & var):_model(model), _variant(var)
{

   //int ndrills = _model->child(ModelEnums::Root::DRILLS)->rowCount();
//   _checked.push_back(1);
  
}

int ViewHandler::rowCount(const QModelIndex & parent) const
{
   return indexToPointer(parent)->rowCount();
}

int ViewHandler::columnCount(const QModelIndex & /*parent*/) const 
{
   return 2;
}

QVariant ViewHandler::data(const QModelIndex & index, int role) const
{
   if (role == Qt::DisplayRole)
   {
      //qDebug()<<index<<" imas da je "<<role<<"\n";
    //  if(index.column()==0)//check
    //     return static_cast<ModelNodeBase*>(index.internalPointer())->data
    //        (index.row(), ModelEnums::DrillProp::Check+1);
     // else
      if(index.column()==0)//name
      {
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), 0);
      }
      else if(index.column()==1)
      {
         if(_variant==1)
         {
            return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DrillChecksProp::PercTick+1);
         }else if(_variant==0)
         {
            return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DrillChecksProp::dTick+1);
         }else if(_variant==2)
         {
            return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DrillChecksProp::ProfileColor+1);
         }

      }
    /*  else if(index.column()==1)
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DrillProp::Color+1);
      else if(index.column()==2)
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DrillProp::Line+1);*/

   }
   if(index.column()==0 && role == Qt::CheckStateRole)//check
   {
      if(_variant==1)
         return QVariant(static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::DrillChecksProp::PercDrillCheck+1).toBool());
      else if(_variant==0)       
         return QVariant(static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::DrillChecksProp::Check+1).toBool());
      else if(_variant==2)
      {         
         ModelNodeBase * profiles = static_cast<ModelNodeBase*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles));
         int active = profiles->properties()[0].toInt();
         IntPointSet * profileArray = static_cast<IntPointSet*> (profiles->child(active)->child(0));
         return profileArray->isPresent(index.row());
            //QVariant(static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::DrillChecksProp::ProfileCheck+1).toBool());
      }
   }
   return QVariant();
}

bool ViewHandler::setData (const QModelIndex &index, const QVariant & value, int role)
{
   if(role == Qt::CheckStateRole)
   {
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      if(index.column()==0)//change check
      {
         if(_variant==1)
         {
            bool newVal = !parentNode->child(index.row())->properties()[ModelEnums::DrillChecksProp::PercDrillCheck].toBool();
            parentNode->child(index.row())->setProperty(newVal,
                  ModelEnums::DrillChecksProp::PercDrillCheck);
            emit drillPercSelectionChange(index.row());
         }
         else if(_variant==0)
         {
            bool newVal = !parentNode->child(index.row())->properties()[ModelEnums::DrillChecksProp::Check].toBool();
            parentNode->child(index.row())->setProperty(newVal,
                  ModelEnums::DrillChecksProp::Check);
            emit drillSelectionChange(index.row());
         }
         else if(_variant==2)
         {
            ModelNodeBase * profiles = static_cast<IntPointSet*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles));
            int active = profiles->properties()[0].toInt();
            IntPointSet * profileArray = static_cast<IntPointSet*> (profiles->child(active)->child(0));
            bool newVal = !profileArray->isPresent(index.row());

            int idrill = index.row();
            if(newVal)
               profileArray->appendPoint(&idrill);
            else
               profileArray->findAndRemovePoint(idrill);

            emit drillProfSelectionChange(index.row());
         }


      }
      return true;
   }
   if (!index.isValid() || role != Qt::EditRole || !(Qt::ItemIsEditable & 
            static_cast<ModelNodeBase*>
            (index.internalPointer())->flags (index.row(), index.column())))
   {
      return false;
   }
   else if(index.column()==1)
   {
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      if(_variant==1)
      {
         parentNode->child(index.row())->setProperty(value.toInt(),
                  ModelEnums::DrillChecksProp::PercTick);
         emit drillPercTickChange(index.row());
      }
      else if(_variant == 0)
      {
         parentNode->child(index.row())->setProperty(value.toInt(),
                  ModelEnums::DrillChecksProp::dTick);
         emit drillTickChange(index.row());
      }
      else if(_variant==2)
      {
         parentNode->child(index.row())->setProperty(value.toInt(),
                  ModelEnums::DrillChecksProp::ProfileColor);
         emit drillProfColorChange(index.row());
      }
      return true;
   }
   /*
   else
   {
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      if(index.column()==1)//change color
         parentNode->child(index.row())->setProperty(value, ModelEnums::DrillProp::Color);
      else if(index.column()==2)
         parentNode->child(index.row())->setProperty(value, ModelEnums::DrillProp::Line);
   }*/


   qDebug()<<" ERROR view handler setData ";//should never hapen
   return true;
}

Qt::ItemFlags ViewHandler::flags(const QModelIndex & index) const
{
   if(index.column()==0)
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

}

QModelIndex ViewHandler::index (int row, int column, 
      const QModelIndex & parent) const
{
   if (!hasIndex (row, column, parent)) return QModelIndex();
   ModelNodeBase * child = (parent.isValid() ? static_cast<ModelNodeBase*>
         (parent.internalPointer())->child (parent.row()) : _model);
   assert (child);
   QModelIndex ind = createIndex (row, column, child);
   return ind;
}


QModelIndex ViewHandler::parent (const QModelIndex & index) const
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

inline ModelNodeBase * ViewHandler::indexToPointer (const QModelIndex & parent) const
{
   return (parent.isValid() && parent.internalPointer() ?
         static_cast<ModelNodeBase*> (parent.internalPointer())->child
         (parent.row()) : _model);
}

QVariant ViewHandler::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
   if (orientation == Qt::Horizontal)
   {
      //return QVariant("Drills");
      switch (section)
      {
         case 0:
            return QVariant("Drills"); 
         default:
            return _variant==2 ? QVariant("Color") : QVariant("Tick");
      }
   }
    
   return QAbstractItemModel::headerData(section, orientation, role);
}

bool ViewHandler::isCheckedDrill(const int & idrill)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->properties()[ModelEnums::DrillChecksProp::Check].toBool();
}

bool ViewHandler::isCheckedDrillPerc(const int & idrill)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->properties()[ModelEnums::DrillChecksProp::PercDrillCheck].toBool();
}

bool ViewHandler::isCheckedDrillProfile(const int & idrill)
{
   ModelNodeBase * profiles = static_cast<IntPointSet*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles));
   
   int active = profiles->properties()[0].toInt();
   
   IntPointSet * profileArray = static_cast<IntPointSet*> (profiles->child(active)->child(0));
   return profileArray->isPresent(idrill);
}

void ViewHandler::changeCheckProfile(const int & idrill)
{
   ModelNodeBase * profiles = static_cast<IntPointSet*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles));
   
   int active = profiles->properties()[0].toInt();
   
   IntPointSet * profileArray = static_cast<IntPointSet*> (profiles->child(active)->child(0));
   bool val = !profileArray->isPresent(idrill);

   QModelIndex drillsIndex = createIndex(ModelEnums::Root::CHECKS,0,_model);

   if(val)
      profileArray->appendPoint(&idrill);
   else
      profileArray->findAndRemovePoint(idrill);

   emit dataChanged(this->index(idrill, 0, drillsIndex), 
            this->index(idrill, 0, drillsIndex));
   emit drillProfSelectionChange(idrill);
}

int ViewHandler::profileColor(const int & idrill)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->properties()[ModelEnums::DrillChecksProp::ProfileColor].toInt();
}

void ViewHandler::setTypeCalc(const int & type)
{
   _model->child(ModelEnums::Root::PROFILETAB)->setProperty(type, ModelEnums::ProfileTabProp::ProfType);
   emit typeCalcChanged();
}

void ViewHandler::setProfileFrom(const double & value)
{
   _model->child(ModelEnums::Root::PROFILETAB)->setProperty(value,
         ModelEnums::ProfileTabProp::ProfFrom);
   emit typeCalcChanged();
}

void ViewHandler::setProfileTo(const double & value)
{
   _model->child(ModelEnums::Root::PROFILETAB)->setProperty(value,
         ModelEnums::ProfileTabProp::ProfTo);
   emit typeCalcChanged();
}

int ViewHandler::drillTick(const int & idrill)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->properties()[ModelEnums::DrillChecksProp::dTick].toInt()+1;
}

int ViewHandler::drillPercTick(const int & idrill)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->properties()[ModelEnums::DrillChecksProp::PercTick].toInt()+1;
}

QString ViewHandler::drillName(const int & idrill)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->name();
}

int ViewHandler::nDrills()
{
   return _model->child(ModelEnums::Root::CHECKS)->rowCount();
}

int ViewHandler::nSamples(const int & idrill)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->rowCount();
}

double ViewHandler::sampleVal(const int & idrill, const int & isample, 
      const int & iv)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(isample)->properties()[iv].toDouble();
}

void ViewHandler::addDrill(const int & row)
{
   QModelIndex drillsIndex = index(ModelEnums::Root::CHECKS, 0, QModelIndex());
   beginInsertRows (drillsIndex, row, row);
   endInsertRows();
}

void ViewHandler::deleteDrill(const int & row)
{
   QModelIndex drillsIndex = index(ModelEnums::Root::CHECKS, 0, QModelIndex());

   beginRemoveRows(drillsIndex, row, row);
   endRemoveRows();
}

void ViewHandler::profileChanged(const int & iprofile)
{
   ModelNodeBase * profiles = static_cast<IntPointSet*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles));

   profiles->setProperty(iprofile, 0);

   QModelIndex drillsIndex = index(ModelEnums::Root::CHECKS, 0, QModelIndex());
   emit dataChanged(this->index(0, 0, drillsIndex), 
            this->index(rowCount(drillsIndex), 0, drillsIndex));
}

