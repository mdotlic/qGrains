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
#include "isoList.h"

#include <QKeyEvent>
#include <QDebug>

IsoList::IsoList()
{
   connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(listItemClick(QListWidgetItem *)));

}

void IsoList::keyPressEvent(QKeyEvent * event)
{
   if(event->key() == Qt::Key_Delete)
   {
      qDebug()<<" kliknuo na delete \n";
      delete this->takeItem(_clicked);
      emit elementDeleted(_clicked);
      emit listChanged();
   }
   //qDebug()<<" key event \n";
}

void IsoList::listItemClick(QListWidgetItem * item)
{
   _clicked=this->row(item);
  // this->openPersistentEditor(item);
  /* for(int i=0;i<this->count();i++)
   {
      if(i!=_clicked)
         this->closePersistentEditor(this->item(i));
   }*/
   //qDebug()<<" kliknuo "<<item->text()<<"\n";
}

