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
#include "profilePicture.h"
#include "model/handlers/handler.h"
#include "model/handlers/viewHandler.h"
#include "model/handlers/surfHandler.h"
#include "model/handlers/profileHandler.h"
#include "qGrains.h"
#include "calculation/calculation.h"
#include "dxf/dxf.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMenu>
#include <QFileDialog>
#include <QStatusBar>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDebug>
#include <cmath>


ProfilePicture::ProfilePicture(Handler * handler, ViewHandler * viewHandler, 
      SurfHandler * surfHandler, ProfileHandler * profileHandler, QGrains * qGrains):_handler(handler), _viewHandler(viewHandler),_surfHandler(surfHandler), _qGrains(qGrains)
{
   _color = {Qt::black, Qt::red, Qt::blue, Qt::green, 
      Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
   QSize newSize(width(),height());
   _image=new QImage(newSize, QImage::Format_RGB32);

   setMouseTracking(true);
   setBackgroundRole(QPalette::Base);
   setAutoFillBackground(true);
   
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));

   connect(_viewHandler, SIGNAL(drillProfSelectionChange(const int &)), this, 
         SLOT(rescaleAndRepaintWrapper(const int &)));
   connect(_viewHandler, SIGNAL(drillProfColorChange(const int &)), this, 
         SLOT(repaintWrapper(const int &)));
   connect(_viewHandler, SIGNAL(typeCalcChanged()), this, SLOT(repaint()));
   connect(_surfHandler, SIGNAL(surfCheckChanged()), this, SLOT(repaint()));
   connect(_surfHandler, SIGNAL(selectedInTable(const int &)), this, 
         SLOT(selectedChange(const int &)));
   connect(profileHandler, SIGNAL(selectedInTable(const int &)), 
         this, SLOT(rescaleAndRepaintWrapper(const int &)));
}

QSize ProfilePicture::minimumSizeHint() const
{
    return QSize(150,150);
}

void ProfilePicture::mousePressEvent(QMouseEvent * event)
{
   if(event->buttons() == Qt::LeftButton)
   {
      if(_selectedSurf>-1)
      {
         for(int i=0;i<_handler->nSurfPoints(_selectedSurf);i++)
         {
            if(fabs(event->pos().y() - 
                 transformY(_handler->surfPointZ(_selectedSurf, i)))<_snap
                  && fabs(event->pos().x() - transformX(Calculation::
                        distanceFromFirst(_handler->surfPointDrill(
                              _selectedSurf,i), _handler)))<_snap)
            {
               _movePoint = true;
               _moveSurfPoint = std::make_pair(_selectedSurf, i);
               return;
            }
         }
      }
      _lastDragPos = event->pos();
      _drag=true;
   }
}

void ProfilePicture::mouseMoveEvent(QMouseEvent * event)
{
   if(_drag)
   {
      double moveX = event->pos().x()-_lastDragPos.x();
      double moveY = event->pos().y()-_lastDragPos.y();

      _transform_b += moveX;
      _transform_d -= moveY;
      _lastDragPos=event->pos();
      repaint();
   }else if(_movePoint)
   {
      _surfHandler->changePointHeight(_moveSurfPoint.first, _moveSurfPoint.second, inverseTransformY(event->pos().y()));
      QString str="Picture ("+QString::number(inverseTransformX(event->pos().x()), 'f', 2)+";"+QString::number(inverseTransformY(event->pos().y()), 'f', 2)+")";
      _qGrains->statusBar()->showMessage(str);
      repaint();
   }
   else{
      QString str="Picture ("+QString::number(inverseTransformX(event->pos().x()), 'f', 2)+";"+QString::number(inverseTransformY(event->pos().y()), 'f', 2)+")";
      _qGrains->statusBar()->showMessage(str);

      if(_surfaceAdd)
      {
         repaint();
      }
   }
}

