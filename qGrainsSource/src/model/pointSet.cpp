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
#include "pointSet.h"
#include <assert.h>
#include <QVariant>
#include <QDebug>

PointSet::PointSet(int dim, ModelNodeBase * parent): ModelNodeBase(parent), _dim(dim)
{
   //reserve(64*dim);//64 is size
}

int PointSet::insertNewPoint (const double * p)
{
//   assert (id<=numberOfPoints() && id>=0);
   //emit pointsAboutToBeInserted (id, id);
   for(int i=0;i<numberOfPoints();i++)
   {
      if(p[1]<(*this)[i*_dim+1])
      {
         if(p[0]>=(*this)[i*_dim] || (i>0 && p[0]<=(*this)[(i-1)*_dim]))
         {
            return -1;
         }
         insert (begin()+i*_dim, p, p+_dim);
         return i; 
      }
   }
   if(numberOfPoints()>0 && p[0]<(*this)[(numberOfPoints()-1)*_dim])
   {
      return -1;
   }
   insertPoint (numberOfPoints(), p);
   return numberOfPoints();
   //emit pointsInserted (id, id);
}

void PointSet::insertPoint (int id, const double * p)
{
   assert (id<=numberOfPoints() && id>=0);
   //emit pointsAboutToBeInserted (id, id);
   insert (begin()+id*_dim, p, p+_dim);
   //emit pointsInserted (id, id);
}

void PointSet::appendPoint (const double * p)
{
   //const int npnt = numberOfPoints();
   //emit pointsAboutToBeInserted (npnt, npnt);
   insertPoint (numberOfPoints(), p);
   //emit pointsInserted (npnt, npnt);
}

void PointSet::removePoint (int id)
{
   //emit pointsAboutToBeRemoved (id, id);
   erase (begin()+id*_dim, begin()+(id+1)*_dim);
   //emit pointsRemoved (id, id);
}

void PointSet::insertPoints (const int first, const int len, const double * pp)
{
   //emit pointsAboutToBeInserted (first, first+len-1);
   double * const startq = std::vector<double>::data() + size()-_dim;
   const double * const endq = std::vector<double>::data() + (first-1)*_dim;
   const int blocklen = len*_dim;
   resize (size()+blocklen);
   double * p = std::vector<double>::data() + size();
   for (double * q=startq; q!=endq; q-=_dim) { // Make a gap
      p -= _dim;
      memcpy (p, q, _dim*sizeof (double));
   }
   memcpy (std::vector<double>::data() + first*_dim, pp,
         blocklen*sizeof (double)); //Fill the gap
   //emit pointsInserted (first, first+len-1);
}

void PointSet::removePoints (const int first, const int len)
{
   //emit pointsAboutToBeRemoved (first, first+len-1);
   double * p = std::vector<double>::data() + first*_dim;
   const double * endq = std::vector<double>::data() + size();
   for (double * q=p+len*_dim; q!=endq; q+=_dim) { // Fill the gap
      memcpy (p, q, _dim*sizeof (double));
      p += _dim;
   }
   resize (p-std::vector<double>::data());
   //emit pointsRemoved (first, first+len-1);
}

QVariant PointSet::data (int row, int col) const
{
   return QVariant ((*this)[row*_dim+col]);
}

Qt::ItemFlags PointSet::flags (int row, int col) const
{
   if (row<0 || row>=numberOfPoints()+1 || col<0 || col>=_dim)
      return Qt::ItemIsEnabled;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool PointSet::setData (int row, int col, QVariant value)
{
   assert (row*_dim+col < (int) size());
   (*this)[row*_dim+col] = value.toDouble();
   return true;
}
