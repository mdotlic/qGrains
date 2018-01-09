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
#ifndef PLOTPICTURE_H
#define PLOTPICTURE_H

#include <QWidget>
#include <set>
class QImage;
class QGrains;
class PlotHandler;
class InputHandler;

class PlotPicture : public QWidget
{
   Q_OBJECT
   public:
      PlotPicture(QGrains * qGrains, PlotHandler * plotHandler, InputHandler *);
      void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
      void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      //void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
   private slots:
      void contextMenuPlotTab(QPoint point);
      void setBounds();
      void properties();
      void drawSplineSlot(const int &, const int &);
      void changeStyleSlot();
      void savePic();
      void sampleSelectedSlot(const int &, const int &);
      void showHideLegend();
      void deleteSplineSlot(const int &, const int &);
   protected:
      void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
   private:
      double logTransfX(const double x) const;
      double logTransfY(const double y) const;
      int logInvX(const double x) const;
      int logInvY(const double y) const;
      QPointF logInvPoint(const QPointF & p);
      int transformX(const double x) const;
      int transformY(const double y) const;
      double inverseTransformX(const double x) const;
      double inverseTransformY(const double y) const;
      QPoint transformPoint(const QPoint & point) const;
      int checkLogInvX(const double x, const QPoint & plf, const QPoint & pbr) const;
      void drawFrame(QPainter & painter);
      void drawSplines(QPainter & painter);
      std::vector<QPointF> calculateP1(const std::vector<QPointF> &);
      QPointF changeToLimit(const QPointF &, const QPointF &, const QPointF &);

      QGrains * _qGrains;
      PlotHandler * _plotHandler;
      QImage * _image;
      QPoint _origin = QPoint(50,150);//for now it is not changeable 
      QPoint _end = QPoint(550, 350);//
      int _leftBIndex = 4;
      int _rightBIndex = 10;
      int _rectColor = 0;
      int _lineRectWidth = 2;
      int _lineThinWidth = 1;
      int _lineLineWidth = 1;
      int _snap = 10;
      int _fontSize = 12;
      double _ratio = 0.4;
      std::vector<QString> _bVal={"1e-8", "1e-7", "1e-6", "1e-5", "1e-4", 
         "1e-3", "1e-2", "1e-1", "1", "10", "100"};
      std::vector<int> _color = {Qt::black, Qt::red, Qt::blue, Qt::green, 
      Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
      bool _drag=false;
      QPoint _lastDragPos;
      double _transform_a = 1.0;//every point is transform to 
      double _transform_b = 0.0;//f(x)=ax+b
      double _transform_c = 1.0;//f(y)=cx+d
      double _transform_d = 0.0;

      std::set<std::pair<int, int> > _drillSampleSpline;
      std::map<std::pair<int, int>, QPainterPath > _splines;
      std::pair<int, int> _selected = std::make_pair(-1, -1);
      QVector<QString> _names = {"zrna u % težine manjih od d", "zrna u % težine većih od d", "prečnik zrna d (mm)", "J.U.S.\nuB1.018", "M.I.T.\nBoston", "GLINA", "fina", "sred", "krup", "PRAŠINA", "prašinast", "sitan", "srednji", "PESAK", "ŠLJUNAK", "OBLUCI"};
      bool _showLegend = false;
      QString _caption = "";
      //std::map<std::pair<int, int>, std::vector<QPointF> > _splinePoints2;
};
#endif