void ProfilePicture::mouseDoubleClickEvent(QMouseEvent * event)
{
   if(_surfaceAdd)
   {
      int iprofile = _handler->activeProfile();
      int idrill = _handler->profilePoint(iprofile, _drillCount);
      if(_additionalSurfPoints)
         _surfHandler->addPointOldSurface(_handler->drillX(idrill), 
            _handler->drillY(idrill), inverseTransformY(event->pos().y()),
            idrill, _selectedSurf);
      else
         _surfHandler->addPointSurface(_handler->drillX(idrill), 
            _handler->drillY(idrill), inverseTransformY(event->pos().y()),
            idrill); 

      _drillCount++;
      if(_selectedSurf>=0)
      {
         if(!findDrillCount(_drillCount))
         {
            _surfaceAdd = false;
            _drillCount = 0;
            _additionalSurfPoints = false;
         } 
      }
      if(_drillCount == _handler->nProfilePoints(iprofile))
      {
         _surfaceAdd = false;
         _drillCount = 0;
      }
      repaint();   
   }
}

void ProfilePicture::wheelEvent(QWheelEvent * event)
{
   double zoomInFactor=0.95;
   int numDegrees = -event->delta() / 8;
   double numSteps = numDegrees / 15.0f;
   double zoom=pow(zoomInFactor, numSteps);
   
   if(event->modifiers().testFlag(Qt::ControlModifier))
   {
      _transform_a*=zoom;
      _transform_b=zoom*_transform_b + event->pos().x()*(1-zoom);
   }
   else if (event->modifiers().testFlag(Qt::ShiftModifier))
   {
      _transform_c*=zoom;
      _transform_d=zoom*_transform_d + event->pos().y()*(1-zoom);
   }
   else
   {
      _transform_a*=zoom;
      _transform_b=zoom*_transform_b + event->pos().x()*(1-zoom);
      _transform_c*=zoom;
      _transform_d=zoom*_transform_d + event->pos().y()*(1-zoom);
   }
   repaint();
}

void ProfilePicture::mouseReleaseEvent(QMouseEvent * event)
{
   _drag=false;
   _movePoint=false;
   if(_surfaceAdd)
      return;
   for(auto& it:_lines)
   {
      if(it.second.elementCount()==1)
      {
         if(fabs(it.second.elementAt(0).x - event->pos().x())<4 && 
               fabs(it.second.elementAt(0).y - event->pos().y())<4)
         {
            _selectedSurf = it.first;
            emit _surfHandler->lineSelected(_selectedSurf);
            repaint();
            return;
         }
      }
      if(it.second.contains(event->pos()))      
      {
         _selectedSurf = it.first;
         emit _surfHandler->lineSelected(_selectedSurf);
         repaint();
         return;
      }
   }
   if(!_additionalSurfPoints)
      _selectedSurf = -1;//std::make_pair(-1,-1);
   repaint();
}

double ProfilePicture::transformX(double x) const
{
   return _transform_a*x+_transform_b;
}

double ProfilePicture::transformY(double y) const
{
   return height()-(_transform_c*y+_transform_d);
}

double ProfilePicture::inverseTransformX(double x) const
{
   return (1/_transform_a)*x-_transform_b/_transform_a;
}

double ProfilePicture::inverseTransformY(double y) const
{
   return (1/_transform_c)*(height()-y)-_transform_d/_transform_c;
}


bool pointComparison(const QPointF & a, const QPointF & b)
{
   return a.x()<b.x();
}

