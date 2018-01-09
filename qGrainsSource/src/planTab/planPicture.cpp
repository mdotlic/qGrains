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
#include "planPicture.h"
#include "model/handlers/handler.h"
#include "model/modelNodeBase.h"
#include "qGrains.h"
#include "planChooser.h"
#include "isolinesWidget.h"
#include "calculation/calculation.h"
#include "dxf/dxf.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QLineEdit>
#include <QFileDialog>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QStatusBar>
#include <QMenu>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>
#include <cmath>

PlanPicture::PlanPicture(Handler * handler, QGrains * qGrains, 
      PlanChooser * planChooser, IsolinesWidget * isolinesWidget):_handler(handler), _qGrains(qGrains), _isolinesWidget(isolinesWidget)
{
   QSize newSize(width(),height());
   _image=new QImage(newSize, QImage::Format_RGB32);

   setMouseTracking(true);
   setBackgroundRole(QPalette::Base);
   setAutoFillBackground(true);

   setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));

   connect(planChooser, SIGNAL(changePicture()), this, SLOT(calcPicture()));
   connect(planChooser, SIGNAL(changeSurfPicture()), this, 
         SLOT(calcSurfPicture()));
   connect(planChooser, SIGNAL(changeSurfThicknessPicture()), this, 
         SLOT(calcSurfThicknessPicture()));
   connect(_isolinesWidget, SIGNAL(repaintPict()), this, SLOT(repaint()));

   connect(_qGrains->model(), SIGNAL(loadFinished()), this, SLOT(loadMap()));
   connect(_qGrains->model(), SIGNAL(imageLoad()), this, SLOT(loadMap()));
}

QSize PlanPicture::minimumSizeHint() const
{
   return QSize(150,150);
}

void PlanPicture::mouseMoveEvent(QMouseEvent * event)
{
   //qDebug()<<" imamo move event "<<event->pos().x()<<" "<<event->pos().y()<<"\n";
   //setMouseTracking(false);
   if(_drag)
   {
      double moveX=inverseTransformX(event->pos().x())-inverseTransformX(_lastDragPos.x());
      double moveY=inverseTransformY(event->pos().y())-inverseTransformY(_lastDragPos.y());
      _handler->setXmin(_handler->xmin()-moveX);
      _handler->setXmax(_handler->xmax()-moveX);
      _handler->setYmin(_handler->ymin()-moveY);
      _handler->setYmax(_handler->ymax()-moveY);
      repaint();
      _lastDragPos=event->pos();
   }
   QString str;
   if(_paintDistance)
   {
      double x0=inverseTransformX(_distanceOrigin.x());
      double y0=inverseTransformY(_distanceOrigin.y());
      double x1=inverseTransformX(event->pos().x());
      double y1=inverseTransformY(event->pos().y());
      double dist=sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
      _distanceEnd=event->pos();
      repaint();
      str="Distance from ("+QString::number(x0, 'f', 2)+";"+QString::number(y0, 'f', 2)+")"+" to ("+QString::number(x1, 'f', 2)+";"+QString::number(y1, 'f', 2)+")   is   "+QString::number(dist, 'f', 2);
   }else
      str="Map ("+QString::number(inverseTransformX(event->pos().x()), 'f', 2)+";"+QString::number(inverseTransformY(event->pos().y()), 'f', 2)+")";
   _qGrains->statusBar()->showMessage(str);
}

void PlanPicture::mousePressEvent(QMouseEvent * event)
{
   if (event->button() == Qt::LeftButton)
   {
      if(_paintDistance)
      {
         _distanceT=false;
         _paintDistance=false;
         this->setCursor(Qt::ArrowCursor);
         repaint();
      }else if(_distanceT)
      {
         _paintDistance=true;
         _distanceOrigin = event->pos();
      }else{
         this->setCursor(Qt::ClosedHandCursor);
        _lastDragPos = event->pos();
        _drag=true;
      }
   }
}

void PlanPicture::mouseReleaseEvent(QMouseEvent * event)
{
   if (event->button() == Qt::LeftButton)
   {
      if(!_distanceT)
         this->setCursor(Qt::ArrowCursor);
      _drag=false;
   }
}

