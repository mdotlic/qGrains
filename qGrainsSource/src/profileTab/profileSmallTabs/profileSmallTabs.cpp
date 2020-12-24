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
#include "profileSmallTabs.h"
#include "profileChooser.h"
#include "surfaceChooser.h"

ProfileSmallTabs::ProfileSmallTabs(ModelNodeBase * model, Handler * handler,
      ViewHandler * viewHandler, SurfHandler * surfHandler, ProfileHandler * profileHandler, PlotHandler * plotHandler)
{
   setTabPosition(QTabWidget::West);
   ProfileChooser * profileChooser  = new ProfileChooser(model, handler, 
         viewHandler, profileHandler, plotHandler);
   profileChooser->setFocusPolicy(Qt::ClickFocus);
   QString profileChooserString = QString("Drills");
   
   SurfaceChooser * surfaceChooser  = new SurfaceChooser(handler, surfHandler);
   surfaceChooser->setFocusPolicy(Qt::ClickFocus);
   QString surfaceString = QString("Surfaces");
   
   addTab(profileChooser, profileChooserString);
   addTab(surfaceChooser, surfaceString);

}
