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
#ifndef DEPTHNAMES_H
#define DEPTHNAMES_H

#include <QAbstractItemModel>
#include <QDebug>

class DepthHeaderLineColorNames : public QAbstractItemModel
{
   Q_OBJECT
   public:
      DepthHeaderLineColorNames(){}
      int rowCount(const QModelIndex &/*parent = QModelIndex()*/) const override 
      {
         return 1;
      }
      int columnCount(const QModelIndex &/*parent = QModelIndex()*/) const override
      {
         return 4;
      }

      QVariant data(const QModelIndex &/*index*/, int /*role = Qt::DisplayRole*/) const override
      {
         return QVariant();
      }

      QModelIndex index (int /*row*/, int /*column*/,
          const QModelIndex & /*parent = QModelIndex()*/) const override
      {
         return QModelIndex();
      }
      
      QModelIndex parent (const QModelIndex & /*index*/) const override
      {
         return QModelIndex();
      }

      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
      {
         if (role != Qt::DisplayRole && role != Qt::EditRole)
            return QVariant();
         if (orientation == Qt::Horizontal) 
         {
            switch (section) {
               case 0:
                  return QVariant("From");
               case 1:
                  return QVariant("To");
               case 2:
                  return QVariant("Color");
               default:
                  return QVariant("Line");
            }
         }
         return QAbstractItemModel::headerData(section, orientation, role);

      }
};

#endif