void PlanPicture::mouseDoubleClickEvent(QMouseEvent *)
{
   _isolinesWidget->setXmin(_handler->xmin());
   _isolinesWidget->setXmax(_handler->xmax());
   _isolinesWidget->setYmin(_handler->ymin());
   _isolinesWidget->setYmax(_handler->ymax());
}

void PlanPicture::wheelEvent(QWheelEvent * event)
{

   double zoomInFactor=0.95;
   int numDegrees = event->delta() / 8;
    double numSteps = numDegrees / 15.0f;
    double zoom=pow(zoomInFactor, numSteps);
    double invX=inverseTransformX(event->pos().x());
    double xdiff=invX-_handler->xmin();
    _handler->setXmin(invX-xdiff*zoom);
    double xMaxDiff=invX-_handler->xmax();
    _handler->setXmax(invX-xMaxDiff*zoom);
    
    double invY=inverseTransformY(event->pos().y());
    double ydiff=invY-_handler->ymin();
    _handler->setYmin(invY-ydiff*zoom);
    double yMaxDiff=invY-_handler->ymax();
    _handler->setYmax(invY-yMaxDiff*zoom);

   repaint();
}

double PlanPicture::transformX(double x) const
{
   return _transform_a*x+_transform_b;
}

double PlanPicture::transformY(double y) const
{
   return _transform_c*y+_transform_d;
}

double PlanPicture::inverseTransformX(double x) const
{
   return (1/_transform_a)*x-_transform_b/_transform_a;
}

double PlanPicture::inverseTransformY(double y) const
{
   return (1/_transform_c)*y-_transform_d/_transform_c;
}

void PlanPicture::setTransform(const double g, const double h)
{
   _transform_a=g/(_handler->xmax()-_handler->xmin());
   _transform_b=-_transform_a*_handler->xmin();
   _transform_c=-h/(_handler->ymax()-_handler->ymin());
   _transform_d=h-_transform_c*_handler->ymin();
}

void PlanPicture::loadImage(const QString & imageName)
{
   _pixmap.load(imageName);
}

double PlanPicture::distance(const double x1, const double y1, 
      const double x2, const double y2, const double refX, const double refY) const
{
   //p = -b*x + a*y + c1 line x1x2
   //q = a*x + b*y + c2 line perpendicular to x1x2
   double a = x2-x1;
   double b = y2-y1;
   double c1 = x1*y2 - y1*x2;
   double c2 = -a*refX -b*refY;
   double x = (-c2*a+c1*b)/(a*a+b*b);
   double y = (-c1*a-c2*b)/(a*a+b*b);
   double divisor = sqrt(a*a+b*b);
   if(fabs(divisor)<1e-12)
   {
      QString err("Point 1 and Point two are same.");
      _qGrains->modalWarning(_qGrains, err);
      //izbaci gresku
      return std::numeric_limits<double>::max();
   }
   double distance = fabs((x2-x1)*(y1-refY) - (x1-refX)*(y2-y1))/divisor;
   if(fabs(refX-x)<1e-12)
      return (refY>y) ? distance : -distance;
      
   return (refX>x)? -distance : distance;
}