void ProfilePicture::paint(QPainter *painter, QPaintEvent * event)
{
   painter->setRenderHint(QPainter::Antialiasing, true);
   QPen pen;
   //painter->setFont(QFont("times",_axisFontSize));
   //QPen pen(QPen(Qt::red));
   pen.setStyle(Qt::SolidLine);
   pen.setColor(Qt::GlobalColor(Qt::black));
   pen.setWidth(2);
   painter->setPen(pen);
   
   int iprofile = _handler->activeProfile();
   drawText(painter, width()/2, 0, 
            Qt::AlignTop | Qt::AlignHCenter, _handler->profileName(iprofile));
   painter->setFont(QFont("times",_axisFontSize));
  

   double invY0 = inverseTransformY(0);
   double invYH = inverseTransformY(height());

   double step = getTickStep(invY0, invYH);
   QVector<double> tickVector = createTickVector(step, invY0, invYH, 
         cleanMantissa(invYH));

   int ntick = tickVector.size();
   for(int i=0;i<ntick;i++)
   {
      double invYi = transformY(tickVector[i]);
      
     /* pen.setStyle(Qt::DotLine);
      pen.setWidth(0);
      painter->setPen(pen);*/
      painter->setPen(QPen(QColor(80,80,80), 0, Qt::DotLine));
      painter->drawLine(0, invYi, width(), invYi);

      pen.setStyle(Qt::SolidLine);
      pen.setWidth(2);
      painter->setPen(pen);
      painter->drawLine(0, invYi, _axisTickSize, invYi);
      drawText(painter, _axisTickSize+3, invYi, Qt::AlignVCenter, 
            QString::number(tickVector[i]));
      painter->drawLine(width()-_axisTickSize-1, invYi, width()-1, invYi);
      drawText(painter, width()-_axisTickSize-4, invYi, 
            Qt::AlignVCenter | Qt::AlignRight, QString::number(tickVector[i]));
   }

   painter->setFont(QFont("times",_fontSize));
   double rho = 0.0;
   double rho1 =0.0;

   for(int i=0;i<_handler->nProfilePoints(iprofile); i++)
   {
      int idrill = _handler->profilePoint(iprofile, i);
      if(i!=0)
      {
         int first = _handler->profilePoint(iprofile, i-1);
         rho+=sqrt(pow(_handler->drillX(first)-_handler->drillX(idrill), 2) + pow(_handler->drillY(first)-_handler->drillY(idrill), 2) );
      }
      if(_drillCount == i)
         rho1=rho;
      double x=transformX(rho);
      double y0=transformY(_handler->elev(idrill));
      double y1=transformY(_handler->elev(idrill)-_handler->depth(idrill));
     
      pen.setColor(Qt::GlobalColor(_color[_viewHandler->profileColor(idrill)]));
      painter->setPen(pen);
      painter->drawLine(x, y0, x, y1);
      painter->drawLine(x-2*_tickSize, y0, x+2*_tickSize, y0);
      painter->drawLine(x-2*_tickSize, y1, x+2*_tickSize, y1);
      drawText(painter, x, y0, Qt::AlignBottom | Qt::AlignHCenter, _viewHandler->drillName(idrill));
      for(int j=0;j<_viewHandler->nSamples(idrill); j++)
      {
         double sy0=transformY(_handler->elev(idrill) - 
               _viewHandler->sampleVal(idrill, j, 0));
         double sy1=transformY(_handler->elev(idrill) -
               _viewHandler->sampleVal(idrill, j, 1));
         painter->drawLine(x-_tickSize, sy0, x+_tickSize, sy0);
         painter->drawLine(x-_tickSize, sy1, x+_tickSize, sy1);
         bool correct = false;
         QString value = Calculation::calcValue(idrill, j, _handler, 
                  _handler->typeCalc(), _handler->fromProfile(), 
                  _handler->toProfile(), correct);
         if(!correct)
         {
            pen.setColor(Qt::GlobalColor(Qt::darkGray));
            painter->setPen(pen);
         }else{
            pen.setColor(Qt::GlobalColor(_color[_viewHandler->profileColor(idrill)]));
            painter->setPen(pen);
         }
         drawText(painter, x+_tickSize+3, (sy0+sy1)/2, Qt::AlignVCenter, value);

         if(_showDepth)
         {
            double yy0=_viewHandler->sampleVal(idrill, j, 0);
            double yy1=_viewHandler->sampleVal(idrill, j, 1);
            drawText(painter, x-_tickSize-3, sy0,
                  Qt::AlignVCenter | Qt::AlignRight, QString::number(yy0));
            drawText(painter, x-_tickSize-3, sy1,
                  Qt::AlignVCenter | Qt::AlignRight, QString::number(yy1));
//            drawText(painter, x-_tickSize-3, sy1, 
  //                Qt::AlignVCenter | Qt::AlignRight, QString::number(yy1));
         }
      }
   }

   if(_surfaceAdd)
   {
      pen.setWidth(2);
      pen.setColor(Qt::GlobalColor(Qt::magenta));
      painter->setPen(pen);

      //painter->setPen(QPen(QColor(0,0,0), 0));
      QPointF center (transformX(rho1), mapFromGlobal(QCursor::pos()).y());
      painter->drawLine(center-QPointF(4, 4), center+QPointF(4, 4));
      painter->drawLine(center+QPointF(-4, 4), center+QPointF(4, -4));
   }

   for(int i=0;i<_handler->nSurfaces(); i++)
   {
      if(_selectedSurf==i || (!_additionalSurfPoints && 
               _surfaceAdd && i==_handler->nSurfaces()-1))
      {
         pen.setColor(Qt::GlobalColor(Qt::magenta));
         painter->setPen(pen);
      }
      else
      {
         pen.setColor(Qt::GlobalColor(Qt::black));
         painter->setPen(pen);
      }
      if(_surfHandler->isCheckedSurf(i))
      {
         std::vector<QPointF> points;
         for(int j=0;j<_handler->nSurfPoints(i);j++)
         {
            if(!_viewHandler->isCheckedDrillProfile(
                     _handler->surfPointDrill(i,j)))
               continue;
            double x=transformX(Calculation::distanceFromFirst(
                     _handler->surfPointDrill(i,j), _handler));
            QPointF center (x, transformY(_handler->surfPointZ(i,j)));
            points.push_back(center);
            painter->drawLine(center-QPointF(4, 4), center+QPointF(4, 4));
            painter->drawLine(center+QPointF(-4, 4), center+QPointF(4, -4));
         }

         std::sort(points.begin(), points.end(), pointComparison);

         QPainterPathStroker stroker;
         stroker.setWidth(_snap);
         stroker.setJoinStyle(Qt::MiterJoin);
         
         //QPainterPath nline = stroker.createStroke(line);          

         if(points.size()>0)
         {
            QPainterPath line;
            line.moveTo(points[0]);
            for(size_t k=1;k<points.size();k++)
            {
               //            painter->drawLine(points[i-1], points[i]);
               line.lineTo(points[k]);
               QPointF mid = (points[k]+points[k-1])/2;
               drawText(painter, mid.x(), mid.y(), Qt::AlignBottom | Qt::AlignHCenter, _handler->surfName(i));
            }
            QPainterPath nline = stroker.createStroke(line);
            _lines[i]=nline;
            painter->drawPath(line);
         }
      }
   }

   /*std::sort(points.begin(), points.end(), pointComparison);
   for(size_t i=1;i<points.size();i++)
   {
      painter->drawLine(points[i-1], points[i]);
   }*/

   painter->setPen(palette().dark().color());
   painter->setBrush(Qt::NoBrush);
   painter->drawRect(QRect(0, 0, width() - 1, height() - 1));

   QPainter painterW(this);
   QRect dirtyRect = event->rect();
   painterW.drawImage(dirtyRect, *_image, dirtyRect);
}

