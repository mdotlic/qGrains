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
#ifndef PICTURE_H
#define PICTURE_H

#include <QWidget>
#include <QString>
#include <QPixmap>
class QPushButton;
class QImage;
class QGrains;
class InputHandler;

class Picture : public QWidget
{
   Q_OBJECT
   public:
      Picture(QGrains * qGrains, InputHandler *, QPushButton *, QPushButton *);
      void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
      void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

   private slots:
      void contextMenuRequest(QPoint point);
      void setBounds();
      void properties();
      void setPredefined();
   public slots:
      void imageProp();
      void drawRectangle();
      void predefSlot();
   protected:
      void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
   private:
      double logTransfX(const double x) const;
      double logTransfY(const double y) const;
      int logInvX(const double x) const;
      int logInvY(const double y) const;
      int transformX(const double x) const;
      int transformY(const double y) const;
      double inverseTransformX(const double x) const;
      double inverseTransformY(const double y) const;
      QPoint transformPoint(const QPoint & point) const;
      void drawFrame(QPainter & painter);
     // QPointF findRelative (QPoint &);
      //void drawSpline(QPainter &, const std::vector<QPointF> &);
      //std::vector<QPointF> calculateP1(const std::vector<QPointF> &);

      
      QGrains * _qGrains;
      InputHandler * _inputHandler;
      QString _name;
      QPixmap _pixmap;
      QImage * _image;
      QPoint _origin;
      QPoint _tempEnd;
      QPoint _end;//when end is selected it is always changed to be right bott
      //and _origin is set to be left top
      //       double _pictureRatio;//x size/y size i.e. width / height
      std::vector<double> _predefined = {1, 5, 10, 15, 17, 20, 30, 40, 50, 60, 70, 85, 95, 99};
      int _predefCount = -1;
      double _pictX=0;
      double _pictY=0;
      bool _firstPoint=false;
      bool _drawTemp=false;
      bool _drawRect=false;
      QPushButton * _rectButton;
      QPushButton * _predefButton;
      double _transform_a = 1.0;//every point is transform to 
      double _transform_b = 0.0;//f(x)=ax+b
      double _transform_c = 1.0;//f(y)=cx+d
      double _transform_d = 0.0;
      QPoint _lastDragPos;
      bool _drag = false;
      int _leftBIndex = 0;
      int _rightBIndex = 10;
      std::vector<QString> _bVal={"1e-8", "1e-7", "1e-6", "1e-5", "1e-4", 
         "1e-3", "1e-2", "1e-1", "1", "10", "100"};
      std::vector<int> _color = {Qt::black, Qt::red, Qt::blue, Qt::green, 
      Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
      int _tempColor = 4;
      int _rectColor = 1;
      int _pointColor = 2;
      int _lineWidth = 1;
      int _pointRad = 3;
};

#endif
