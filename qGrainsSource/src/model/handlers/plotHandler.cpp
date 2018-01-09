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
#include "plotHandler.h"
#include "model/modelNodeBase.h"
#include "model/pointSet.h"
#include "model/modelEnums.h"
#include <QDebug>
#include <assert.h>

PlotHandler::PlotHandler(ModelNodeBase * model):_model(model)
{
}

int PlotHandler::rowCount(const QModelIndex & parent) const
{
   ModelNodeBase * pNode = indexToPointer(parent);
   if(pNode->name() == "Points")
   {
      return static_cast<PointSet*> (pNode)->rowCount();
   }
   return pNode->rowCount();
}

int PlotHandler::columnCount(const QModelIndex & parent) const
{
   if(indexToPointer(parent)->name() == "Drills")
   {//drills have four arguments name, x, y, z
      return 5;
   }
   else if(indexToPointer(parent)->name() == "Points")
   {
      return 2;
   }
   else
   {//for samples depth1, depth2
      return 4;
   }

   return 1;
}

QVariant PlotHandler::data(const QModelIndex &index, int role) const
{

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
       if(index.isValid() && index.row()<rowCount(parent(index)))
       {//if this data exist in model show it
          if(indexToPointer(parent(parent(index))) == 
              _model->child(ModelEnums::Root::DRILLS))
          {   //for depths dont take names
             if(index.column()<2)
                return QString::number((static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), index.column()+1)).toDouble(), 'f', 2);
             else if(index.column()==2)
             {
                return _model->child(ModelEnums::Root::CHECKS)->child(parent(index).row())->child(index.row())->properties()[ModelEnums::SampleProp::SampleColor];
             }
             else if(index.column()==3)
             {
                return _model->child(ModelEnums::Root::CHECKS)->child(parent(index).row())->child(index.row())->properties()[ModelEnums::SampleProp::SampleLine];
             }
          }
          else if(indexToPointer(parent(index)) == 
              _model->child(ModelEnums::Root::DRILLS) && index.column()!=0)
          {//drill properties, without names
             return QString::number((static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), index.column())).toDouble(), 'f', 2);
          }
          return static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), index.column());

       }
    }
    if(index.column()==0 && role == Qt::CheckStateRole)//check
    {
       ModelNodeBase * parentNode = indexToPointer(parent(index));
       if(parentNode->name()=="Drills")
          return QVariant(_model->child(ModelEnums::Root::CHECKS)->child(index.row())->properties()[ModelEnums::DrillChecksProp::PlotCheck].toBool());
       else if(parentNode->name()=="Points")
       {
          return QVariant();
       }
       else if(parentNode->name()=="Model")
       {
          return QVariant();
       }
       else//this is samples
       {
          return QVariant(_model->child(ModelEnums::Root::CHECKS)->child(parent(index).row())->child(index.row())->properties()[ModelEnums::SampleProp::SampleCheck].toBool());
       }

//         return QVariant(static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), ModelEnums::DrillChecksProp::PlotCheck+1).toBool());
        // return QVariant(true);
    }
    return QVariant();
}

bool PlotHandler::setData (const QModelIndex &index, const QVariant &value, int role)
{
   if(role == Qt::CheckStateRole)
   {
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      if(index.column()==0)//change check
      {
         if(parentNode->name()=="Drills")
         {
            bool newVal = !_model->child(ModelEnums::Root::CHECKS)->child(index.row())->properties()[ModelEnums::DrillChecksProp::PlotCheck].toBool();
            _model->child(ModelEnums::Root::CHECKS)->child(index.row())->setProperty(newVal, ModelEnums::DrillChecksProp::PlotCheck);
            int limit = _model->child(ModelEnums::Root::CHECKS)->child(index.row())->rowCount();
            for(int i=0;i<limit;i++)
            {
               _model->child(ModelEnums::Root::CHECKS)->child(index.row())->child(i)->setProperty(newVal, ModelEnums::SampleProp::SampleCheck);
               emit drawSpline(index.row(), i);
            }
            emit dataChanged(this->index(0, 0, index), 
                  this->index(limit, 0, index));
         } 
         else//this is samples
         {
            bool newVal = !_model->child(ModelEnums::Root::CHECKS)->child(parent(index).row())->child(index.row())->properties()[ModelEnums::SampleProp::SampleCheck].toBool();
            _model->child(ModelEnums::Root::CHECKS)->child(parent(index).row())->child(index.row())->setProperty(newVal, ModelEnums::SampleProp::SampleCheck);
            emit drawSpline(parent(index).row(), index.row());
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
   if(role == Qt::EditRole && indexToPointer(parent(parent(index))) == 
              _model->child(ModelEnums::Root::DRILLS))
   {
      if(index.column() == 2)
      {
         _model->child(ModelEnums::Root::CHECKS)->child(parent(index).row())->child(index.row())->setProperty(value, ModelEnums::SampleProp::SampleColor);
      }else if(index.column() == 3)
      {
         _model->child(ModelEnums::Root::CHECKS)->child(parent(index).row())->child(index.row())->setProperty(value, ModelEnums::SampleProp::SampleLine);
      }
      emit changeStyle();
      return true;
   }

   qDebug()<<" ERROR plot handler setData ";//should never hapen
   return true;
}

Qt::ItemFlags PlotHandler::flags(const QModelIndex & index) const
{
   if(indexToPointer(parent(parent(index))) == 
              _model->child(ModelEnums::Root::DRILLS) && 
              (index.column() == 2 || index.column()==3))
   {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
   }
   if(index.column()==0 && indexToPointer(parent(index))->name() != "Points")
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex PlotHandler::index (int row, int column, 
      const QModelIndex & parent) const
{
   if (!hasIndex (row, column, parent)) return QModelIndex();
   ModelNodeBase * child = (parent.isValid() ? static_cast<ModelNodeBase*>
         (parent.internalPointer())->child (parent.row()) : _model);
   assert (child);
   QModelIndex ind = createIndex (row, column, child);
   return ind;
}


QModelIndex PlotHandler::parent (const QModelIndex & index) const
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

inline ModelNodeBase * PlotHandler::indexToPointer (const QModelIndex & parent) const
{
   return (parent.isValid() && parent.internalPointer() ?
         static_cast<ModelNodeBase*> (parent.internalPointer())->child
         (parent.row()) : _model);
}

const QString PlotHandler::name(const QModelIndex & index)
{
   return indexToPointer(index)->name();
}

QVariant PlotHandler::headerData(int section, Qt::Orientation orientation, int role) const
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

bool PlotHandler::isSampleChecked(const int & idrill, const int & jsample)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->child(jsample)->properties()[ModelEnums::SampleProp::SampleCheck].toBool();
}

int PlotHandler::npoints(const int & idrill, const int & jsample)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(jsample)->child(0)->rowCount();
}

double PlotHandler::pointValue(const int & idrill, const int & jsample, 
      const int & i, const int & j)
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(jsample)->child(0));
   return points->getPoint(i)[j];
}