void PlanPicture::paint(QPainter *painter, QPaintEvent * event)
{//this function is used to paint everything
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
   
    //QPen pen(QPen(Qt::red));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    painter->setPen(pen);

    double sirina;
    double visina;
    if(ceil(width()/_handler->ratio())<height())
    {
       sirina=width();
       visina=ceil(sirina/_handler->ratio());
    }else{
       visina=height();
       sirina=floor(visina*_handler->ratio());
    }

    setTransform(sirina, visina);
    painter->drawPixmap(transformX(_handler->picXmin()),
        transformY(_handler->picYmax()), 
        transformX(_handler->picXmax())-transformX(_handler->picXmin()),
        transformY(_handler->picYmin())-transformY(_handler->picYmax()),
        _pixmap);

    setGeometry(frameGeometry().topLeft().x(),frameGeometry().topLeft().y(),sirina,visina);

    drawText(painter, width()/2, 10 , Qt:: AlignTop | Qt::AlignHCenter,
          _titleName);

    if(_paintDistance)
    {
       painter->setPen(QPen(Qt::black,1,Qt::DashLine));
       painter->drawLine(_distanceOrigin, _distanceEnd);
       double x0=inverseTransformX(_distanceOrigin.x());
       double y0=inverseTransformY(_distanceOrigin.y());
       double x1=inverseTransformX(_distanceEnd.x());
       double y1=inverseTransformY(_distanceEnd.y());
       double dist=sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
       painter->drawText((_distanceOrigin.x()+_distanceEnd.x())/2,
             (_distanceOrigin.y()+_distanceEnd.y())/2,
             QString::number(dist, 'f', 2));
    }

    painter->setPen(pen);

    //paints scale    
    double razmera = powf(10.0f, floorf(log10f(inverseTransformX(90)-inverseTransformX(0))));
    QString unitString;
    if(razmera>1000)
    {
       unitString=QString::number(razmera/1000)+"km";
    }else if(razmera<0.01)
    {
       unitString=QString::number(razmera*100)+"cm";
    }else
       unitString=QString::number(razmera)+"m";

    double trans = transformX(razmera)-transformX(0);
    painter->drawLine(sirina-65-trans, visina-10, sirina-65, visina-10);
    painter->drawLine(sirina-65-trans, visina-8, sirina-65-trans, visina-12);
    painter->drawLine(sirina-65, visina-8, sirina-65, visina-12);
    painter->drawText(sirina-62, visina-10, unitString);


    pen.setColor(Qt::GlobalColor(Qt::blue));
    painter->setPen(pen);

    for(int i=0;i<_handler->nDrills();i++)
    {
       double xc=transformX(_handler->drillX(i));
       double yc=transformY(_handler->drillY(i));
       QPointF center(xc, yc);
       pen.setColor(Qt::GlobalColor(Qt::black));
       if(_isolinesWidget->drillsColorExist())
       {
          pen.setColor(Qt::GlobalColor(_isolinesWidget->drillColor(i)));
       }
       painter->setPen(pen);
       painter->drawEllipse(center, _drillRadius, _drillRadius);
       drawText(painter, xc, yc, Qt:: AlignBottom | Qt::AlignHCenter, _handler->drillName(i));
       if(_isolinesWidget->drillsColorExist() && 
             _isolinesWidget->drillColor(i)!=Qt::black && _showDrillNumber)
       {
          drawText(painter, xc, yc, Qt:: AlignTop | Qt::AlignHCenter, QString::number(_isolinesWidget->drillNumber(i), 'g', 3));
       }
    }
    
    pen.setColor(Qt::GlobalColor(_color[_isoColor]));
    painter->setPen(pen);

    int niso = _isolinesWidget->isoSize();
    for(int isol=0;isol<niso;isol++)
    {
       int nlines=_isolinesWidget->isoLinesSize(isol);
       for(int iline=0;iline<nlines;iline++)
       {
          QPointF p1 = _isolinesWidget->line1P(isol, iline);
          QPointF p2 = _isolinesWidget->line2P(isol, iline);

          painter->drawLine(transformX(p1.x()), transformY(p1.y()),
                transformX(p2.x()), transformY(p2.y()));
          if(iline%_isoLabelNumber==0)
             painter->drawText(transformX(p1.x())+2, transformY(p1.y())+2,
                   QString::number(_handler->isoValue(isol), 'g', 3));

       }
    }


    painter->setPen(palette().dark().color());
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRect(0, 0, width() - 1, height() - 1));
    
    QPainter painterW(this);
    QRect dirtyRect = event->rect();
    painterW.drawImage(dirtyRect, *_image, dirtyRect);
}

/*
double PlanPicture::drillNumber(const int & idrill)
{
   if(_drillColor.size()==0)
      return std::numeric_limits<size_t>::max();
   else if(_drillColor[idrill]==Qt::black)
      return std::numeric_limits<size_t>::max();
   return _drillNumber[idrill]
}*/

void PlanPicture::repaint()
{
   update(0,0,width(),height());
}