void ProfilePicture::rescaleAndRepaint()
{
   double rho=0.0;
   double min = std::numeric_limits<double>::max();
   double max = -std::numeric_limits<double>::max();
   int iprofile = _handler->activeProfile();
   for(int i=0;i<_handler->nProfilePoints(iprofile); i++)
   {
      int idrill = _handler->profilePoint(iprofile, i);
      if(i!=0)
      {
         int first = _handler->profilePoint(iprofile, i-1);
         rho+=sqrt(pow(_handler->drillX(first)-_handler->drillX(idrill), 2) + pow(_handler->drillY(first)-_handler->drillY(idrill), 2) );
      }
      
      if(_handler->elev(idrill)>max)
         max=_handler->elev(idrill);
      if((_handler->elev(idrill)-_handler->depth(idrill)) < min)
         min=_handler->elev(idrill)-_handler->depth(idrill);
   }
   if(min>max)
   {
      double tmp = min;
      min = max;
      max = tmp;
   }
   if(rho!=0.0)
   {
      _transform_b = 25.0;//because xmin is always 0
      _transform_a = (width()-50)/rho;
      if(min != std::numeric_limits<double>::max() && max != -std::numeric_limits<double>::max())
      {
         _transform_c = (height()-50)/(max-min);
         _transform_d = 25-(_transform_c*min);
      }
   }else{
      _transform_b = 25.0;//because xmin is always 0
      _transform_a = 1.0;//no need for scaling
      if(min != -std::numeric_limits<double>::max() && max != std::numeric_limits<double>::max())
      {
         _transform_c = (height()-50)/(max-min);
         _transform_d = 25-(_transform_c*min);
      }
   }
   repaint();
}

