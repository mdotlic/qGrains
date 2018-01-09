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
#include "intPointSet.h"
#include <assert.h>
#include <QVariant>
#include <QDebug>

IntPointSet::IntPointSet(int dim, ModelNodeBase * parent): ModelNodeBase(parent), _dim(dim)
{
   //reserve(64*dim);//64 is size
}

int IntPointSet::insertNewPoint (const int * p)
{
//   assert (id<=numberOfPoints() && id>=0);
   //emit pointsAboutToBeInserted (id, id);
   for(int i=0;i<numberOfPoints();i++)
   {
      if(p[1]<(*this)[i*_dim+1])
      {
         insert (begin()+i*_dim, p, p+_dim);
         return i; 
      }
   }
   insertPoint (numberOfPoints(), p);
   return numberOfPoints();
   //emit pointsInserted (id, id);
}

void IntPointSet::insertPoint (int id, const int * p)
{
   assert (id<=numberOfPoints() && id>=0);
   //emit pointsAboutToBeInserted (id, id);
   insert (begin()+id*_dim, p, p+_dim);
   //emit pointsInserted (id, id);
}

void IntPointSet::appendPoint (const int * p)
{
   //const int npnt = numberOfPoints();
   //emit pointsAboutToBeInserted (npnt, npnt);
   insertPoint (numberOfPoints(), p);
   //emit pointsInserted (npnt, npnt);
}

bool IntPointSet::isPresent(int value)
{
   if(numberOfPoints()==0)
      return false;
   std::vector<int>::iterator it = find (begin(), end(), value);
   return it==end() ? false : true;
}

void IntPointSet::findAndRemovePoint (int value)
{
   std::vector<int>::iterator it = find (begin(), end(), value);
   assert(it!=end());
   erase(it);
}

void IntPointSet::removePoint (int id)
{
   //emit pointsAboutToBeRemoved (id, id);
   erase (begin()+id*_dim, begin()+(id+1)*_dim);
   //emit pointsRemoved (id, id);
}

void IntPointSet::insertPoints (const int first, const int len, const int * pp)
{
   //emit pointsAboutToBeInserted (first, first+len-1);
   int * const startq = std::vector<int>::data() + size()-_dim;
   const int * const endq = std::vector<int>::data() + (first-1)*_dim;
   const int blocklen = len*_dim;
   resize (size()+blocklen);
   int * p = std::vector<int>::data() + size();
   for (int * q=startq; q!=endq; q-=_dim) { // Make a gap
      p -= _dim;
      memcpy (p, q, _dim*sizeof (int));
   }
   memcpy (std::vector<int>::data() + first*_dim, pp,
         blocklen*sizeof (int)); //Fill the gap
   //emit pointsInserted (first, first+len-1);
}

void IntPointSet::removePoints (const int first, const int len)
{
   //emit pointsAboutToBeRemoved (first, first+len-1);
   int * p = std::vector<int>::data() + first*_dim;
   const int * endq = std::vector<int>::data() + size();
   for (int * q=p+len*_dim; q!=endq; q+=_dim) { // Fill the gap
      memcpy (p, q, _dim*sizeof (int));
      p += _dim;
   }
   resize (p-std::vector<int>::data());
   //emit pointsRemoved (first, first+len-1);
}

QVariant IntPointSet::data (int row, int col) const
{
   return QVariant ((*this)[row*_dim+col]);
}

Qt::ItemFlags IntPointSet::flags (int row, int col) const
{
   if (row<0 || row>=numberOfPoints()+1 || col<0 || col>=_dim)
      return Qt::ItemIsEnabled;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool IntPointSet::setData (int row, int col, QVariant value)
{
   assert (row*_dim+col < (int) size());
   (*this)[row*_dim+col] = value.toInt();
   return true;
}
