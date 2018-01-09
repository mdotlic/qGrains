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

#ifndef QGRAINS_H
#define QGRAINS_H
#include "dialogs/message.h"
#include <QMainWindow>

class CentralWindow;
class QStatusBar;
class ModelNodeBase;
class MainMenu;
class Message;

class QGrains: public QMainWindow
{
   Q_OBJECT
   public:
      QGrains();
      //~NewCode();

      void clear();
      void initialize();

      bool modified(){return _modified;}     
      void setModified(const bool & val) {_modified = val;}
      QStatusBar * statusBar() { return _statusBar; }
      ModelNodeBase * model() {return _model;}
      CentralWindow * centralWindow() {return _centralWindow;}
      MainMenu * mainMenu(){return _mainMenu;}
      QString title() {return _title;}
      void closeEvent (QCloseEvent*);

      void error(const QString& text) { return _message->newError(text); }
      void warning(const QString& text) { return _message->newWarning(text); }
      int modalWarning(QWidget * parent,const QString & text) 
      { return _message->newModalWarning(parent,text); }
   private slots:
      void modelChangedSlot();

   private:
      bool _modified = false;
      CentralWindow * _centralWindow;
      QStatusBar * _statusBar;
      QString _title = "qGrains";
      ModelNodeBase * _model;
      MainMenu * _mainMenu;
      Message * _message;
};

#endif
