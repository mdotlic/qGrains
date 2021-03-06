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
#ifndef PERCPICTURE_H
#define PERCPICTURE_H
#include "../thirdparty/qcustomplot/qcustomplot.h"

class ViewHandler;
class SizeHandler;
class QGrains;
class Handler;

class PercPicture : public QCustomPlot
{
   Q_OBJECT
   public:
      PercPicture(Handler *, ViewHandler *, SizeHandler *, QGrains *);
   private slots:
      void contextMenuRequest(QPoint point);
      void setRange();
      void saveImage();
      void changeIsDepth();
      void selectionChanged();
      void mouseWheel();
      void setDataForGraph(const int &, const int &);
      void showCoord(QMouseEvent *);
      void showHideLegend();
      void setTitle();
   public slots:
      void sizeSelChangeSlot(const int &);
      void drillSelChangeSlot(const int &);
      void styleChangeSlot(const int &);
      void changeGraphSize(const int &);
      void drillTickPercSlot(const int &);
      void allPlotSlot();
      //void loadDSlot();
//      void setGraphStyle(const int &, const int &);   
   private:
      bool _isLegend = true;
      ViewHandler * _viewHandler;
      SizeHandler * _sizeHandler;
      std::map<int, int> _graphD;
      std::vector<int> _color = {Qt::black, Qt::red, Qt::blue, 
         Qt::green, Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
      QGrains * _qGrains;
      Handler * _handler;
      QString _caption = "";
};

#endif