void PlanPicture::calcPicture()
{
   _isolinesWidget->resizeDrillsColorNumber();   
   if(_handler->planFrom() > _handler->planTo())
      return;
   bool bdummy = true;
   double fromM = _handler->planFrom();
   double toM = _handler->planTo();
   for(int i=0;i<_handler->nDrills();i++)
   {
      double from = fromM;
      double to = toM;
      if(_handler->planType()==0)
      {
         from = _handler->elev(i) - toM;
         to = _handler->elev(i) - fromM;
      }
      
      int count = 0;
      double dn = 0;
      for(int isample=0; isample<_handler->nSample(i); isample++)
      {
         if(_handler->sampleVal(i, isample, 1)>=from && 
               _handler->sampleVal(i, isample, 0)<=to)
         {
            count++;
            dn =+ Calculation::calcValue(i, isample,
                  _handler, _handler->planCalcType(), _handler->planFromSize(), 
                  _handler->planToSize(), bdummy).toDouble();
         }
      }
      if(count==1)
         _isolinesWidget->setDrillColor(i, Qt::darkGreen);
      else if(count>1)
      {
         _isolinesWidget->setDrillColor(i, Qt::red);
         dn = dn/count;
      }
      _isolinesWidget->setDrillNumber(i, dn);
   }
   emit _isolinesWidget->somethingChanged();
   _isolinesWidget->clearIsoList();
   setFullTitle();
   repaint();
}

void PlanPicture::calcSurfPicture()
{
   _isolinesWidget->resizeDrillsColorNumber();
   int isurf = _handler->planSurface();
   if(isurf >= _handler->nSurfaces())
      return;
   for(int ipoint = 0; ipoint < _handler->nSurfPoints(isurf); ipoint++)
   {
      int idrill = _handler->surfPointDrill(isurf, ipoint);
      double surfZ = _handler->surfPointZ(isurf, ipoint);
      _isolinesWidget->setDrillColor(idrill, Qt::darkGreen);
      _isolinesWidget->setDrillNumber(idrill, surfZ);
   }
   emit _isolinesWidget->somethingChanged();
   _isolinesWidget->clearIsoList();
   setFullTitle();
   repaint();   
}

void PlanPicture::calcSurfThicknessPicture()
{
   _isolinesWidget->resizeDrillsColorNumber();
   int fsurf = _handler->planSurfaceFrom();
   int tsurf = _handler->planSurfaceTo();

   if(tsurf >= _handler->nSurfaces() || fsurf >= _handler->nSurfaces())
      return;
   
   std::vector<double> fromZ(_handler->nDrills(), std::numeric_limits<size_t>::max());
   std::vector<double> toZ(_handler->nDrills(), std::numeric_limits<size_t>::max());
   for(int ipoint = 0; ipoint < _handler->nSurfPoints(fsurf); ipoint++)
   {
      int idrill = _handler->surfPointDrill(fsurf, ipoint);
      fromZ[idrill] = _handler->surfPointZ(fsurf, ipoint);
   }
   for(int ipoint = 0; ipoint < _handler->nSurfPoints(tsurf); ipoint++)
   {
      int idrill = _handler->surfPointDrill(tsurf, ipoint);
      toZ[idrill] = _handler->surfPointZ(tsurf, ipoint);
   }
   for(int idrill=0; idrill<_handler->nDrills(); idrill++)
   {
      if(fromZ[idrill] != std::numeric_limits<size_t>::max() && 
            toZ[idrill] != std::numeric_limits<size_t>::max())
      {
         _isolinesWidget->setDrillNumber(idrill, fromZ[idrill] - toZ[idrill]);
         _isolinesWidget->setDrillColor(idrill, Qt::darkGreen);
      }
   }
   emit _isolinesWidget->somethingChanged();
   _isolinesWidget->clearIsoList();
   setFullTitle();
   repaint();
}

void PlanPicture::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Image properties", this, SLOT(imageProp()));
   menu->addAction("Save image", this, SLOT(savePic()));
   menu->addAction("Export to dxf", this, SLOT(exportToDxf()));
   
   menu->addAction("Objects properties", this, SLOT(setObjectProp()));
   menu->addAction("Set range",this, SLOT(setRange()));
   menu->addAction("Distance tool", this, SLOT(distanceTool()));
   if(_showDrillNumber)
      menu->addAction("Hide drill value", this, SLOT(showHideDrillNumber()));
   else
      menu->addAction("Show drill value", this, SLOT(showHideDrillNumber()));
   menu->addAction("Set title", this, SLOT(setTitle()));

   menu->popup(mapToGlobal(pos));
}

