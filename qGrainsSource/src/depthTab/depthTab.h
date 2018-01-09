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
#ifndef DEPTHTAB_H
#define DEPTHTAB_H

#include <QTabWidget>
class QGrains;
class DepthDTab;
class PercTab;
class Handler;
/*class ChooserWidget;
class DepthDPicture;*/

class DepthTab : public QTabWidget
{
   Q_OBJECT
   public:
      DepthTab(QGrains *, Handler *);
      DepthDTab * depthDTab() {return _depthDTab;}
      PercTab * percTab() {return _percTab;}
   private:
      DepthDTab * _depthDTab;
      PercTab * _percTab;
};



#endif
