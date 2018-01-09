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
#include "surfHandler.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/pointSet.h"

#include <assert.h>

SurfHandler::SurfHandler(ModelNodeBase * model):_model(model)
{}

int SurfHandler::rowCount(const QModelIndex & parent) const
{
   return indexToPointer(parent)->rowCount();
}

int SurfHandler::columnCount(const QModelIndex & /*parent*/) const 
{
   return 1;
}

QVariant SurfHandler::data(const QModelIndex & index, int role) const
{
   if (role == Qt::DisplayRole)
   {
      return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), 0);
   }
   if(index.column()==0 && role == Qt::CheckStateRole)//check
   {
      return QVariant(static_cast<ModelNodeBase*>(index.internalPointer())->data(index.row(), 1).toBool());
   }
   return QVariant();
}

bool SurfHandler::setData (const QModelIndex &index, const QVariant & value, int role)
{
   if(role == Qt::CheckStateRole)
   {
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      //change check
      bool newVal = !parentNode->child(index.row())->properties()[0].toBool();
      parentNode->child(index.row())->setProperty(newVal,0);
      emit surfCheckChanged();
      return true;
   }
   else
   {//change name
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      parentNode->child(index.row())->setName(value.toString());
      emit surfCheckChanged();
      return true;
   }
   return false;
}

Qt::ItemFlags SurfHandler::flags(const QModelIndex &/* index*/) const
{
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
}

QModelIndex SurfHandler::index (int row, int column, 
      const QModelIndex & parent) const
{
   if (!hasIndex (row, column, parent)) return QModelIndex();
   ModelNodeBase * child = (parent.isValid() ? static_cast<ModelNodeBase*>
         (parent.internalPointer())->child (parent.row()) : _model);
   assert (child);
   QModelIndex ind = createIndex (row, column, child);
   return ind;
}

QModelIndex SurfHandler::parent (const QModelIndex & index) const
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

inline ModelNodeBase * SurfHandler::indexToPointer (const QModelIndex & parent) const
{
   return (parent.isValid() && parent.internalPointer() ?
         static_cast<ModelNodeBase*> (parent.internalPointer())->child
         (parent.row()) : _model);
}

void SurfHandler::deleteNode(const QModelIndex & index) 
{
   ModelNodeBase * parentNode = indexToPointer(parent(index));
   if(index.row() == parentNode->rowCount())//trying to delete non existing
      return;
   QModelIndex parentIndex = parent(index);
   beginRemoveRows(parentIndex, index.row(), index.row());
   parentNode->removeChildren(index.row(),1);
   endRemoveRows();
   emit surfCheckChanged();
  /* emit rowDeleted(index);
   if(parentNode == _model->child(ModelEnums::Root::DRILLS))      
      emit _model->deletedDrill(index.row());*/
}

QVariant SurfHandler::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
   if (orientation == Qt::Horizontal)
   {
      return QVariant("Surface name"); 
   }
    
   return QAbstractItemModel::headerData(section, orientation, role);
}

void SurfHandler::addSurface()
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = new ModelNodeBase(surfaces, 1);
   int row = surfaces->rowCount();
   surface->setName(QString::number(row+1));
   surface->setProperty(true, 0);

   PointSet * coors = new PointSet(4, surface);
   coors->setName("Surface points");
   surface->appendChild(coors);
   surfaces->appendChild(surface);
   
   QModelIndex surfacesIndex = index(ModelEnums::Root::SURFACES, 0,
         QModelIndex());
   beginInsertRows (surfacesIndex, row, row);
   endInsertRows();
   emit newSurf(index(row, 0, surfacesIndex));
}

void SurfHandler::addPointSurface(const double & x, const double & y, 
      const double & z, const int & drillID)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(surfaces->rowCount()-1);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   double pnt[4];
   pnt[0] = x;
   pnt[1] = y;
   pnt[2] = z;
   pnt[3] = drillID;
   points->appendPoint(pnt);
}

void SurfHandler::addPointOldSurface(const double & x, const double & y, 
      const double & z, const int & drillID, const int & isurf)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   double pnt[4];
   pnt[0] = x;
   pnt[1] = y;
   pnt[2] = z;
   pnt[3] = drillID;
   points->appendPoint(pnt);
}
/*
int SurfHandler::nSurfaces()
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   return surfaces->rowCount();
}

int SurfHandler::nSurfPoints(const int & isurf)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->rowCount();   
}

double SurfHandler::surfPointX(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::Xsurf).toDouble();
}

double SurfHandler::surfPointY(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::Ysurf).toDouble();
}

double SurfHandler::surfPointZ(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::Zsurf).toDouble();
}

int SurfHandler::surfPointDrill(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::DrillSurf).toDouble();
}*/

bool SurfHandler::isCheckedSurf(const int & isurf)
{
   return _model->child(ModelEnums::Root::SURFACES)->child(isurf)->properties()[0].toBool();
}
/*
QString SurfHandler::surfName(const int & isurf)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   return surface->name();
}*/

void SurfHandler::changePointHeight(const int & isurf, const int & ipoint, 
      const double & value)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   points->setData(ipoint, ModelEnums::SurfacePoint::Zsurf, QVariant(value));
}

void SurfHandler::selectionChangedSlot(const QModelIndex & index)
{
   emit selectedInTable(index.row());
}
