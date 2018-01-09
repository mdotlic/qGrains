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
#include "qGrains.h"
#include "depthDTab.h"
#include "chooserWidget.h"
#include "dChooser/dChooser.h"
#include "depthDPicture.h"
#include "drillChooser.h"

#include <QVBoxLayout>


DepthDTab::DepthDTab(Handler * handler, QGrains * qGrains)
{
/*   DepthDTab * depthDTab = new InputTab(qGrains);
   depthDTab->setFocusPolicy(Qt::ClickFocus);
   QString depthDString = QString("Depth vs d");
   
   PercTab * percTab = new PercTab(qGrains);
   percTab->setFocusPolicy(Qt::ClickFocus);
   QString percString = QString("Depth vs %");

   addTab(depthDTab, depthDString);
   addTab(percTab, percString);
*/
   _chooserWidget = new ChooserWidget(qGrains->model());
   QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spLeft.setHorizontalStretch(1);
   _chooserWidget->setSizePolicy(spLeft);
   
   _depthDPicture = new DepthDPicture(handler, _chooserWidget->drillChooser()->viewHandler(), _chooserWidget->dChooser()->dHandler(), qGrains);

   QFrame * pictureFrame = new QFrame;
   QVBoxLayout * pictureLayout = new QVBoxLayout;

   pictureLayout->addWidget(_depthDPicture,Qt::AlignVCenter);
   pictureFrame->setLayout(pictureLayout);
   pictureFrame->setMidLineWidth(2);
   pictureFrame->setFrameShape(QFrame::StyledPanel);
   pictureFrame->setFrameShadow(QFrame::Sunken);

   QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spRight.setHorizontalStretch(10);
   pictureFrame->setSizePolicy(spRight);

   addWidget(_chooserWidget);
   addWidget(pictureFrame);

}
