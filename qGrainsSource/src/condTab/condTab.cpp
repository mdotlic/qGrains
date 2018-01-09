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
#include "condTab.h"
#include "qGrains.h"
#include "condTab/condLeftWidget.h"
#include "condTab/condTable.h"
#include "model/handlers/condHandler.h"

CondTab::CondTab(QGrains * qGrains, Handler * handler)
{
   CondHandler * condHandler = new CondHandler(qGrains->model());
   CondTable * condTable = new CondTable(condHandler, handler);
   QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spRight.setHorizontalStretch(3);
   condTable->setSizePolicy(spRight);

   CondLeftWidget * condLeftWidget = new CondLeftWidget(qGrains->model(), condHandler, condTable);
   QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spLeft.setHorizontalStretch(1);
   condLeftWidget->setSizePolicy(spLeft);

   addWidget(condLeftWidget);
   addWidget(condTable);
}
