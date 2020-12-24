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
#include "planTab.h"
#include "planLeftWidget.h"
#include "planPicture.h"

#include <QDebug>

PlanTab::PlanTab(QGrains * qGrains, Handler * handler,
      PlotHandler * plotHandler)
{
   PlanLeftWidget * planLeftWidget = new PlanLeftWidget(qGrains, handler,
         plotHandler);
   QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spLeft.setHorizontalStretch(1);
   planLeftWidget->setSizePolicy(spLeft);

   PlanPicture * planPicture = new PlanPicture(handler, qGrains, 
         planLeftWidget->planChooser(), planLeftWidget->isolinesWidget());
   QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
   spRight.setHorizontalStretch(2);
   planPicture->setSizePolicy(spRight);

   addWidget(planLeftWidget);
   addWidget(planPicture);

}
