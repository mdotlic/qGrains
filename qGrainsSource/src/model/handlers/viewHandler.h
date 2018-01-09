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
#ifndef VIEWHANDLER_H
#define VIEWHANDLER_H

#include <QAbstractItemModel>
class ModelNodeBase;

class ViewHandler : public QAbstractItemModel
{
   Q_OBJECT
   public:
      ViewHandler(ModelNodeBase *, const int &);

      int rowCount(const QModelIndex & ) const override;
      int columnCount(const QModelIndex &) const override;

      QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
      bool setData (const QModelIndex &index, const QVariant &value, int role);
      Qt::ItemFlags flags(const QModelIndex & index) const override ;
      QModelIndex index (int, int, const QModelIndex & parent = QModelIndex()) const override;
      
      QModelIndex parent (const QModelIndex & ) const override;
      
      inline ModelNodeBase * indexToPointer (const QModelIndex & parent) const;
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

      bool isCheckedDrill(const int &);
      bool isCheckedDrillPerc(const int &);
      bool isCheckedDrillProfile(const int &);
      void changeCheckProfile(const int &);
      int profileColor(const int &);      
      
      void setTypeCalc(const int &);
      void setProfileFrom(const double &);
      void setProfileTo(const double &);

      int drillTick(const int & idrill);
      int drillPercTick(const int & idrill);
      QString drillName(const int & idrill);
     
      int nDrills();
      int nSamples(const int & idrill);
      double sampleVal(const int & idrill, const int & isample, const int & iv);
      

   signals:
      void drillSelectionChange(const int &);
      void drillPercSelectionChange(const int &); 
      void drillProfSelectionChange(const int &);
      void drillTickChange(const int &);
      void drillPercTickChange(const int &);
      void drillProfColorChange(const int &);
      void typeCalcChanged();
   public slots:
      void addDrill(const int &);
      void deleteDrill(const int &);
      void profileChanged(const int &);
      

   private:
      ModelNodeBase * _model;
      int _variant = 0;//variant = 0 for depthd, 1 for perc, 2 for profile
};

#endif
