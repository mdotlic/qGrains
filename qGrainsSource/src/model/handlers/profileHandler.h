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
#ifndef PROFILEHANDLER_H
#define PROFILEHANDLER_H

#include <QAbstractItemModel>
class ModelNodeBase;

class ProfileHandler : public QAbstractItemModel
{
   Q_OBJECT
   public:
      ProfileHandler(ModelNodeBase *);

      int rowCount(const QModelIndex & ) const override;
      int columnCount(const QModelIndex &) const override;

      QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
      bool setData (const QModelIndex &index, const QVariant &value, int role);
      Qt::ItemFlags flags(const QModelIndex & index) const override ;
      QModelIndex index (int, int, const QModelIndex & parent = QModelIndex()) const override;
      
      QModelIndex parent (const QModelIndex & ) const override;
      
      inline ModelNodeBase * indexToPointer (const QModelIndex & parent) const;
      void deleteNode(const QModelIndex & index);
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

   signals:
      void selectedInTable(const int &);
      void hideDrillsTable();
      void changeName(const int &);
   public slots:
      void profileSelChangedSlot(const QModelIndex &);
      void deleteDrill(const int &);
   private:
      ModelNodeBase * _model;
};

#endif
