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

#ifndef CENTRALWINDOW_H
#define CENTRALWINDOW_H

#include <QTabWidget>

class QGrains;
class InputTab;
class DepthTab;
class Handler;

class CentralWindow : public QTabWidget
{
   Q_OBJECT
   public:
      CentralWindow(QGrains *);
      Handler * handler() {return _handler;}
      InputTab * inputTab() {return _inputTab;}
      DepthTab * depthTab() {return _depthTab;}
   private:
      InputTab * _inputTab;
      DepthTab * _depthTab;
      Handler * _handler;
};

#endif