void ProfilePicture::repaint()
{
   update(0,0,width(),height());
}

void ProfilePicture::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Save image", this, SLOT(savePic()));
   menu->addAction("Export to dxf", this, SLOT(exportToDxf()));
   if(_showDepth)
      menu->addAction("Hide depth", this, SLOT(showDepth()));
   else
      menu->addAction("Show depth", this, SLOT(showDepth()));
   menu->addAction("Objects properties", this, SLOT(setObjectProp()));
   menu->addAction("Start surface", this, SLOT(startSurface()));
   if(_selectedSurf != -1 && !_additionalSurfPoints)
      menu->addAction("Add point to selected surface", this, SLOT(addSurfacePoint()));
      

   menu->popup(mapToGlobal(pos));
}

void ProfilePicture::setObjectProp()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Object properties");
   QGridLayout * gridLayout = new QGridLayout(&dialog);

   QLabel * labelSnap = new QLabel("Snap");
   gridLayout->addWidget(labelSnap,0,0);
   QLineEdit * lineEditSnap = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditSnap,0,1);
   lineEditSnap->setText(QString::number(_snap));

   QLabel * labelTick = new QLabel("Drills tick size");
   gridLayout->addWidget(labelTick,1,0);
   QLineEdit * lineEditTickSize = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditTickSize,1, 1);
   lineEditTickSize->setText(QString::number(_tickSize));

   QLabel * labelFontSize = new QLabel("Drills font size");
   gridLayout->addWidget(labelFontSize,2,0);
   QLineEdit * lineEditFontSize = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditFontSize,2,1);
   lineEditFontSize->setText(QString::number(_fontSize));
   
   QLabel * labelATick = new QLabel("Axis tick size");
   gridLayout->addWidget(labelATick,3,0);
   QLineEdit * lineEditATickSize = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditATickSize,3,1);
   lineEditATickSize->setText(QString::number(_axisTickSize));
   QLabel * labelAFontSize = new QLabel("Axis font size");
   gridLayout->addWidget(labelAFontSize,4,0);
   QLineEdit * lineEditAFontSize = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditAFontSize,4,1);
   lineEditAFontSize->setText(QString::number(_axisFontSize));
  
   QLabel * labelAxisNumb = new QLabel("Number of axis ticks");
   gridLayout->addWidget(labelAxisNumb,5,0);
   QLineEdit * lineEditAxisNumb = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditAxisNumb,5,1);
   lineEditAxisNumb->setText(QString::number(_axisNumbTicks));



   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   gridLayout->addWidget(&buttonBox, 6, 0);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   // Show the dialog as modal
   if (dialog.exec() == QDialog::Accepted) {
      _tickSize=lineEditTickSize->text().toInt(); 
      _fontSize=lineEditFontSize->text().toInt();
      _axisTickSize=lineEditATickSize->text().toInt(); 
      _axisFontSize=lineEditAFontSize->text().toInt();
      _axisNumbTicks=lineEditAxisNumb->text().toInt();
      _snap = lineEditSnap->text().toInt();
      repaint();
   }
}

void ProfilePicture::showDepth()
{
   _showDepth = !_showDepth;
   repaint();
}

