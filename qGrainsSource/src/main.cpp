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

#include <QApplication>
#include <QSplashScreen>
#include <QTimer>

#include "qGrains.h"
#include "mainMenu.h"


int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   QGrains qGrains;//initialization of everything

   QPixmap pixmap(":/resources/images/qGrainsSplash.png");
   QSplashScreen * splash = new QSplashScreen;
   splash->setPixmap(pixmap.scaledToHeight(600,Qt::SmoothTransformation));
   splash->show();

   QTimer::singleShot(2000,splash,SLOT(close()));

   if(argc==2)
   {
      QString fileName= QString::fromStdString(argv[1]);
      qGrains.clear();
      qGrains.initialize();
      qGrains.mainMenu()->loadModel(fileName);
      QString title = qGrains.title();
      qGrains.setWindowTitle(title.append(" - ").append(fileName));
      qGrains.mainMenu()->setCurrentFile(fileName);
      //     emit qGrains->centralWindow()->depthTab()->depthDTab()->chooserWidget()->dChooser()->dHandler()->loadFinished();
   }


   qGrains.show();
   return app.exec();

}
