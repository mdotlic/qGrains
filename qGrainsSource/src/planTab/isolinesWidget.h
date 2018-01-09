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
#ifndef ISOLINESWIDGET_H
#define ISOLINESWIDGET_H
#include <QFrame>

class QLineEdit;
class Handler;
class QPushButton;
class IsoList;
class QGrains;

class IsolinesWidget : public QFrame
{
   Q_OBJECT
   public:
      IsolinesWidget(QGrains * qGrains, Handler * handler);
      void setXmin(const double &);
      void setXmax(const double &);
      void setYmin(const double &);
      void setYmax(const double &);
      void setDrillColor(const int &, const int &);
      void setDrillNumber(const int &, const double &);
      int drillColor(const int &);
      double drillNumber(const int &);
      void resizeDrillsColorNumber();
      bool drillsColorExist();
      int isoSize();
      int isoLinesSize(const int &);
      QPointF line1P(const int & isol, const int & iline);
      QPointF line2P(const int & isol, const int & iline);
      void clearIsoList();
   private slots:
      void calculate();
      void changeXmin();
      void changeXmax();
      void changeYmin();
      void changeYmax();
      void changeXdiv();
      void changeYdiv();
      void calcMarchingSquares();
      void addToList();
      void deleteFromList(int i);
      void listCh();
      void automaticIsoValues();
      void somethingChangedSlot();
      void loadFinishedSlot();
   signals:
      void repaintPict();
      void somethingChanged();
   private:
      QPointF interp(const QPointF & p0, const QPointF & p1, 
      const double value0, const double value1, const double value) const;

      QGrains * _qGrains;
      Handler * _handler;
      QLineEdit * _xmin;
      QLineEdit * _xmax;
      QLineEdit * _ymin;
      QLineEdit * _ymax;
      QLineEdit * _xdiv;
      QLineEdit * _ydiv;
      QLineEdit * _isoVal;
      IsoList * _list;
      QPushButton * _calculateButton;
      QPushButton * _showButton;
      std::vector<int> _drillColor;
      std::vector<double> _drillNumber;
      std::vector<double> _values;
      double _maxVal;
      double _minVal;
      QVector<QVector<QLineF> > _lines;

};

#endif