void ProfilePicture::exportToDxf()
{
   bool bdummy = true;
   QString fileN = QFileDialog::getSaveFileName(this,
         tr("Save dxf"),".",tr("DXF Files (*.dxf)"));
   
   if(!fileN.endsWith(".dxf"))
      fileN.append(".dxf");
   
   QFile file(fileN);
   file.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream in(&file);

   in<<" 0\nSECTION\n  2\nENTITIES\n  0\n";
  
   double rho=0.0;
   int iprofile = _handler->activeProfile();
   for(int i=0;i<_handler->nProfilePoints(iprofile); i++)
   {
      int idrill = _handler->profilePoint(iprofile, i);
      if(i!=0)
      {
         int first = _handler->profilePoint(iprofile, i-1);
         rho+=sqrt(pow(_handler->drillX(first)-_handler->drillX(idrill), 2) + pow(_handler->drillY(first)-_handler->drillY(idrill), 2) );
      }
      double x=rho;
      double y0=_handler->elev(idrill);
      double y1=_handler->elev(idrill)-_handler->depth(idrill);
     
      DXF::writeLineDxf(x, y0, x, y1, "drillsLayer", in);
      DXF::writePointDxf(x, y0, "drillsLayer", in);
      DXF::writeTextDxf(x, y0, "textLayer", in, 0, 0, 
            _viewHandler->drillName(idrill));
      DXF::writePointDxf(x, y1, "drillsLayer", in);

      for(int j=0;j<_viewHandler->nSamples(idrill); j++)
      {
         double sy0=_handler->elev(idrill) - 
            _viewHandler->sampleVal(idrill, j, 0);
         double sy1=_handler->elev(idrill) -
            _viewHandler->sampleVal(idrill, j, 1);
         DXF::writePointDxf(x, sy0, "samplesLayer", in);
         DXF::writePointDxf(x, sy1, "samplesLayer", in);
         DXF::writeTextDxf(x, (sy0+sy1)/2, "textLayer", in, 0, 2, 
               Calculation::calcValue(idrill, j, _handler, 
                  _handler->typeCalc(), _handler->fromProfile(), 
                  _handler->toProfile(), bdummy));
         if(_showDepth)
         {
            double yy0=_handler->elev(idrill) - 
                  _viewHandler->sampleVal(idrill, j, 0);
            double yy1=_handler->elev(idrill) - 
               _viewHandler->sampleVal(idrill, j, 1);
            DXF::writeTextDxf(x, sy0, "textLayer", in, 2, 2, QString::number(yy0));
            DXF::writeTextDxf(x, sy1, "textLayer", in, 2, 2, QString::number(yy1));
         }
      }
   }

   for(int i=0; i<_handler->nSurfaces(); i++)
   {
      if(_surfHandler->isCheckedSurf(i))
      {
         std::vector<QPointF> points;
         for(int j=0;j<_handler->nSurfPoints(i);j++)
         {
            if(!_viewHandler->isCheckedDrillProfile(
                     _handler->surfPointDrill(i,j)))
               continue;
            double x=Calculation::distanceFromFirst(
                  _handler->surfPointDrill(i,j), _handler);

            DXF::writePointDxf(x, _handler->surfPointZ(i,j), "surfacePointLayer", in);

            QPointF center (x, _handler->surfPointZ(i,j));
            points.push_back(center);
         }

         std::sort(points.begin(), points.end(), pointComparison);

         if(points.size()>0)
         {
            for(size_t k=1;k<points.size();k++)
            {
               DXF::writeLineDxf(points[k-1].x(), points[k-1].y(), 
                     points[k].x(), points[k].y(), "surfaceLineLayer", in);

               QPointF mid = (points[k]+points[k-1])/2;
               DXF::writeTextDxf(mid.x(), mid.y(), "textLayer", in, 0, 0, _handler->surfName(i));
            }
         }
      }


   }

   in<<"ENDSEC\n  0\nEOF";
   file.close();
}

void ProfilePicture::savePic()
{
   QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save image"),".",tr("JPG Files (*.jpg)"));

   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");

   _image->save(fileName);
}

void ProfilePicture::repaintWrapper(const int &)
{
   repaint();
}

void ProfilePicture::rescaleAndRepaintWrapper(const int &)
{
   rescaleAndRepaint();
}

void ProfilePicture::startSurface()
{
   _surfaceAdd = true;
   _selectedSurf = _handler->nSurfaces();
   _surfHandler->addSurface();
}

