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
#include "inputTab.h"
#include "qGrains.h"
#include "inputTab/pictureWidget/pictureWidget.h"
#include "inputTab/leftWidget.h"
#include <QDebug>

InputTab::InputTab(InputHandler * inputHandler, Handler * handler, 
      QGrains * qGrains):QSplitter(Qt::Horizontal)
{
   _leftWidget = new LeftWidget(inputHandler, handler, qGrains);
   QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spLeft.setHorizontalStretch(2);
   _leftWidget->setSizePolicy(spLeft);
   PictureWidget * pictureWidget = new PictureWidget(qGrains, 
         _leftWidget->inputHandler());
   QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spRight.setHorizontalStretch(3);
   pictureWidget->setSizePolicy(spRight);

   addWidget(_leftWidget);
   addWidget(pictureWidget);
}
