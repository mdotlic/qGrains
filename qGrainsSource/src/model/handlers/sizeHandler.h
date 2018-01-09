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
#ifndef SIZEHANDLER_H
#define SIZEHANDLER_H

#include <QAbstractItemModel>
#include <QDebug>
#include <vector>
class ModelNodeBase;

class SizeHandler : public QAbstractItemModel
{
   Q_OBJECT
   public:
      SizeHandler(ModelNodeBase *);

      int rowCount(const QModelIndex & ) const override;
      int columnCount(const QModelIndex &) const override;

      QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
      bool setData (const QModelIndex &index, const QVariant &value, int role);
      Qt::ItemFlags flags(const QModelIndex & index) const override ;
      QModelIndex index (int, int, const QModelIndex & parent = QModelIndex()) const override;
      
      QModelIndex parent (const QModelIndex & ) const override;
      
      inline ModelNodeBase * indexToPointer (const QModelIndex & parent) const;
      QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
      
      int nsizes();
      double from(const int &);
      double to(const int &);
      bool isChecked(const int &);
      int sizeStyle(const int & id);
      int sizeColor(const int & id);
      int sizeTick(const int & id);

      void commandSizeAppend();
      void commandSizeAppendLoad(const std::vector<double> & v);
      ModelNodeBase * model(){return _model;}

   signals:
      void sizeSelectionChange(const int &);
      void styleChange(const int &);
      void loadFinished();
      void sizeDataAppend(const int &, const int &);
      void sizeAppended(const int &);
   private:
      ModelNodeBase * _model;
      std::vector<double> _sizeValues = std::vector<double>(2, std::numeric_limits<size_t>::max());
      std::vector<QVariant> _values = std::vector<QVariant>(2, QVariant(0));
};

#endif
