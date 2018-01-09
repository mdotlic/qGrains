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
#ifndef CONDLEFTWIDGET_H
#define CONDLEFTWIDGET_H

#include <QWidget>
class QLabel;
class QTableView;
class CondHandler;
class ModelNodeBase;
class DepthHeaderNames;
class CoorHeaderNames;
class QItemSelection;
class QLineEdit;
class QButtonGroup;
class CondHandler;
class CondTable;

class CondLeftWidget : public QWidget
{
   Q_OBJECT
   public:
      CondLeftWidget(ModelNodeBase *, CondHandler *, CondTable *);
      CondHandler * condHandler() {return _condHandler;}
   private slots:
      void showDepth(const QItemSelection & sel, const QItemSelection & desel);
      void intervalSelect();
      void buttonReleasedSlot(int);
      void loadCondSlot();
   private:
      QLabel * _dlabel;
      QTableView * _namesTable;
      QTableView * _depthTable;
      CondHandler * _condHandler;
      DepthHeaderNames * _dHeaderNames;
      QLineEdit * _fromLineEdit;
      QLineEdit * _toLineEdit;
      QButtonGroup * _buttonGroup;
      CondTable * _condTable;

};
#endif