void PlanPicture::imageProp()
{
   QDialog * dialog = new QDialog(this);
   dialog->setWindowTitle("Image properties");
   QVBoxLayout * dialogLayout = new QVBoxLayout(dialog);

   QHBoxLayout * inputLayout = new QHBoxLayout;

   QLabel * label = new QLabel("Image file ");
   _lineEdit = new QLineEdit;
   _lineEdit->setText(_handler->pictureName());
   inputLayout->addWidget(label);
   inputLayout->addWidget(_lineEdit);
   QPushButton * browseButton = new QPushButton(tr("Choose"));
   inputLayout->addWidget(browseButton);
   connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
   
   dialogLayout->addLayout(inputLayout);


   QFrame * limitFrame = new QFrame;
   QVBoxLayout * allLimitLayout = new QVBoxLayout;
   QLabel * labelLimits = new QLabel("Limits");
   allLimitLayout->addWidget(labelLimits);

   QGridLayout * limitLayout = new QGridLayout;

   QLabel * labelXmin = new QLabel("x min");
   QLineEdit * lineEditXmin = new QLineEdit;
   lineEditXmin->setText(QString::number(_handler->picXmin(), 'g',10));
   QLabel * labelXmax = new QLabel("x max");
   QLineEdit * lineEditXmax = new QLineEdit;
   lineEditXmax->setText(QString::number(_handler->picXmax(), 'g',10));
   QLabel * labelYmin = new QLabel("y min");
   QLineEdit * lineEditYmin = new QLineEdit;
   lineEditYmin->setText(QString::number(_handler->picYmin(), 'g',10));
   QLabel * labelYmax = new QLabel("y max");
   QLineEdit * lineEditYmax = new QLineEdit;
   lineEditYmax->setText(QString::number(_handler->picYmax(), 'g',10));
   limitLayout->addWidget(labelXmin, 0, 0);
   limitLayout->addWidget(lineEditXmin, 0, 1);
   limitLayout->addWidget(labelXmax, 0, 2);
   limitLayout->addWidget(lineEditXmax, 0, 3);
   limitLayout->addWidget(labelYmin, 1, 0);
   limitLayout->addWidget(lineEditYmin, 1, 1);
   limitLayout->addWidget(labelYmax, 1, 2);
   limitLayout->addWidget(lineEditYmax, 1, 3);
   //limitLayout->addWidget(limitFrame);
   allLimitLayout->addLayout(limitLayout);

   limitFrame->setLayout(allLimitLayout);
   limitFrame->setFrameShape(QFrame::StyledPanel);
   
   dialogLayout->addWidget(limitFrame);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   dialogLayout->addWidget(buttonBox);
   dialog->setLayout(dialogLayout);
   if(dialog->exec() == QDialog::Accepted)
   {
      _handler->setPicXmin(lineEditXmin->text().toDouble());
      _handler->setPicYmin(lineEditYmin->text().toDouble()); 
      _handler->setPicXmax(lineEditXmax->text().toDouble());
      _handler->setPicYmax(lineEditYmax->text().toDouble());
      _handler->setXmin(_handler->picXmin());
      _handler->setXmax(_handler->picXmax());
      _handler->setYmin(_handler->picYmin());
      _handler->setYmax(_handler->picYmax());
      _handler->setRatio();
      _handler->setPictureName(_lineEdit->text());
      emit _qGrains->model()->imageLoad();
   }
   delete _lineEdit;
   
}

void PlanPicture::savePic()
{
   QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save image"),".",tr("JPG Files (*.jpg)"));

   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");

   _image->save(fileName);
}