int PlotHandler::color(const int & idrill, const int & jsample)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->child(jsample)->properties()[ModelEnums::SampleProp::SampleColor].toInt();
}

int PlotHandler::line(const int & idrill, const int & jsample)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->child(jsample)->properties()[ModelEnums::SampleProp::SampleLine].toInt();
}

int PlotHandler::nDrills()
{
   return _model->child(ModelEnums::Root::CHECKS)->rowCount();
}

int PlotHandler::nSample(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->rowCount();
}

QString PlotHandler::drillName(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->name();
}

QString PlotHandler::sampleName(const int & idrill, const int & jsample)
{
   return QString::number(_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(jsample)->properties()[0].toDouble(), 'f', 2)+" - "+QString::number(_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(jsample)->properties()[1].toDouble(), 'f', 2);
}

void PlotHandler::addDrill(const int & row)
{
   QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex());
   beginInsertRows (drillsIndex, row, row);
   endInsertRows();
}

void PlotHandler::deleteDrill(const int & row)
{
   QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex());

   beginRemoveRows(drillsIndex, row, row);
   endRemoveRows();
}

void PlotHandler::deleteSample(const int & idrill, const int & row)
{
   
   QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex());
   QModelIndex drillIndex = index(idrill, 0, drillsIndex);
   
//   QModelIndex sampleIndex = index(row, 0, drillIndex);

   beginRemoveRows(drillIndex, row, row);
   endRemoveRows();
   
   
}

void PlotHandler::allDrillsChanged(const int & state)
{
   bool checked = !!state;
   int ndrills = _model->child(ModelEnums::Root::CHECKS)->rowCount();
   for(int i=0;i<ndrills;i++)
   {
      _model->child(ModelEnums::Root::CHECKS)->child(i)->setProperty(checked, ModelEnums::DrillChecksProp::PlotCheck);
      int limit = _model->child(ModelEnums::Root::CHECKS)->child(i)->rowCount();
      for(int j=0;j<limit;j++)
      {
         _model->child(ModelEnums::Root::CHECKS)->child(i)->child(j)->setProperty(checked, ModelEnums::SampleProp::SampleCheck);
         emit drawSpline(i, j);
//               QModelIndex iii = this->index(i, 0, index);
  //             setData(iii, newVal, Qt::CheckStateRole);
      }
      QModelIndex drillsIndex = createIndex(ModelEnums::Root::DRILLS,0,_model);
      emit dataChanged(this->index(0, 0, drillsIndex), 
            this->index(limit, 0, drillsIndex));
   }
}

void PlotHandler::selectInInterval(const double & from, const double & to)
{
   int ndrills = _model->child(ModelEnums::Root::CHECKS)->rowCount();
   for(int i=0;i<ndrills;i++)
   {
      int limit = _model->child(ModelEnums::Root::CHECKS)->child(i)->rowCount();
      for(int j=0;j<limit;j++)
      {
         if(from<=_model->child(ModelEnums::Root::DRILLS)->child(i)->child(j)->properties()[0].toDouble() && to>=_model->child(ModelEnums::Root::DRILLS)->child(i)->child(j)->properties()[1].toDouble())
         {
            _model->child(ModelEnums::Root::CHECKS)->child(i)->child(j)->setProperty(true, ModelEnums::SampleProp::SampleCheck);
            emit drawSpline(i, j);
         }
      }
      QModelIndex drillsIndex = createIndex(0,0,_model);
      emit dataChanged(this->index(0, 0, drillsIndex), 
            this->index(limit, 0, drillsIndex));
   }
}
