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
#include "dHandler.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/pointSet.h"

#include <assert.h>

DHandler::DHandler(ModelNodeBase * model):_model(model)
{
   //int ndrills = _model->child(ModelEnums::Root::DRILLS)->rowCount();
//   _checked.push_back(1);
  
}

int DHandler::rowCount(const QModelIndex & parent) const
{
   return indexToPointer(parent)->rowCount();
}

int DHandler::columnCount(const QModelIndex & /*parent*/) const 
{
   return 3;
}

QVariant DHandler::data(const QModelIndex & index, int role) const
{
   if (role == Qt::DisplayRole)
   {
      //qDebug()<<index<<" imas da je "<<role<<"\n";
    //  if(index.column()==0)//check
    //     return static_cast<ModelNodeBase*>(index.internalPointer())->data
    //        (index.row(), ModelEnums::DrillProp::Check+1);
     // else
      if(index.column()==0)//name
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), 0);
      else if(index.column()==1)
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DProp::Color+1);
      else if(index.column()==2)
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DProp::Line+1);
    /*  else if(index.column()==3)
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::DProp::Tick+1);*/

   }
   if(index.column()==0 && role == Qt::CheckStateRole)//check
   {
         return QVariant(static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::DProp::DCheck+1).toBool());
//         return QVariant(true);
   }
   return QVariant();
}

bool DHandler::setData (const QModelIndex &index, const QVariant & value, int role)
{
   qDebug()<<" usao u set data \n";
   if(role == Qt::CheckStateRole)
   {
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      if(index.column()==0)//change check
      {
         bool newVal = !parentNode->child(index.row())->properties()[ModelEnums::DProp::DCheck].toBool();
         parentNode->child(index.row())->setProperty(newVal,
               ModelEnums::DProp::DCheck);
         qDebug()<<" izemitovan je signal \n";
         emit dSelectionChange(index.row());
      }
      return true;
   }
   if (!index.isValid() || role != Qt::EditRole || !(Qt::ItemIsEditable & 
            static_cast<ModelNodeBase*>
            (index.internalPointer())->flags (index.row(), index.column())))
   {
      return false;
   }
   else
   {
      ModelNodeBase * parentNode = indexToPointer(parent(index));
     /* if(index.column()==0)//change check
         parentNode->child(index.row())->setProperty(value, ModelEnums::DrillProp::Check);
      else*/ 
      if(index.column()==1)//change color
      {
         parentNode->child(index.row())->setProperty(value, ModelEnums::DProp::Color);
         emit styleChange(index.row());
      }
      else if(index.column()==2)
      {
         parentNode->child(index.row())->setProperty(value, ModelEnums::DProp::Line);
         emit styleChange(index.row());
      }
   /*   else if(index.column()==3)
      {
         parentNode->child(index.row())->setProperty(value, ModelEnums::DProp::Tick);
         emit styleChange(index.row());
      }*/
   }

   return true;
}

Qt::ItemFlags DHandler::flags(const QModelIndex & index) const
{
   if(index.column()==0)
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

}

QModelIndex DHandler::index (int row, int column, 
      const QModelIndex & parent) const
{
   if (!hasIndex (row, column, parent)) return QModelIndex();
   ModelNodeBase * child = (parent.isValid() ? static_cast<ModelNodeBase*>
         (parent.internalPointer())->child (parent.row()) : _model);
   assert (child);
   QModelIndex ind = createIndex (row, column, child);
   return ind;
}


QModelIndex DHandler::parent (const QModelIndex & index) const
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

inline ModelNodeBase * DHandler::indexToPointer (const QModelIndex & parent) const
{
   return (parent.isValid() && parent.internalPointer() ?
         static_cast<ModelNodeBase*> (parent.internalPointer())->child
         (parent.row()) : _model);
}

QVariant DHandler::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
   if (orientation == Qt::Horizontal)
   {
      switch (section)
      {
         case 0:
            return QVariant("Percentage"); 
         case 1:
            return QVariant("Color");
         case 2:
            return QVariant("Line");
         /*default:
            return QVariant("Tick");*/
      }
   }
    
   return QAbstractItemModel::headerData(section, orientation, role);
}

void DHandler::setDLoad(const std::vector<double> & v, 
      const QString & name, const int & id)
{
   _model->child(ModelEnums::Root::DCHECKED)->child(id)->setName(name);
   _model->child(ModelEnums::Root::DCHECKED)->child(id)->setProperty(v[0], 0);
   _model->child(ModelEnums::Root::DCHECKED)->child(id)->setProperty(v[1], 1);
   _model->child(ModelEnums::Root::DCHECKED)->child(id)->setProperty(v[2], 2);
   //_model->child(ModelEnums::Root::DCHECKED)->child(id)->setProperty(v[3], 3);
}

bool DHandler::isCheckedD(const int & id)
{
   return _model->child(ModelEnums::Root::DCHECKED)->child(id)->properties()[ModelEnums::DProp::DCheck].toBool();
}

int DHandler::dStyle(const int & id)
{
   return _model->child(ModelEnums::Root::DCHECKED)->child(id)->properties()[ModelEnums::DProp::Line].toInt()+1;
}

int DHandler::dColor(const int & id)
{
   return _model->child(ModelEnums::Root::DCHECKED)->child(id)->properties()[ModelEnums::DProp::Color].toInt();
}

/*int DHandler::dTick(const int & id)
{
   return _model->child(ModelEnums::Root::DCHECKED)->child(id)->properties()[ModelEnums::DProp::Tick].toInt()+1;
}*/

QString DHandler::dName(const int & id)
{
   return _model->child(ModelEnums::Root::DCHECKED)->child(id)->name();
}


