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

#include "picture.h"
#include <QPainter>
#include <QMenu>
#include <QFileDialog>
#include <QPushButton>
#include <QMouseEvent>
#include <QImage>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QDebug>
#include <math.h>
#include <QStatusBar>
#include <QLineEdit>
#include <QToolTip>

#include "qGrains.h"
#include "centralWindow.h"
#include "inputTab/inputTab.h"
#include "inputTab/leftWidget.h"
#include "model/tables/tables.h"
#include "model/handlers/inputHandler.h"
#include "predefList.h"

Picture::Picture(QGrains * qGrains, InputHandler * inputHandler, 
      QPushButton * rectButton, QPushButton * pButton): _qGrains(qGrains), 
   _inputHandler(inputHandler), _rectButton(rectButton), _predefButton(pButton)
{
   QSize newSize(width(),height());
   _image=new QImage(newSize, QImage::Format_RGB32);
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));

   connect(_inputHandler, SIGNAL(pointChanged()), this, SLOT(repaint()));
   
   setMouseTracking(true);
}

void Picture::mousePressEvent(QMouseEvent * event)
{
   if(event->buttons() == Qt::LeftButton)
   {
      if(_firstPoint)
      {
         _origin = QPoint(inverseTransformX(event->pos().x()), 
               inverseTransformY(event->pos().y()));
         _drawTemp=true;
         _firstPoint=false;
         _rectButton->setText("First point is clicked");
         _tempEnd = QPoint(event->pos().x(), event->pos().y());
      }else if(_drawTemp)
      {
         _drawRect=true;
         _drawTemp=false;
         _firstPoint=false;
         _end = QPoint(inverseTransformX(event->pos().x()), 
               inverseTransformY(event->pos().y()));
         if(_origin.x() > _end.x())
         {
            int temp = _origin.x();
            _origin.setX(_end.x());
            _end.setX(temp);
         }
         if(_origin.y() > _end.y())
         {
            int temp = _origin.y();
            _origin.setY(_end.y());
            _end.setY(temp);
         }
         _rectButton->setText("Draw new rectangle");
         _rectButton->setChecked(false);
         repaint();
      }else if(event->modifiers().testFlag(Qt::ControlModifier) && _predefCount>-1)
      {
         _predefCount++;
         _predefButton->setText("Predefined value - "+QString::number(_predefined[_predefCount]));
         if(_predefCount == (int)_predefined.size())
         {
            _predefButton->setChecked(false);
            predefSlot();
           // _predefButton.setChecked(false);
           // _predefCount=0;
         }
         repaint();
      }
      else
      {//only for drag
         _lastDragPos = event->pos();
         _drag=true;
      }
   }
}

void Picture::mouseMoveEvent(QMouseEvent * event)
{
   if(_drag)
   {
      double moveX = event->pos().x()-_lastDragPos.x();
      double moveY = event->pos().y()-_lastDragPos.y();

      _transform_b += moveX;
      _transform_d += moveY;
      _lastDragPos=event->pos();
      repaint();
   }
   if(_drawTemp)
   {
      _tempEnd=event->pos();
      repaint();
   }
   if(_drawRect)
   {
      //double x = inverseTransformX(event->pos().x());
      //double y = inverseTransformY(event->pos().y());
      // double xval = 0.0;
      // double yval = 0.0;
      //qDebug()<<" ovo bi bilo "<<100.0-100*(y-_origin.y())/(_end.y()-_origin.y())<<" donja "<<_bVal[_leftBIndex].toDouble()*pow(10, exp)<<"\n";
      QString str(QString::number(logTransfX(event->pos().x())));
      str.append("; ");
      if(_predefCount>-1)
         str.append(QString::number(_predefined[_predefCount]));
      else
         str.append(QString::number(logTransfY(event->pos().y())));//y axis
      /*
         QString str(QString::number(event->pos().x()));
         str.append("; ");
         str.append(QString::number(event->pos().y()));
       */
      _qGrains->statusBar()->showMessage(str);
      repaint();

      //qDebug()<<point.x()<<"; "<<point.y()<<" evo me na poziciji ("<<event->pos().x()<<"; "<<event->pos().y()<<") dok je ("<<_origin.x()<<";"<<_origin.y()<<") -- ("<<_end.x()<<";"<<_end.y()<<"\n";
   }
}

