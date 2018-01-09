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
#include "profileHandler.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/intPointSet.h"
#include "model/pointSet.h"

#include <assert.h>
#include <QDebug>

ProfileHandler::ProfileHandler(ModelNodeBase * model):_model(model)
{}

int ProfileHandler::rowCount(const QModelIndex & parent) const
{
   return indexToPointer(parent)->rowCount()+1;
}

int ProfileHandler::columnCount(const QModelIndex & /*parent*/) const 
{
   return 1;
}

QVariant ProfileHandler::data(const QModelIndex & index, int role) const
{
   if (role == Qt::DisplayRole)
   {
      if(index.row()<rowCount(parent(index))-1)
      {
         return static_cast<ModelNodeBase*>(index.internalPointer())->data
            (index.row(), 0);
      }
   }
   return QVariant();
}

bool ProfileHandler::setData (const QModelIndex &index, const QVariant & value, int /*role*/)
{
   if (!index.isValid())
   {
      return false;
   }else{
      ModelNodeBase * parentNode = indexToPointer(parent(index));
      int row = parentNode->rowCount();
      if(index.row() != row)
      {
         ModelNodeBase * node = indexToPointer(index);
         node->setName(value.toString());
         emit changeName(index.row());
      }
      else
      {
         ModelNodeBase * profiles = _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles);

         ModelNodeBase * profile = new ModelNodeBase (profiles);
         profile->setName(value.toString());
         IntPointSet * drillsID = new IntPointSet(1, profile);
         drillsID->setName("array Drill Ids");
         profile->appendChild(drillsID);

         beginInsertRows(parent(index), row, row);
         profiles->appendChild(profile);
         endInsertRows();
      }
      return true;
   }
}

Qt::ItemFlags ProfileHandler::flags(const QModelIndex & /*index*/) const
{
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QModelIndex ProfileHandler::index (int row, int column, 
      const QModelIndex & parent) const
{
   if (!hasIndex (row, column, parent)) return QModelIndex();
   ModelNodeBase * child = (parent.isValid() ? static_cast<ModelNodeBase*>
         (parent.internalPointer())->child (parent.row()) : _model);
   assert (child);
   QModelIndex ind = createIndex (row, column, child);
   return ind;
}


QModelIndex ProfileHandler::parent (const QModelIndex & index) const
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

inline ModelNodeBase * ProfileHandler::indexToPointer (const QModelIndex & parent) const
{
   return (parent.isValid() && parent.internalPointer() ?
         static_cast<ModelNodeBase*> (parent.internalPointer())->child
         (parent.row()) : _model);
}

void ProfileHandler::deleteNode(const QModelIndex & index) 
{
   ModelNodeBase * parentNode = indexToPointer(parent(index));
   if(index.row() == parentNode->rowCount())//trying to delete non existing
      return;
   QModelIndex parentIndex = parent(index);
   beginRemoveRows(parentIndex, index.row(), index.row());
   parentNode->removeChildren(index.row(),1);
   endRemoveRows();
//   emit surfCheckChanged();
  /* emit rowDeleted(index);
   if(parentNode == _model->child(ModelEnums::Root::DRILLS))      
      emit _model->deletedDrill(index.row());*/
}

QVariant ProfileHandler::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
   if (orientation == Qt::Horizontal)
   {
      //return QVariant("Drills");
      if (section==0)
      {
            return QVariant("Profiles"); 
      }
   }
    
   return QAbstractItemModel::headerData(section, orientation, role);
}

void ProfileHandler::profileSelChangedSlot(const QModelIndex & index)
{
   int nprofiles = _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles)->rowCount();
   if(index.row()<nprofiles)
      emit selectedInTable(index.row());
   else
      emit hideDrillsTable();
}

void ProfileHandler::deleteDrill(const int & row)
{
   QModelIndex drillsIndex = index(ModelEnums::Root::DRILLS, 0, QModelIndex());

   beginRemoveRows(drillsIndex, row, row);
   endRemoveRows();

   ModelNodeBase * profilesNode = _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles);

   int nprofiles = profilesNode->rowCount();


   for(int i=0;i<nprofiles;i++)
   {
      ModelNodeBase * profil = profilesNode->child(i);
      IntPointSet * points = static_cast<IntPointSet*> (profil->child(0));
      

      for(int ipoint=0; ipoint < points->numberOfPoints(); ipoint++)
      {
         int previous = points->data(ipoint, 0).toInt();
         if(previous==row)
            points->removePoint(ipoint);
         if(previous>row)
            points->setData(ipoint, 0, QVariant(previous-1));
      }
   }
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   int nsurfaces = surfaces->rowCount();
   for(int isurf=0; isurf<nsurfaces; isurf++)
   {
      ModelNodeBase * surface = surfaces->child(isurf);
      PointSet * points = static_cast<PointSet*> (surface->child(0));
      for(int ipoint=0; ipoint < points->numberOfPoints(); ipoint++)
      {
         int previous = points->data(ipoint, 3).toInt();
         if(previous==row)
            points->removePoint(ipoint);
         if(previous>row)
            points->setData(ipoint, 3, QVariant(previous-1));
      }
   }
}

