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
#include "sizeHandler.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/pointSet.h"

#include <assert.h>

SizeHandler::SizeHandler(ModelNodeBase * model):_model(model)
{
   //int ndrills = _model->child(ModelEnums::Root::DRILLS)->rowCount();
//   _checked.push_back(1);
  
}

int SizeHandler::rowCount(const QModelIndex & parent) const
{
   return indexToPointer(parent)->rowCount()+1;
}

int SizeHandler::columnCount(const QModelIndex & /*parent*/) const 
{
   return 4;
}

QVariant SizeHandler::data(const QModelIndex & index, int role) const
{
   if (role == Qt::DisplayRole || role == Qt::EditRole)
   {
      //qDebug()<<index<<" imas da je "<<role<<"\n";
    //  if(index.column()==0)//check
    //     return static_cast<ModelNodeBase*>(index.internalPointer())->data
    //        (index.row(), ModelEnums::DrillProp::Check+1);
     // else     
      if(index.row()<rowCount(parent(index))-1)
      {
      if(index.column()==0)
         return QString::number((static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::SizeRangeProp::From+1)).toDouble());
      else if(index.column()==1)
         return QString::number((static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::SizeRangeProp::To+1)).toDouble());
      else if(index.column()==2)
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::SizeRangeProp::PercColor+1);
      else if(index.column()==3)
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), ModelEnums::SizeRangeProp::PercLine+1);
      }
      else
      {//if data is additional line
         if(index.column()==0)
         {
            if(_sizeValues[0]!=std::numeric_limits<size_t>::max())
            {
               return _sizeValues[0];
            }
            else
               return QVariant();
         }            
         else if(index.column()==1)
         {
            if(_sizeValues[1]!=std::numeric_limits<size_t>::max())
            {
               return _sizeValues[1];
            }
            else
               return QVariant();
         }
         else
            return _values[index.column()-2];
      }

   }
   if(index.column()==0 && role == Qt::CheckStateRole)//check
   {
      if(index.row()<rowCount(parent(index))-1)
         return QVariant(static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::SizeRangeProp::PCheck+1).toBool());
      else
         return QVariant(true);
   }
   return QVariant();
}

bool SizeHandler::setData (const QModelIndex &index, const QVariant & value, int role)
{
   if(role == Qt::CheckStateRole)
   {
      if(index.row()<rowCount(parent(index))-1)
      {
         ModelNodeBase * parentNode = indexToPointer(parent(index));
         if(index.column()==0)//change check
         {
            bool newVal = !parentNode->child(index.row())->properties()[ModelEnums::SizeRangeProp::PCheck].toBool();
            parentNode->child(index.row())->setProperty(newVal,
                  ModelEnums::SizeRangeProp::PCheck);
            emit sizeSelectionChange(index.row());
         }
         return true;
      }
      else
         return false;
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
      if(index.row() != parentNode->rowCount())
      {//only change value
         if(index.column()==0)//change from
         {
            parentNode->child(index.row())->setProperty(value, ModelEnums::SizeRangeProp::From);
            emit sizeSelectionChange(index.row());
         }
         if(index.column()==1)//change to
         {
            parentNode->child(index.row())->setProperty(value, ModelEnums::SizeRangeProp::To);
            emit sizeSelectionChange(index.row());
         }
         if(index.column()==2)//change color
         {
            parentNode->child(index.row())->setProperty(value, ModelEnums::SizeRangeProp::PercColor);
            emit styleChange(index.row());
         }
         else if(index.column()==3)
         {
            parentNode->child(index.row())->setProperty(value, ModelEnums::SizeRangeProp::PercLine);
            emit styleChange(index.row());
         }
      }else
      {//append new
         if(index.column()==0 || index.column()==1)
         {
            bool ok;
            double val= value.toDouble(&ok);
            if(ok)
            {
               _sizeValues[index.column()]=val;
            }
            if(_sizeValues[0]!=std::numeric_limits<size_t>::max() &&
                 _sizeValues[1]!=std::numeric_limits<size_t>::max())
            {  
               commandSizeAppend();
            }
            emit sizeDataAppend(index.row(), index.column());
         }
         else
         {
            _values[index.column()-2]=value;
         }

      }
   }

   return true;
}

Qt::ItemFlags SizeHandler::flags(const QModelIndex & index) const
{
   if(index.column()==0)
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

}

