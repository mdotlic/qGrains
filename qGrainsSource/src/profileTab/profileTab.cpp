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
#include "profileTab.h"
#include "qGrains.h"
#include "profileLeftWidget.h"
#include "profilePicture.h"
#include "model/handlers/viewHandler.h"
#include "model/handlers/surfHandler.h"
#include "model/handlers/profileHandler.h"
#include "model/handlers/plotHandler.h"
#include "model/handlers/handler.h"
#include "model/modelNodeBase.h"

ProfileTab::ProfileTab(QGrains * qGrains, Handler * handler, 
      PlotHandler * plotHandler)
{
   
   ViewHandler * viewHandler = new ViewHandler(qGrains->model(), 2);
   SurfHandler * surfHandler = new SurfHandler(qGrains->model());
   ProfileHandler * profileHandler = new ProfileHandler(qGrains->model());
   ProfileLeftWidget * profileLeftWidget = new ProfileLeftWidget(qGrains,
         handler, viewHandler, surfHandler, profileHandler, plotHandler);
   QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spLeft.setHorizontalStretch(1);
   profileLeftWidget->setSizePolicy(spLeft);

   ProfilePicture * profilePicture = new ProfilePicture(handler, viewHandler, 
         surfHandler, profileHandler, qGrains);
   QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spRight.setHorizontalStretch(1);
   profilePicture->setSizePolicy(spRight);

   addWidget(profileLeftWidget);
   addWidget(profilePicture);

   connect(qGrains->model(), SIGNAL(loadFinished()), profilePicture, 
         SLOT(rescaleAndRepaint()));
}
