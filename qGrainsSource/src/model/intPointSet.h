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
#ifndef INTPOINTSET_H
#define INTPOINTSET_H

#include <vector>
#include "modelNodeBase.h"

class IntPointSet : public std::vector<int>, public ModelNodeBase
{
   public:
      IntPointSet(int dim, ModelNodeBase * parent);
      int insertNewPoint (const int * p);
      virtual void insertPoint (int id, const int * p);
      
      virtual void appendPoint (const int * p);
      
      virtual void insertPoints (const int first, const int len, const int * pp);

      const int * getPoint(int id) const { return &((*this)[_dim*id]);}
      
      int * getPoint (int id) { return &((*this)[_dim*id]); }
      
      int numberOfPoints() const { return size()/_dim; }

      bool isPresent(int value);
     
      virtual void findAndRemovePoint (int value);
      
      virtual void removePoint (int id);
      
      virtual void removePoints (const int first, const int len);

      int dimension() const { return _dim; }
      QVariant data (int row, int col) const override;
      int rowCount() const override { return numberOfPoints(); }
//      int columnCount() const override { return _dim; }
      Qt::ItemFlags flags (int row, int col) const override;
      bool setData (int row, int col, QVariant value) override;
      
   protected:
      int _dim;
};


#endif
