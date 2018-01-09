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
#ifndef SURFHANDLER_H
#define SURFHANDLER_H

#include <QAbstractItemModel>
#include <QDebug>
#include <vector>
class ModelNodeBase;

class SurfHandler : public QAbstractItemModel
{
   Q_OBJECT
   public:
      SurfHandler(ModelNodeBase *);

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
      
      void addSurface();
      void addPointSurface(const double &, const double &, const double &, 
            const int &);
      void addPointOldSurface(const double &, const double &, const double &, 
            const int &, const int &);

     /* int nSurfaces();
      int nSurfPoints(const int &);
      double surfPointX(const int & isurf, const int & ipoint);
      double surfPointY(const int & isurf, const int & ipoint);
      double surfPointZ(const int & isurf, const int & ipoint);
      int surfPointDrill(const int & isurf, const int & ipoint);
      */
      bool isCheckedSurf(const int & isurf);
      //QString surfName(const int &);
      void changePointHeight(const int & isurf, const int & ipoint, 
            const double & value);

   signals:
      void surfCheckChanged();
      void selectedInTable(const int &);
      void newSurf(const QModelIndex &);
      void lineSelected(const int &);

   public slots:
      void selectionChangedSlot(const QModelIndex &);

   private:
      ModelNodeBase * _model;
};
#endif
