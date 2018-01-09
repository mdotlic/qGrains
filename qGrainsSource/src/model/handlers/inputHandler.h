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
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <QAbstractItemModel>
//#include "handler.h"
class ModelNodeBase;

class InputHandler : public QAbstractItemModel
{
    Q_OBJECT
  public:

    InputHandler(ModelNodeBase * model);
    void initializeModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData (const QModelIndex &index, const QVariant &value,
          int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override ;

    QModelIndex index (int row, int column,
          const QModelIndex & parent = QModelIndex()) const override;
    QModelIndex parent (const QModelIndex & index) const override;
    ModelNodeBase * indexToPointer (const QModelIndex & parent) const;

    void deleteNode(const QModelIndex & index);
    
    void commandDrillAppend();
    
    void commandDepthAppend(const QModelIndex & index);
    
    void commandCoorAppend(const QModelIndex & index, const double & v);
    void commandCoorAppendPicture(const QModelIndex & index, const double & x, 
          const double & y);
  
    double pointValue(const QModelIndex & index, const int row, 
          const int col) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int unknownValue();
    int unknownDepthValue();
    
    const QString name(const QModelIndex & index);
   
  signals:
     void addedDrill(const QModelIndex & index);
     void addedDepth(const QModelIndex & index);
     void addedCoor();
     void coorNotAdded();
     void coorChanged(const QModelIndex & index);
     void rowDeleted(const QModelIndex &);
     void pointChanged();
     void drillDataAppend(const QModelIndex &);
     void depthDataAppend(const QModelIndex &);
     void deleteSpline(const int &, const int &);
  public slots:
     void loadDrillSlot(const std::vector<double> & v, const QString & name);
     void loadDepthSlot(const double & v0, const double & v1, 
           const QModelIndex &);
     void loadCoorSlot(const double *, const QModelIndex &);
  private:
    void appendToNodeBase(const QModelIndex & parent, ModelNodeBase* p);
    ModelNodeBase * _model;
    double _otherValue=std::numeric_limits<size_t>::max();
    bool _hasFirst = false;
    std::vector<double> _values = std::vector<double>(4, std::numeric_limits<size_t>::max());
    std::vector<double> _depthValues = std::vector<double>(2, std::numeric_limits<size_t>::max());
    QString _drillName;
};

#endif 
