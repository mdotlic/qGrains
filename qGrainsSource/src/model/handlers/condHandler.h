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
#ifndef CONDHANDLER_H
#define CONDHANDLER_H

#include <QAbstractItemModel>
class ModelNodeBase;

class CondHandler : public QAbstractItemModel
{
   Q_OBJECT
   public:
      CondHandler(ModelNodeBase * model);
      int rowCount(const QModelIndex &parent = QModelIndex()) const override;
      int columnCount(const QModelIndex &parent = QModelIndex()) const override;
      QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
      bool setData (const QModelIndex &index, const QVariant &value,
            int role = Qt::EditRole) override;
      Qt::ItemFlags flags(const QModelIndex & index) const override ;
      QModelIndex index (int row, int column,
            const QModelIndex & parent = QModelIndex()) const override;
      QModelIndex parent (const QModelIndex & index) const override;
      inline ModelNodeBase * indexToPointer (const QModelIndex & parent) const;
      const QString name(const QModelIndex & index);
      QVariant headerData(int section, Qt::Orientation orientation, 
            int role = Qt::DisplayRole) const override;
      bool isSampleChecked(const int &, const int &);
      int npoints(const int &, const int &);
      double pointValue(const int &, const int &, const int &, const int &);
      ModelNodeBase * model(){return _model;}
      /*QString drillName(const int &);
      QString sampleName(const int &, const int &);
      double tolerance();
      double visc();
      double terc();
      double zunk();*/
      void setCondCheck(const int &, const bool &);
      bool condCheck(const int &);
   signals:
      void changeRow(const int & idrill, const int & jsample);
     /* void drawSpline(const int idrill, const int jsample);
      void changeStyle();
      void splineSelected(const int, const int);
      void deselectedSpline();
      void sampleSelected(const int &, const int &);*/
   public slots:
      void addDrill(const int &);
      void deleteDrill(const int &);
      void allDrillsChanged(const int &);
      void selectInInterval(const double & from, const double & to);
   private:
      ModelNodeBase * _model;

};
#endif