void Picture::wheelEvent(QWheelEvent * event)
{

   double zoomInFactor=0.95;
   int numDegrees = -event->delta() / 8;
   double numSteps = numDegrees / 15.0f;
   double zoom=pow(zoomInFactor, numSteps);
   _transform_a*=zoom;
   _transform_b=zoom*_transform_b + event->pos().x()*(1-zoom);
   _transform_c*=zoom;
   _transform_d=zoom*_transform_d + event->pos().y()*(1-zoom);

   repaint();
}

void Picture::mouseReleaseEvent(QMouseEvent * /*event*/)
{
   _drag=false;
}

void Picture::mouseDoubleClickEvent(QMouseEvent * event)
{
   //_newCode->model()->append();
   if(_drawRect)
   {
      double xx =logTransfX(event->pos().x());
      double yy;
      if(_predefCount>-1)
         yy=_predefined[_predefCount];
      else
         yy=logTransfY(event->pos().y());
      
      if((_qGrains->centralWindow()->inputTab()->leftWidget()->depthTable()->currentIndex().row()+1 == _inputHandler->rowCount(_inputHandler->parent(_qGrains->centralWindow()->inputTab()->leftWidget()->depthTable()->currentIndex()))) || (_qGrains->centralWindow()->inputTab()->leftWidget()->depthTable()->currentIndex().row() == -1) )
      {
         QString str(" First select valid sample");
         _qGrains->statusBar()->showMessage(str);
         return;
      }
      QModelIndex index1 = _inputHandler->index(0,0,_qGrains->centralWindow()->inputTab()->leftWidget()->depthTable()->currentIndex());
      QModelIndex index2 = _inputHandler->index(_inputHandler->rowCount(index1)-1, 0, index1);
      _inputHandler->commandCoorAppendPicture(index2, xx, yy);
      if(_predefCount>-1)
      {
         _predefCount++;
         _predefButton->setText("Predefined value - "+QString::number(_predefined[_predefCount]));
         if(_predefCount == (int)_predefined.size())
         {
            _predefButton->setChecked(false);
            predefSlot();
           // _predefButton.setChecked(false);
           // _predefCount=0;
         }
      }
      repaint();
      QString str(" Point ("+QString::number(xx)+"; "+QString::number(yy)+") added ");
      _qGrains->statusBar()->showMessage(str);
   }
   else
   {
      QString str("First draw rectangle, then add points");
      _qGrains->statusBar()->showMessage(str);
   }
}

void Picture::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Set bounds", this, SLOT(setBounds()));
   menu->addAction("Properties", this, SLOT(properties()));
   menu->addAction("Predefined values", this, SLOT(setPredefined()));
   menu->popup(mapToGlobal(pos));
}

void Picture::setBounds()
{
   QDialog * dialog = new QDialog(this);
   QGridLayout * setBoundsLayout = new QGridLayout;
   QLabel * left = new QLabel("Left end");
   setBoundsLayout->addWidget(left, 0,0, 1,1);
   QComboBox * leftCombo = new QComboBox;
   QComboBox * rCombo = new QComboBox;
   for(size_t i=0;i<_bVal.size();i++)
   {
      leftCombo->addItem(_bVal[i]);
      rCombo->addItem(_bVal[i]);
   }

   leftCombo->setCurrentIndex(_leftBIndex);//left combo
   setBoundsLayout->addWidget(leftCombo, 0, 1, 1, 1);
   QLabel * right = new QLabel("Right end");
   setBoundsLayout->addWidget(right, 1,0, 1,1);
   
   rCombo->setCurrentIndex(_rightBIndex);//left combo
   setBoundsLayout->addWidget(rCombo, 1,1,1,1);
   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, dialog);
   setBoundsLayout->addWidget(&buttonBox, 2,0, 1,1);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
   dialog->setLayout(setBoundsLayout);
   if (dialog->exec() == QDialog::Accepted)
   {
      _leftBIndex = leftCombo->currentIndex();
      _rightBIndex = rCombo->currentIndex();
      repaint();
   }
}

QComboBox * addComboBox(int defColor)
{
   QComboBox * comboBox = new QComboBox;
   comboBox->addItem(QIcon(":resources/images/colors/black.png"), QString("black"));
   comboBox->addItem(QIcon(":resources/images/colors/red.png"), QString("red"));
   comboBox->addItem(QIcon(":resources/images/colors/blue.png"), QString("blue"));
   comboBox->addItem(QIcon(":resources/images/colors/green.png"), QString("green"));
   comboBox->addItem(QIcon(":resources/images/colors/yellow.png"), QString("yellow"));
   comboBox->addItem(QIcon(":resources/images/colors/cyan.png"), QString("cyan"));
   comboBox->addItem(QIcon(":resources/images/colors/magenta.png"), QString("magenta"));
   comboBox->addItem(QIcon(":resources/images/colors/gray.png"), QString("gray"));
   comboBox->setCurrentIndex(defColor);//because default is blue
   for(int i = 0; i < comboBox->count(); i++)
    comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
   return comboBox;
}