void PlanPicture::exportToDxf()
{
   QString fileN = QFileDialog::getSaveFileName(this,
         tr("Save dxf"),".",tr("DXF Files (*.dxf)"));
   
   if(!fileN.endsWith(".dxf"))
      fileN.append(".dxf");
   
   QFile file(fileN);
   file.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream in(&file);

   in<<" 0\nSECTION\n  2\nENTITIES\n  0\n";

   for(int i=0;i<_handler->nDrills();i++)
   {
      double xc=_handler->drillX(i);
      double yc=_handler->drillY(i);
      
      DXF::writePointDxf(xc, yc, "drillsLayer", in);
      DXF::writeTextDxf(xc, yc, "textLayer", in, 1, 1, _handler->drillName(i));
      if(_isolinesWidget->drillsColorExist() && 
            _isolinesWidget->drillColor(i)!=Qt::black && _showDrillNumber)
      {
         DXF::writeTextDxf(xc, yc, "textLayer", in, 1, 3, 
               QString::number(_isolinesWidget->drillNumber(i), 'g', 3));
      }
   }

   int niso = _isolinesWidget->isoSize();
   for(int isol=0;isol<niso;isol++)
   {
      int nlines=_isolinesWidget->isoLinesSize(isol);
      for(int iline=0;iline<nlines;iline++)
      {
         QPointF p1 = _isolinesWidget->line1P(isol, iline);
         QPointF p2 = _isolinesWidget->line2P(isol, iline);
         
         DXF::writeLineDxf(p1.x(), p1.y(), p2.x(), p2.y(), "isolineLayer", in);

         if(iline%_isoLabelNumber==0)
            DXF::writeTextDxf(p1.x(), p1.y(), "isolineNumberLayer", in, 0, 0,
                   QString::number(_handler->isoValue(isol), 'g', 3));

       }
    }
   
   in<<"ENDSEC\n  0\nEOF";
   file.close();
}

void PlanPicture::setObjectProp()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Object properties");
   QGridLayout * gridLayout = new QGridLayout(&dialog);

   QLabel * labelWellRadius = new QLabel("Radius");
   gridLayout->addWidget(labelWellRadius,1,0);
   QLineEdit * lineEditWellRadius = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditWellRadius,1,1);
   lineEditWellRadius->setText(QString::number(_drillRadius));

   QFrame * isoNumbFrame = new QFrame;
   QLabel * isoLabel = new QLabel("Isolines");
   QLabel * isoNumbLabel = new QLabel("Label incidence");
   QGridLayout * isoNumbLayout = new QGridLayout;
   isoNumbLayout->addWidget(isoLabel,0,0);
   isoNumbLayout->addWidget(isoNumbLabel,1,0);
   QLineEdit * isoNumbLineEdit = new QLineEdit(&dialog);
   isoNumbLayout->addWidget(isoNumbLineEdit,1,1);
   isoNumbLineEdit->setText(QString::number(_isoLabelNumber));
   QLabel * labelIsoColor = new QLabel("Color");
   isoNumbLayout->addWidget(labelIsoColor,2,0);
   QComboBox * isoColorCombo=addComboBoxIso(_isoColor);
   isoNumbLayout->addWidget(isoColorCombo,2,1);
   isoNumbFrame->setLayout(isoNumbLayout);
   isoNumbFrame->setFrameShape(QFrame::StyledPanel);
   isoNumbFrame->setFrameShape(QFrame::StyledPanel);
   gridLayout->addWidget(isoNumbFrame, 2, 0, 2, 2);

/*   QLabel * isoNumbLabel = new QLabel("Label incidence");
   gridLayout->addWidget(isoNumbLabel,3,0);
   QLineEdit * isoNumbLineEdit = new QLineEdit(&dialog);
   gridLayout->addWidget(isoNumbLineEdit,3,1);
   isoNumbLineEdit->setText(QString::number(_isoLabelNumber));*/


   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   gridLayout->addWidget(&buttonBox, 4,0);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   // Show the dialog as modal
   if (dialog.exec() == QDialog::Accepted) {
      _drillRadius=lineEditWellRadius->text().toInt();
      _isoLabelNumber=isoNumbLineEdit->text().toInt();
      _isoColor=isoColorCombo->currentIndex();
      repaint();
   }
}

