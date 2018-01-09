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
#include "tables.h"
#include "model/handlers/inputHandler.h"
#include <QKeyEvent>
#include <QHeaderView>
#include <QDebug>

Tables::Tables(InputHandler * inputHandler, QWidget * w):QTableView(w), _inputHandler(inputHandler)
{}

void Tables::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Delete) 
   {
      _inputHandler->deleteNode(currentIndex());
   }
   else if (event->modifiers()==Qt::ControlModifier && event->key() == Qt::Key_V) 
   {
      emit pasteData(currentIndex());
   }
   QTableView::keyPressEvent(event);
}

void Tables::selectionChanged(const QItemSelection &selected, 
      const QItemSelection &deselected)
{
   if(selected.indexes().size()==1)
   {
      emit selChange(selected.indexes().at(0));
   }
   QTableView::selectionChanged(selected, deselected);
}

void Tables::resizeEvent(QResizeEvent * ev)
{
   horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   for(int icol=0; icol<horizontalHeader()->count()-1; icol++)
   {
      int width =  horizontalHeader()->sectionSize(icol);
      horizontalHeader()->setSectionResizeMode(icol, QHeaderView::Interactive);
      horizontalHeader()->resizeSection(icol, width);
   }

   QTableView::resizeEvent(ev);
}