void Picture::properties()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Properties");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);

   QFrame * rectTempFrame = new QFrame;
   QGridLayout * rectTempLayout = new QGridLayout;
   QLabel * labelTempColor = new QLabel("Temporary color");
   rectTempLayout->addWidget(labelTempColor,1,0);
   QComboBox * tempColorCombo=addComboBox(_tempColor);
   rectTempLayout->addWidget(tempColorCombo,1,1);
   
   QLabel * labelRectColor = new QLabel("Rectangle color");
   rectTempLayout->addWidget(labelRectColor,2,0);
   QComboBox * rectColorCombo=addComboBox(_rectColor);
   rectTempLayout->addWidget(rectColorCombo,2,1);
   
   QLabel * labelPointColor = new QLabel("Point color");
   rectTempLayout->addWidget(labelPointColor,3,0);
   QComboBox * pointColorCombo=addComboBox(_pointColor);
   rectTempLayout->addWidget(pointColorCombo,3,1);
   
   QLabel * lineWidth = new QLabel("Line width");
   rectTempLayout->addWidget(lineWidth,4,0);
   QLineEdit * lineWidthEdit = new QLineEdit(&dialog);
   lineWidthEdit->setText(QString::number(_lineWidth));
   rectTempLayout->addWidget(lineWidthEdit,4,1);

   QLabel * pointRad = new QLabel("Point radius");
   rectTempLayout->addWidget(pointRad,5,0);
   QLineEdit * pointRadEdit = new QLineEdit(&dialog);
   pointRadEdit->setText(QString::number(_pointRad));
   rectTempLayout->addWidget(pointRadEdit,5,1);

   rectTempFrame->setLayout(rectTempLayout);
   rectTempFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(rectTempFrame);
   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);

   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   if (dialog.exec() == QDialog::Accepted) {
      _tempColor = tempColorCombo->currentIndex();
      _rectColor = rectColorCombo->currentIndex();
      _pointColor = pointColorCombo->currentIndex();
      _lineWidth = lineWidthEdit->text().toInt();
      _pointRad = pointRadEdit->text().toInt();
      repaint();
   }
}

void Picture::setPredefined()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Set predefined values");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);

   PredefList * list = new PredefList;
   QListWidgetItem * itemEdit = new QListWidgetItem;
   list->addItem(itemEdit);
   itemEdit->setFlags (itemEdit->flags () | Qt::ItemIsEditable);
   list->openPersistentEditor(itemEdit);

   list->setSortingEnabled(true);

   for(size_t i=0;i<_predefined.size();i++)
   {
      QListWidgetItem *item = new QListWidgetItem;      
      item->setData(Qt::DisplayRole, _predefined[i]);
      list->addItem(item);
   }

   dialogLayout->addWidget(list);

   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);

   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

  // QObject::connect(value, SIGNAL(returnPressed()), this, SLOT(addToList()));

   if (dialog.exec() == QDialog::Accepted) 
   {
      _predefined.resize(list->count()-1);
      for(int i=1;i<list->count();i++)
      {
         _predefined[i-1]=list->item(i)->text().toDouble();
      }
   }
}

void Picture::imageProp()
{
   QString fileName = QFileDialog::getOpenFileName(this);
   if(!fileName.isEmpty())
   {
     // qDebug()<<" imamo da je izabrano "<<fileName<<"\n";
      _name = fileName;    
      _pixmap.load(_name);
      _pictX = (double)_pixmap.size().width();
      _pictY = (double)_pixmap.size().height();
     // qDebug()<<" imamo da je "<<_pixmap.size()<<" tj "<<_pixmap.size().width()<<"/"<<_pixmap.size().height()<<"\n";
      update(0,0,width(),height());
   } 
}

void Picture::drawRectangle()
{
   _firstPoint = true;
   _drawRect=false;
   _drawTemp=false;
   _predefButton->setChecked(false);
   _predefButton->setText("Use predefined values");
}

