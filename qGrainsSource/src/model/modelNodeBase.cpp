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
#include "modelNodeBase.h"
#include <assert.h>
#include <QDebug>


ModelNodeBase::ModelNodeBase (ModelNodeBase * parent, 
      const int numberOfProperties) : QObject(), _children(), _parent (parent)
{
   _properties = new std::vector<QVariant> (numberOfProperties);
   _name = new QString;
   connect(this, SIGNAL(modelChanged()), _parent, SLOT(modelChSlot()));
}

ModelNodeBase::~ModelNodeBase()
{
   for (ModelNodeBase* child : _children) {
      delete child;
   }
   delete _properties;
   delete _name;
}

QVariant ModelNodeBase::data (int row, int col) const
{
   assert (row < (int) _children.size());
   assert (col <= (int) _children[row]->properties().size());
   return col == 0 ? QVariant (_children[row]->name()) :
      _children[row]->properties()[col-1];
}

bool ModelNodeBase::setData (int row, int col, QVariant v)
{
   if (row < (int) _children.size() && 
         col <= (int) _children[row]->properties().size()) 
   {
      emit modelChanged();
      if (col) _children[row]->setProperty (v, col-1);
      else _children[row]->setName (v.toString());
      return true;
   } else return false;
}

int ModelNodeBase::row() const
{
   if (_parent)
   {
      return (std::find (_parent->_children.begin(), _parent->_children.end(),
               this) - _parent->_children.begin());
   }
   return 0;
}

Qt::ItemFlags ModelNodeBase::flags (int row, int col) const
{
   if (row < 0 || row >= (int) _children.size()+1 || col < 0 || col > 5 )
      return Qt::ItemIsEnabled;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void ModelNodeBase::setProperty (const QVariant & p, const int i)
{
   assert (i >= 0 && i < (int) _properties->size());
   (*_properties)[i] = p;
   emit modelChanged();
}

void ModelNodeBase::setName (const QString & s) 
{ 
   *_name = s; 
   emit modelChanged();
}

const QString & ModelNodeBase::name() const { return *_name; }

void ModelNodeBase::appendChild (ModelNodeBase * child)
{
   _children.push_back (child);
   emit modelChanged();

   //const int sz = _children.size();
   //emit childrenInserted (sz, sz);
}

void ModelNodeBase::insertChild (ModelNodeBase * child, size_t i)
{
   _children.insert(_children.begin()+i, child);
   emit modelChanged();
}

void ModelNodeBase::removeChildren(const int first, const int len)
{
    assert (first+len <= (int) _children.size() && first>=0 && len>0);
   _children.erase (_children.begin()+first, _children.begin()+first+len);
   //emit childrenRemoved (first, first+len-1);  
   emit modelChanged();
}

void ModelNodeBase::modelChSlot()
{
   emit modelChanged();
}
