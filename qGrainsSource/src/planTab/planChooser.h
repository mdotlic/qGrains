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
#ifndef PLANCHOOSER_H
#define PLANCHOOSER_H

#include <QWidget>

class QComboBox;
class QLineEdit;
class QHBoxLayout;
class Handler;
class PlotHandler;
class QGrains;

class PlanChooser : public QWidget
{
   Q_OBJECT
   public: 
      PlanChooser(QGrains *, Handler *, PlotHandler *);
   private slots:
      void contextMenuRequest(QPoint pos);
      void selectOnPlotTab();
      void indexChangedBySlot(int index);
      void indexChangedValSlot(int index);
      void setPlanFrom();
      void setPlanTo();
      void setSurfaceId(int id);
      void setSurfaceFrom(int id);
      void setSurfaceTo(int id);
      void setPlanFromSize();
      void setPlanToSize();
      void setLoad();

   signals:
      void changePicture();
      void changeSurfPicture();
      void changeSurfThicknessPicture();
      
   private:
      QComboBox * _byComboBox;
      QComboBox * _valComboBox;
      QLineEdit * _fromLineEdit;
      QLineEdit * _toLineEdit;
      QLineEdit * _fromSizeLineEdit;
      QLineEdit * _toSizeLineEdit;
      QHBoxLayout * _hByLayout;
      QHBoxLayout * _hValLayout;
      Handler * _handler;
      PlotHandler * _plotHandler;
};

#endif