void Picture::predefSlot()
{
   if(_drawRect)
   {
      if(!_predefButton->isChecked())
      {
         _predefButton->setChecked(false);
         _predefButton->setText("Use predefined values");
         _predefCount = -1;
      }
      else
      {
         _predefButton->setChecked(true);
         _predefButton->setText("Predefined value - "+QString::number(_predefined[0]));
         _predefCount = 0;
      }
   }
   else
   {
      QString str(" First draw rectangle, then you can use predefined values");
      _qGrains->statusBar()->showMessage(str);
      _predefButton->setChecked(false);
   }

}

void Picture::paintEvent(QPaintEvent * event)
{
   QSize newSize(width(), height());
   delete _image;
   _image=new QImage(newSize, QImage::Format_RGB32);
   _image->fill(qRgb(255, 255, 255));

   QPainter painter(_image);
   painter.drawPixmap(QRect(QPoint(transformX(0), transformY(0)), 
            QPoint(transformX(_pictX), transformY(_pictY))), _pixmap);

   painter.drawText(20, 20, _name);

   QPen pen;
   if(_drawRect)
   {
      drawFrame(painter);
      //QPoint pos = mapFromGlobal(QCursor::pos());
      //double x = inverseTransformX(pos.x());
      //double y = inverseTransformY(pos.y());
      painter.drawText(mapFromGlobal(QCursor::pos()), _qGrains->statusBar()->currentMessage());
      if(_predefCount>-1)
      {
         QPointF center (mapFromGlobal(QCursor::pos()).x(),
               logInvY(_predefined[_predefCount]));
         painter.drawLine(center-QPointF(_pointRad, _pointRad), 
               center+QPointF(_pointRad, _pointRad));
         painter.drawLine(center+QPointF(-_pointRad, _pointRad), 
               center+QPointF(_pointRad, -_pointRad));


      }
      QModelIndex index=_qGrains->centralWindow()->inputTab()->leftWidget()->depthTable()->currentIndex();
      if(index.isValid() &&
            _inputHandler->rowCount(_inputHandler->parent(index))>1)
      {
         if(index.row()<_inputHandler->rowCount(_inputHandler->parent(index))-1)
               //_newCode->model()->rowCount(index)>0)
         {
            QModelIndex index1 = _inputHandler->index(0,0,index);

            int nrow = _inputHandler->rowCount(index1)-1;
            //QPainterPath bezierPath;
            //std::vector<QPointF> points;
            for(int i=0;i<nrow;i++)
            {
               pen.setColor(Qt::GlobalColor(_color[_pointColor]));
               pen.setWidth(_lineWidth);
               painter.setPen(pen);
               QPointF center (
                 logInvX(_inputHandler->pointValue(index1,i, 0)), 
                 logInvY(_inputHandler->pointValue(index1,i, 1)));
               painter.drawEllipse(center, _pointRad, _pointRad);
               //////
              // points.push_back(center);
            }
            //drawSpline(painter, points);
         }
      }
   }
   if(_drawTemp)
   {
      pen.setColor(Qt::GlobalColor(_color[_tempColor]));
      pen.setStyle(Qt::DashLine);
      pen.setWidth(_lineWidth);
      painter.setPen(pen);
      painter.drawRect(QRectF(transformPoint(_origin), _tempEnd));
   }

   painter.setPen(palette().dark().color());
   painter.setBrush(Qt::NoBrush);
   painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

   QPainter painterW(this);
   QRect dirtyRect = event->rect();
   painterW.drawImage(dirtyRect, *_image, dirtyRect);

  /* if(_drawRectangle)
   {
      painter->drawLine(_origin, _end);
   }*/

}

double Picture::logTransfX(const double x) const
{
   double transfX = inverseTransformX(x);
   double exp = (_rightBIndex-_leftBIndex)*(transfX-_origin.x())/(_end.x()-_origin.x());
   return _bVal[_leftBIndex].toDouble()*pow(10, exp);
}

double Picture::logTransfY(const double y) const
{
   double transfY = inverseTransformY(y);
   return 100.0-100*(transfY-_origin.y())/(_end.y()-_origin.y());
}

int Picture::logInvX(const double x) const
{
   double z = log10(x/_bVal[_leftBIndex].toDouble())*(_end.x()-_origin.x())/
      (_rightBIndex-_leftBIndex)+_origin.x();
   return transformX(z);
}

int Picture::logInvY(const double y) const
{
   double z = (100.0-y)*(_end.y()-_origin.y())/100 + _origin.y();
   return transformY(z);
   //return 100.0-100*(transfY-_origin.y())/(_end.y()-_origin.y());
}

