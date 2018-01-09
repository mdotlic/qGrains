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

#include "pictureWidget.h"
#include "picture.h"
#include "model/handlers/inputHandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

PictureWidget::PictureWidget(QGrains * qGrains, InputHandler * inputHandler)
{
   QVBoxLayout * layout = new QVBoxLayout;
   QHBoxLayout * horizontal = new QHBoxLayout;
   QPushButton * choose = new QPushButton("Choose picture");
   horizontal->addWidget(choose);
   QPushButton * rect = new QPushButton("Draw rectangle");
   horizontal->addWidget(rect);
   rect->setCheckable(true);
   QPushButton * predefined = new QPushButton("Use predefined values");
   horizontal->addWidget(predefined);
   predefined->setCheckable(true);
   layout->addLayout(horizontal);
   Picture * picture = new Picture(qGrains, inputHandler, rect, predefined);
   layout->addWidget(picture);
   setLayout(layout);
   connect(choose, SIGNAL(clicked()), picture, SLOT(imageProp()));
   connect(rect, SIGNAL(clicked()), picture, SLOT(drawRectangle()));
   connect(predefined, SIGNAL(clicked()), picture, SLOT(predefSlot()));
}
