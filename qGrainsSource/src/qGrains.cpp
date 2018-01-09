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
//
#include "qGrains.h"
#include "centralWindow.h"
#include "mainMenu.h"
#include "model/modelNodeBase.h"
#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QCloseEvent>
#include <QDebug>

QGrains::QGrains() : QMainWindow()
{
   //_model = new ModelNodeBase();
   initialize();
   _modified=false;
   
  /* _model->commandDrillAppend(" prva ");
   _model->commandDrillAppend(" druga ");*/
   setWindowTitle(_title);
   resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
   QRect screenGeometry = QApplication::desktop()->screenGeometry();

   _mainMenu = new MainMenu(this);
   setMenuBar(_mainMenu);

   //_centralWindow = new CentralWindow(this);
   //setCentralWidget(_centralWindow);
   
   _statusBar = new QStatusBar();
   //_statusBar->showMessage("NESTO", 20000);
   setStatusBar(_statusBar);
   _message = new Message(this);
   show();
   int x = (screenGeometry.width() - size().width()) / 2;
   int y = (screenGeometry.height() - size().height()) / 2;
   move(x,y);
   connect(_model, SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));
}

void QGrains::clear()
{
   delete _model;
   delete _centralWindow;
}

void QGrains::initialize()
{
   _model = new ModelNodeBase();
   _centralWindow = new CentralWindow(this);
   setCentralWidget(_centralWindow);
}

void QGrains::modelChangedSlot()
{
   _modified=true;
}

void QGrains::closeEvent(QCloseEvent * event)
{
   _mainMenu->quit();
   event->ignore();
}
