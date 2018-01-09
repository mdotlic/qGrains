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

#include "centralWindow.h"
#include "inputTab/inputTab.h"
#include "plotTab/plotTab.h"
#include "depthTab/depthTab.h"
#include "condTab/condTab.h"
#include "profileTab/profileTab.h"
#include "planTab/planTab.h"
#include "model/handlers/handler.h"
#include "model/handlers/inputHandler.h"
#include "qGrains.h"
//#include "depthTab/chooserWidget.h"

#include <QDebug>

CentralWindow::CentralWindow(QGrains * qGrains) : QTabWidget()
{
   _handler = new Handler (qGrains->model());
   
   InputHandler * inputHandler = new InputHandler(qGrains->model());

   _inputTab = new InputTab(inputHandler, _handler, qGrains);
   _inputTab->setFocusPolicy(Qt::ClickFocus);
   QString inputString = QString("Input");
   
   PlotTab * plotTab = new PlotTab(inputHandler, qGrains);
   plotTab->setFocusPolicy(Qt::ClickFocus);
   QString plotString = QString("Plot");
   
   _depthTab = new DepthTab(qGrains, _handler);

   _depthTab->setFocusPolicy(Qt::ClickFocus); 
   QString drillsString = QString("Depth plot");

   CondTab * condTab = new CondTab(qGrains, _handler);
   condTab->setFocusPolicy(Qt::ClickFocus); 
   QString condString = QString("Conductivity");

   ProfileTab * profileTab = new ProfileTab(qGrains, _handler);
   profileTab->setFocusPolicy(Qt::ClickFocus); 
   QString profileString = QString("Profile");

   PlanTab * planTab = new PlanTab(qGrains, _handler);
   planTab->setFocusPolicy(Qt::ClickFocus); 
   QString planString = QString("Plan");

   addTab(_inputTab, inputString);
   addTab(plotTab, plotString);
   addTab(_depthTab, drillsString);
   addTab(condTab, condString);
   addTab(profileTab, profileString);
   addTab(planTab, planString);

}
