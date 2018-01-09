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
#ifndef PROFILEPICTURE_H
#define PROFILEPICTURE_H

#include <QWidget>
class ViewHandler;
class SurfHandler;
class ProfileHandler;
class QGrains;
class QTextStream;
class Handler;

class ProfilePicture : public QWidget
{
   Q_OBJECT
   public:
      ProfilePicture(Handler *, ViewHandler *, SurfHandler *, ProfileHandler *,
            QGrains *);
      QSize minimumSizeHint() const Q_DECL_OVERRIDE;

      void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
      void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;


      double transformX(const double x) const;
      double transformY(const double y) const;
      double inverseTransformX(const double x) const;
      double inverseTransformY(const double y) const;
      
      void paint(QPainter *painter, QPaintEvent * event);

   public slots:
      void repaint();
      void rescaleAndRepaint();
   private slots:
      void contextMenuRequest(QPoint point);
      void setObjectProp();
      void showDepth();
      void exportToDxf();
      void savePic();
      void repaintWrapper(const int &);
      void rescaleAndRepaintWrapper(const int &);
      void startSurface();
      void addSurfacePoint();
      void selectedChange(const int &);
   protected:
      void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
   private:
      void drawText(QPainter * painter, qreal x, qreal y, Qt::Alignment flags,
            const QString & text, QRectF * boundingRect = 0) const;
      double getTickStep(const double up, const double down) const;
      double cleanMantissa(double input) const;
      double getMantissa(double input, double *magnitude) const;
      double pickClosest(double, const QVector<double> &) const;
      QVector<double> createTickVector(const double &, const double &, 
            const double &, const double &) const;
    /*  void writeLineDxf(const double p1x, const double p1y, const double p2x,
            const double p2y, const QString &, QTextStream & in);
      void writeTextDxf(const double p1x, const double p1y, const QString &,
            QTextStream & in, const int & hpos, const int & vpos, 
            const QString & text);
      void writePointDxf(const double p1x, const double p1y, 
            const QString &, QTextStream & in);*/
      bool findDrillCount(const int &);
//      double distanceFromFirst(const int &);
      //bool pointComparison(const QPointF &, const QPointF &);

      Handler * _handler;
      ViewHandler * _viewHandler;
      SurfHandler * _surfHandler;
      QGrains * _qGrains;
      QImage * _image;
      std::vector<int> _color;
      double _transform_a = 1.0;//every point is transform to 
      double _transform_b = 0.0;//f(x)=ax+b
      double _transform_c = 1.0;//f(y)=cx+d
      double _transform_d = 0.0;
      QPoint _lastDragPos;
      bool _drag = false;
      int _tickSize = 1;
      int _axisTickSize = 2;
      int _fontSize = 10;
      int _axisFontSize = 10;
      int _axisNumbTicks = 16;
      bool _showDepth = false;
      bool _surfaceAdd = false;
      int _drillCount = 0;
      int _selectedSurf = -1;
      int _snap = 10;
      bool _movePoint = false;
      bool _additionalSurfPoints = false;
      std::pair<int, int> _moveSurfPoint;
      std::map<int, QPainterPath > _lines;
      
};

#endif