void ProfilePicture::addSurfacePoint()
{//this is not good solution FIXME
   //_drillCount =
   _additionalSurfPoints = true;
   if(findDrillCount(0))
      _surfaceAdd=true;

   int activeProfile = _handler->activeProfile();
   for(int i=0; i<_handler->nProfilePoints(activeProfile); i++)
   {
      int idrill = _handler->profilePoint(activeProfile, i);
      if(!_handler->isDrillInSurface(_selectedSurf, idrill))
      {
         _drillCount = i;
         break;
      }
   }
}

void ProfilePicture::selectedChange(const int & id)
{
   _selectedSurf = id;
   repaint();
}

void ProfilePicture::paintEvent(QPaintEvent * event)
{
    QSize newSize(width(),height());
    delete _image;
    _image=new QImage(newSize, QImage::Format_RGB32);
    _image->fill(qRgb(255, 255, 255));
    QPainter painter(_image);    
    paint(&painter, event);
}

void ProfilePicture::drawText(QPainter * painter, qreal x, qreal y, 
      Qt::Alignment flags, const QString & text, QRectF * boundingRect) const
{
   const qreal size = 32767.0;
   QPointF corner(x, y - size);
   if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
   else if (flags & Qt::AlignRight) corner.rx() -= size;
   if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
   else if (flags & Qt::AlignTop) corner.ry() += size;
   else flags |= Qt::AlignBottom;
   QRectF rect{corner.x(), corner.y(), size, size};
   painter->drawText(rect, flags, text, boundingRect);
}

double ProfilePicture::getTickStep(const double up, const double down) const
{
  double exactStep = (up-down)/(double)(_axisNumbTicks+1e-10); // _axisNumbTicks ticks on average, the small addition is to prevent jitter on exact integers
  return cleanMantissa(exactStep);
}

double ProfilePicture::cleanMantissa(double input) const
{
  double magnitude;
  const double mantissa = getMantissa(input, &magnitude);
  return pickClosest(mantissa, QVector<double>() << 1.0 << 2.0 << 2.5 << 5.0 << 10.0)*magnitude;
    /*case tssMeetTickCount:
    {
      // this gives effectively a mantissa of 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 8.0, 10.0
      if (mantissa <= 5.0)
        return (int)(mantissa*2)/2.0*magnitude; // round digit after decimal point to 0.5
      else
        return (int)(mantissa/2.0)*2.0*magnitude; // round to first digit in multiples of 2
    }*/
  return input;
}

double ProfilePicture::getMantissa(double input, double *magnitude) const
{
  const double mag = pow(10.0, floor(log(fabs(input))/log(10.0)));
  if (magnitude) *magnitude = mag;
  return input/mag;
}

double ProfilePicture::pickClosest(double target, const QVector<double> &candidates) const
{
  if (candidates.size() == 1)
    return candidates.first();
  QVector<double>::const_iterator it = std::lower_bound(candidates.constBegin(), candidates.constEnd(), target);
  if (it == candidates.constEnd())
    return *(it-1);
  else if (it == candidates.constBegin())
    return *it;
  else
    return target-*(it-1) < *it-target ? *(it-1) : *it;
}

QVector<double> ProfilePicture::createTickVector(const double & tickStep, const double & up, const double & down, const double & origin) const
{
  QVector<double> result;
  // Generate tick positions according to tickStep:
  int firstStep = ceil((down-origin)/tickStep); // do not use qFloor here, or we'll lose 64 bit precision
  int lastStep = floor((up-origin)/tickStep); // do not use qCeil here, or we'll lose 64 bit precision
  int tickcount = lastStep-firstStep+1;
  if (tickcount < 0) tickcount = 0;
  result.resize(tickcount);
  for (int i=0; i<tickcount; ++i)
    result[i] = origin + (firstStep+i)*tickStep;
  return result;
}

bool ProfilePicture::findDrillCount(const int & start)
{
   int activeProfile = _handler->activeProfile();
   for(int i=start; i<_handler->nProfilePoints(activeProfile); i++)
   {
      int idrill = _handler->profilePoint(activeProfile, i);
      if(!_handler->isDrillInSurface(_selectedSurf, idrill))
      {
         _drillCount = i;
         return true;
         //break;         
      }
   }
   return false;
}
