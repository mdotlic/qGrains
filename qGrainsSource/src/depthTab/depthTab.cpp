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
#include "depthTab.h"
#include "qGrains.h"
#include "depthTab/depthDTab/depthDTab.h"
#include "depthTab/percTab/percTab.h"
/*#include "depthTab/chooserWidget.h"
#include "depthTab/dChooser.h"
#include "depthTab/depthDPicture/depthDPicture.h"
#include "depthTab/drillChooser.h"*/

#include <QVBoxLayout>


DepthTab::DepthTab(QGrains * qGrains, Handler * handler) : QTabWidget()
{
   setTabPosition(QTabWidget::West);
   _depthDTab = new DepthDTab(handler, qGrains);
   _depthDTab->setFocusPolicy(Qt::ClickFocus);
   QString depthDString = QString("Depth vs d");
   
   _percTab = new PercTab(handler, qGrains);
   _percTab->setFocusPolicy(Qt::ClickFocus);
   QString percString = QString("Depth vs %");
   
   addTab(_depthDTab, depthDString);
   addTab(_percTab, percString);

}
