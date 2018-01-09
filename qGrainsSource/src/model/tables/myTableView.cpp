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
#include "myTableView.h"
#include <QHeaderView>
#include <QDebug>
#include <QResizeEvent>

MyTableView::MyTableView(QWidget * w):QTableView(w)
{}

void MyTableView::resizeEvent(QResizeEvent * ev)
{
   horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   int widthTable = width();
   int width = (widthTable-verticalHeader()->width())/horizontalHeader()->count();
   for(int icol=0; icol<horizontalHeader()->count()-1; icol++)
   {
      horizontalHeader()->setSectionResizeMode(icol, QHeaderView::Interactive);
      horizontalHeader()->resizeSection(icol, width);
   }

   QTableView::resizeEvent(ev);
}
