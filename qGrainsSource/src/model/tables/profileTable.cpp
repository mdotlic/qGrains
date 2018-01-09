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
#include "profileTable.h"
#include "model/handlers/profileHandler.h"
#include <QKeyEvent>
#include <QDebug>

ProfileTable::ProfileTable(ProfileHandler * profileHandler, QWidget * w):QTableView(w), _profileHandler(profileHandler)
{}

void ProfileTable::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Delete) 
   {
      _profileHandler->deleteNode(currentIndex());
   }
   /*if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
   {
      qDebug()<<" sadasnji je "<<currentIndex()<<"\n";
   }*/
   QTableView::keyPressEvent(event);
}

void ProfileTable::selectionChanged(const QItemSelection &selected, 
      const QItemSelection &deselected)
{
   if(selected.indexes().size()==1)
   {
      emit selChange(selected.indexes().at(0));
   }
   QTableView::selectionChanged(selected, deselected);
}
