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
#ifndef PROFILEMAP_H
#define PROFILEMAP_H

#include <QWidget>

class ViewHandler;
class QLineEdit;
class QGrains;
class Handler;
class ProfileHandler;

class ProfileMap : public QWidget
{
   Q_OBJECT
   public:
      ProfileMap(Handler *, ViewHandler *, ProfileHandler *, QGrains *);
      QSize minimumSizeHint() const Q_DECL_OVERRIDE;
      
      void mouseDoubleClickEvent(QMouseEvent *) Q_DECL_OVERRIDE;
      void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
      void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      
      double transformX(const double x) const;
      double transformY(const double y) const;
      double inverseTransformX(const double x) const;
      double inverseTransformY(const double y) const;
      void setTransform(const double g, const double h);
      void loadImage(const QString & imageName);       
      double distance(const double x1, const double y1, const double x2, const double y2, const double refX, const double refY) const;
      
      void paint(QPainter *painter, QPaintEvent * event);

   public slots:
      void repaint();
   private slots:
      void contextMenuRequest(QPoint point);
      void imageProp();
      void savePic();
      void setObjectProp();
      void setRange();
      void distanceTool();
      void browse();
      void repaintWrapper(const int &);
      void loadMap();
   protected:
      void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
   private:
      ViewHandler * _viewHandler;
      QGrains * _qGrains;
      Handler * _handler;
      ProfileHandler * _profileHandler;
      QPixmap _pixmap;
      QImage * _image;
      double _transform_a = 1.0;//every point is transform to 
      double _transform_b = 0.0;//f(x)=ax+b
      double _transform_c = 1.0;//f(y)=cx+d
      double _transform_d = 0.0;
      bool _distanceT=false;
      bool _paintDistance=false;
      QPoint _distanceOrigin;
      QPoint _distanceEnd;
      QPoint _lastDragPos;
      bool _drag = false;
      QLineEdit * _lineEdit;
      int _drillRadius = 2;
      int _drillXOffset = 4;
      int _drillYOffset = 4;
};

#endif