QModelIndex SizeHandler::index (int row, int column, 
      const QModelIndex & parent) const
{
   if (!hasIndex (row, column, parent)) return QModelIndex();
   ModelNodeBase * child = (parent.isValid() ? static_cast<ModelNodeBase*>
         (parent.internalPointer())->child (parent.row()) : _model);
   assert (child);
   QModelIndex ind = createIndex (row, column, child);
   return ind;
}


QModelIndex SizeHandler::parent (const QModelIndex & index) const
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

inline ModelNodeBase * SizeHandler::indexToPointer (const QModelIndex & parent) const
{
   return (parent.isValid() && parent.internalPointer() ?
         static_cast<ModelNodeBase*> (parent.internalPointer())->child
         (parent.row()) : _model);
}

QVariant SizeHandler::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
   if (orientation == Qt::Horizontal)
   {
      switch (section)
      {
         case 0:
            return QVariant("From"); 
         case 1:
            return QVariant("To");
         case 2:
            return QVariant("Color");
         case 3:
            return QVariant("Line");
      }
   }
    
   return QAbstractItemModel::headerData(section, orientation, role);
}

int SizeHandler::nsizes()
{
   return _model->child(ModelEnums::Root::PERCCHECK)->rowCount();
}

double SizeHandler::from(const int & id)
{
   return _model->child(ModelEnums::Root::PERCCHECK)->child(id)->properties()[ModelEnums::SizeRangeProp::From].toDouble();
}

double SizeHandler::to(const int & id)
{
   return _model->child(ModelEnums::Root::PERCCHECK)->child(id)->properties()[ModelEnums::SizeRangeProp::To].toDouble();
}

bool SizeHandler::isChecked(const int & id)
{
   return _model->child(ModelEnums::Root::PERCCHECK)->child(id)->properties()[ModelEnums::SizeRangeProp::PCheck].toBool();
}

int SizeHandler::sizeStyle(const int & id)
{
   return _model->child(ModelEnums::Root::PERCCHECK)->child(id)->properties()[ModelEnums::SizeRangeProp::PercLine].toInt()+1;
}

int SizeHandler::sizeColor(const int & id)
{
   return _model->child(ModelEnums::Root::PERCCHECK)->child(id)->properties()[ModelEnums::SizeRangeProp::PercColor].toInt();
}


void SizeHandler::commandSizeAppend()
{
   ModelNodeBase * percCheck = _model->child(ModelEnums::Root::PERCCHECK);
   QModelIndex percCheckIndex = index(ModelEnums::Root::PERCCHECK, 0,QModelIndex());
   ModelNodeBase * range = new ModelNodeBase(percCheck, 
         ModelEnums::SizeRangeProp::SizeRangeProp_Size);
   range->setName("Range");
   range->setProperty(_sizeValues[0], 0);
   range->setProperty(_sizeValues[1], 1);
   range->setProperty(true, 2);
   range->setProperty(_values[0], 3);
   range->setProperty(_values[1], 4);
   _sizeValues = std::vector<double>(2, std::numeric_limits<size_t>::max());
   _values = std::vector<QVariant>(2, QVariant(0));

   const int row = percCheck->children()->size();
   beginInsertRows (percCheckIndex, row, row);
   percCheck->appendChild (range);
   endInsertRows();
   emit sizeAppended(row);

}

void SizeHandler::commandSizeAppendLoad(const std::vector<double> & v)
{
   ModelNodeBase * percCheck = _model->child(ModelEnums::Root::PERCCHECK);
   QModelIndex percCheckIndex = index(ModelEnums::Root::PERCCHECK, 0,QModelIndex());
   ModelNodeBase * range = new ModelNodeBase(percCheck, 
         ModelEnums::SizeRangeProp::SizeRangeProp_Size);
   range->setName("Range");
   range->setProperty(v[0], 0);
   range->setProperty(v[1], 1);
   range->setProperty(v[2], 2);
   range->setProperty(v[3], 3);
   range->setProperty(v[4], 4);

   const int row = percCheck->children()->size();
   beginInsertRows (percCheckIndex, row, row);
   percCheck->appendChild (range);
   endInsertRows();
   emit sizeAppended(row);

}
/*
QString PercHandler::dName(const int & id)
{
   return _model->child(ModelEnums::Root::DCHECKED)->child(id)->name();
}*/


