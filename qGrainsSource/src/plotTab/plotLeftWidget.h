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
#ifndef PLOTLEFTWIDGET_H
#define PLOTLEFTWIDGET_H

#include <QWidget>
class QLabel;
class QTableView;
class PlotHandler;
class InputHandler;
class ModelNodeBase;
class DepthHeaderLineColorNames;
class CoorHeaderNames;
class QItemSelection;
class QLineEdit;
class Tables;

class PlotLeftWidget : public QWidget
{
   Q_OBJECT
   public:
      PlotLeftWidget(InputHandler * inputHandler, PlotHandler * plotHandler,ModelNodeBase * model);
   private slots:
      void contextMenuRequest(QPoint pos);
      void callUsePlotElev();
      void showDepth(const QItemSelection & sel, const QItemSelection & desel);
      void showCoor(const QItemSelection & sel, const QItemSelection & desel);
      void splineSelectedSlot(const int &, const int &);
      void deselectedSplineSlot();
      void intervalSelect();
   private:
      InputHandler * _inputHandler;
      PlotHandler * _plotHandler;
      QLabel * _dlabel;
      QLabel * _clabel;
      QTableView * _namesTable;
      QTableView * _depthTable;
      Tables * _coorTable;
      DepthHeaderLineColorNames * _dHeaderNames;
      CoorHeaderNames * _cHeaderNames;
      QLineEdit * _fromLineEdit;
      QLineEdit * _toLineEdit;

};
#endif
