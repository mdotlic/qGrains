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
#ifndef PLANPICTURE
#define PLANPICTURE

#include <QWidget>
class QGrains;
class Handler;
class QLineEdit;
class PlanChooser;
class IsolinesWidget;
class QComboBox;

class PlanPicture : public QWidget
{
   Q_OBJECT
   public:
      PlanPicture(Handler *, QGrains *, PlanChooser *, IsolinesWidget *);
      QSize minimumSizeHint() const Q_DECL_OVERRIDE;
      
      void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
      void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

      double transformX(const double x) const;
      double transformY(const double y) const;
      double inverseTransformX(const double x) const;
      double inverseTransformY(const double y) const;
      void setTransform(const double g, const double h);
      void loadImage(const QString & imageName);       
      double distance(const double x1, const double y1, const double x2, const double y2, const double refX, const double refY) const;

      void paint(QPainter *painter, QPaintEvent * event);
//      double drillNumber(const int &);
   public slots:
      void repaint();
      void calcPicture();
      void calcSurfPicture();
      void calcSurfThicknessPicture();
   private slots:
      void contextMenuRequest(QPoint point);
      void imageProp();
      void savePic();
      void exportToDxf();
      void setObjectProp();
      void setRange();
      void distanceTool();
      void browse();
      void showHideDrillNumber();
      void setTitle();
      void loadMap();
   protected:
      void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

   private:
      void drawText(QPainter * painter, qreal x, qreal y, Qt::Alignment flags,
            const QString & text, QRectF * boundingRect = 0) const;
      void setFullTitle();
      QComboBox * addComboBoxIso(int);
      Handler * _handler;
      QGrains * _qGrains;
      IsolinesWidget * _isolinesWidget;
      QImage * _image;
      QPixmap _pixmap;
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
      int _isoLabelNumber = 3;
      int _isoColor = 0;
      std::vector<int> _color = {Qt::black, Qt::red, Qt::blue, Qt::green, 
      Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
      bool _showDrillNumber = true;
      std::vector<QString> _names={"elevation", "depth", "surface", "thickness"};
      std::vector<QString> _valNames={"d10", "d15",  "d20", "d50", "d60", "d85",
         "Unif. coef.", "Porosity", "Hazen", "Slichter", "Terzaghi", "Beyer", "Sauerbrei", "Kruger", "Kozeny", "Zunker", "Zamarin", "USBR", "Grain size"};
      QString _caption="";
      QString _titleName="";
};

#endif