void PlanPicture::setRange()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set range");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * xminLabel = new QLabel("Xmin:");
   layout->addWidget(xminLabel,0,0,1,1);
   QLineEdit * xminLineEdit = new QLineEdit(dialog);
   xminLineEdit->setText(QString::number(_handler->xmin(), 'g', 10));
   layout->addWidget(xminLineEdit,0,1,1,1);
   
   QLabel * xmaxLabel = new QLabel("Xmax:");
   layout->addWidget(xmaxLabel,0,2,1,1);
   QLineEdit * xmaxLineEdit = new QLineEdit(dialog);
   xmaxLineEdit->setText(QString::number(_handler->xmax(), 'g', 10));
   layout->addWidget(xmaxLineEdit,0,3,1,1);

   QLabel * yminLabel = new QLabel("Ymin:");
   layout->addWidget(yminLabel,1,0,1,1);
   QLineEdit * yminLineEdit = new QLineEdit(dialog);
   yminLineEdit->setText(QString::number(_handler->ymin(), 'g', 10));
   layout->addWidget(yminLineEdit,1,1,1,1);

   QLabel * ymaxLabel = new QLabel("Ymax:");
   layout->addWidget(ymaxLabel,1,2,1,1);
   QLineEdit * ymaxLineEdit = new QLineEdit(dialog);
   ymaxLineEdit->setText(QString::number(_handler->ymax(), 'g', 10));
   layout->addWidget(ymaxLineEdit,1,3,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      _handler->setXmin(xminLineEdit->text().toDouble());
      _handler->setXmax(xmaxLineEdit->text().toDouble());
      _handler->setYmin(yminLineEdit->text().toDouble());
      _handler->setYmax(ymaxLineEdit->text().toDouble());
      repaint();
   }
}

void PlanPicture::distanceTool()
{
   _distanceT=true;
   this->setCursor(Qt::CrossCursor);
}

void PlanPicture::browse(/*QString & fileName*/)
{
   QString fileName = QFileDialog::getOpenFileName();
   if(!fileName.isEmpty())
   {
      _handler->setPictureName(fileName);
      _lineEdit->setText(_handler->pictureName());
   } 
}

void PlanPicture::showHideDrillNumber()
{
   _showDrillNumber = !_showDrillNumber;
   repaint();
}

void PlanPicture::setTitle()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Set title");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);

   QFrame * captFrame = new QFrame;
   QGridLayout * captLayout = new QGridLayout;

   QLabel * captLabel = new QLabel("Text");
   captLayout->addWidget(captLabel,1,0);
   QPlainTextEdit * captText = new QPlainTextEdit(&dialog);
   captText->document()->setPlainText(_caption);
   captLayout->addWidget(captText,1,1);
   
   captFrame->setLayout(captLayout);
   captFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(captFrame);
   
   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;
   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);

   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   if (dialog.exec() == QDialog::Accepted) 
   {
      _caption = captText->document()->toPlainText();
      setFullTitle();
      repaint();
   }
}

void PlanPicture::loadMap()
{
   loadImage(_handler->pictureName());
}

void PlanPicture::paintEvent(QPaintEvent *event) 
{
   QSize newSize(width(),height());
   delete _image;
   _image=new QImage(newSize, QImage::Format_RGB32);
   _image->fill(qRgb(255, 255, 255));
   QPainter painter(_image);    
   paint(&painter, event);
}

void PlanPicture::drawText(QPainter * painter, qreal x, qreal y, 
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

QComboBox * PlanPicture::addComboBoxIso(int defColor)
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

void PlanPicture::setFullTitle()
{
   _titleName = _caption+"\n"+_names[_handler->planType()];
   if(_handler->planType()==0 || _handler->planType()==1)
   {
      _titleName.append(" from "+QString::number(_handler->planFrom(), 'f', 2)+" to "+QString::number(_handler->planTo(), 'f', 2));
      _titleName.append("\n"+_valNames[_handler->planCalcType()]);
      if(_handler->planCalcType()==18)
      {
         _titleName.append(" from "+QString::number(
                  _handler->planFromSize())+" to "+QString::number(
                  _handler->planToSize()));
      }
   }else if(_handler->planType()==2)
   {
      _titleName.append(_handler->surfName(_handler->planSurface()));
   }else if(_handler->planType()==3)
   {
      _titleName.append(" between "+_handler->surfName(_handler->planSurfaceFrom())+" and "+_handler->surfName(_handler->planSurfaceTo()));
   }
}