int Picture::transformX(const double x) const
{
   return _transform_a*x+_transform_b;
}

int Picture::transformY(const double y) const
{
   return _transform_c*y+_transform_d;
}

double Picture::inverseTransformX(const double x) const
{
   return (1/_transform_a)*x-_transform_b/_transform_a;
}

double Picture::inverseTransformY(const double y) const
{
   return (1/_transform_c)*y-_transform_d/_transform_c;
}

QPoint Picture::transformPoint(const QPoint & point) const
{
   return QPoint(transformX(point.x()), transformY(point.y()));
}

void Picture::drawFrame(QPainter & painter)
{
   QPoint plf = transformPoint(_origin);//topleft point
   QPoint pbr = transformPoint(_end);//bottom right point
   
   QPen pen;
   pen.setColor(Qt::GlobalColor(_color[_rectColor]));
   pen.setStyle(Qt::SolidLine);
   pen.setWidth(_lineWidth);
   painter.setPen(pen);
   painter.drawRect(QRectF(plf, pbr));
   double dy = (pbr.y()-plf.y())/10.0;//note that y is negative oriented
   for(size_t i=0;i<=10;i++)
   {
      painter.drawLine(plf.x()-5, plf.y()+i*dy, plf.x(), plf.y()+i*dy);
      if(i==0)
         painter.drawText(plf.x()-32, plf.y()+i*dy+5, QString::number(10*(10-i)));
      else if(i==10)
         painter.drawText(plf.x()-15, plf.y()+i*dy+5, QString::number(10*(10-i)));
      else
         painter.drawText(plf.x()-23, plf.y()+i*dy+5, QString::number(10*(10-i)));

   }
   int downInterv = fabs(_rightBIndex - _leftBIndex);
   double dx = ((double)(pbr.x()-plf.x()))/downInterv;
   for(int i=0;i<=downInterv;i++)
   {
      painter.drawLine(plf.x()+i*dx, pbr.y(), plf.x()+i*dx, 
            pbr.y()+5);
      int j = _leftBIndex < _rightBIndex ? _leftBIndex+i : _leftBIndex -i;
      painter.drawText(plf.x()+i*dx-6, pbr.y()+17, _bVal[j]);
   }
}

/*
void Picture::drawSpline(QPainter & painter, const std::vector<QPointF> & points)
{//based on https://medium.com/@ramshandilya/draw-smooth-curves-through-a-set-of-points-in-ios-34f6d73c8f9

   if(points.size()<2)
      return;
   if(points.size()==2)
   {
      painter.drawLine(points[0], points[1]);
      return;
   }
   
   std::vector<QPointF> P1=calculateP1(points);

   QPainterPath spline;
   spline.moveTo(points[0]);
   size_t n=points.size();
   for(size_t i=0;i<n-2;i++)
   {
      QPointF P2 = 2*points[i+1]-P1[i+1];
      spline.cubicTo(P1[i], P2, points[i+1]);
      qDebug()<<i<<" drugo tacka "<<logTransfX(P1[i].x())<<"; "<<logTransfY(P1[i].y())<<"\n";
   }
   qDebug()<<n-2<<" drugo tavcka "<<logTransfX(P1[n-2].x())<<"; "<<logTransfY(P1[n-2].y())<<"\n";
   QPointF P2 = 0.5*(points[n-1]+P1[n-2]);
   spline.cubicTo(P1[n-2], P2, points[n-1]);
   painter.drawPath(spline);
}

std::vector<QPointF> Picture::calculateP1(const std::vector<QPointF> & points)
{//matrix is [2 1 ... ; 1 4 1 ...; ...; ... 2 7]
   
   size_t n=points.size()-1;//because there is n-1 P1 control points
   std::vector<double> cprim(n-1);
   std::vector<QPointF> dprim(n);
   cprim[0]=0.5;
   dprim[0]=(points[0]+2*points[1])/2;
   for(size_t i=1; i<n-1; i++)
   {
      double down = 4-cprim[i-1];
      cprim[i]=1/down;
      QPointF di = 4*points[i]+2*points[i+1];
      dprim[i] = (di - dprim[i-1])/down;
   }
   dprim[n-1]=(8*points[n-1]+points[n]-2*dprim[n-2])/(7-2*cprim[n-2]);
   std::vector<QPointF> x(n);
   x[n-1]=dprim[n-1];
   for(size_t i=2; i<n+1; i++)
   {
      x[n-i]=dprim[n-i]-cprim[n-i]*x[n-i+1];
   }
   return x;
}*/
