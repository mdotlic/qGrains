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
#ifndef DEPTHDTAB_H
#define DEPTHDTAB_H

#include <QSplitter>
class QGrains;
class ChooserWidget;
class DepthDPicture;
class Handler;

class DepthDTab : public QSplitter
{
   Q_OBJECT
   public:
      DepthDTab(Handler *, QGrains *);
      ChooserWidget * chooserWidget() {return _chooserWidget;}
   private:
      ChooserWidget * _chooserWidget;
      DepthDPicture * _depthDPicture;      
};
#endif
