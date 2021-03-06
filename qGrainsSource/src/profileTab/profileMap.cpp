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
#include "profileMap.h"
#include "model/handlers/viewHandler.h"
#include "qGrains.h"
#include "centralWindow.h"
#include "model/handlers/handler.h"
#include "model/handlers/profileHandler.h"
#include "model/modelNodeBase.h"
#include <QDebug>
#include <QPainter>
#include <QMenu>
#include <QFileDialog>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPaintEvent>
#include <QPushButton>
#include <QStatusBar>
#include <cmath>

ProfileMap::ProfileMap(Handler * handler, ViewHandler * viewHandler, ProfileHandler * profileHandler, QGrains * qGrains):_viewHandler(viewHandler), _qGrains(qGrains), _handler(handler), _profileHandler(profileHandler)
{
   QSize newSize(width(),height());
   _image=new QImage(newSize, QImage::Format_RGB32);

   setMouseTracking(true);
   setBackgroundRole(QPalette::Base);
   setAutoFillBackground(true);
   
   setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));

   connect(_viewHandler, SIGNAL(drillProfSelectionChange(const int &)), this, 
         SLOT(repaintWrapper(const int &)));

   connect(_profileHandler, SIGNAL(selectedInTable(const int &)), 
         this, SLOT(repaintWrapper(const int &)));

   connect(_qGrains->model(), SIGNAL(loadFinished()), this, SLOT(loadMap()));
   connect(_qGrains->model(), SIGNAL(imageLoad()), this, SLOT(loadMap()));
}

QSize ProfileMap::minimumSizeHint() const
{
    return QSize(150,150);
}

void ProfileMap::mouseDoubleClickEvent(QMouseEvent * event)
{
   for(int i=0;i<_viewHandler->nDrills();i++)
   {
      double xc=transformX(_handler->drillX(i));
      double yc=transformY(_handler->drillY(i));
      if(fabs(event->pos().x()-xc)<_drillRadius && 
            fabs(event->pos().y()-yc)<_drillRadius)
      {
         _viewHandler->changeCheckProfile(i);
         break;
      }
   } 
}

void ProfileMap::mouseMoveEvent(QMouseEvent * event)
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

void ProfileMap::mousePressEvent(QMouseEvent * event)
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

void ProfileMap::mouseReleaseEvent(QMouseEvent * event)
{
   if (event->button() == Qt::LeftButton)
   {
      if(!_distanceT)
         this->setCursor(Qt::ArrowCursor);
      _drag=false;
   }
}

void ProfileMap::wheelEvent(QWheelEvent * event)
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

double ProfileMap::transformX(double x) const
{
   return _transform_a*x+_transform_b;
}

double ProfileMap::transformY(double y) const
{
   return _transform_c*y+_transform_d;
}

double ProfileMap::inverseTransformX(double x) const
{
   return (1/_transform_a)*x-_transform_b/_transform_a;
}

double ProfileMap::inverseTransformY(double y) const
{
   return (1/_transform_c)*y-_transform_d/_transform_c;
}

void ProfileMap::setTransform(const double g, const double h)
{
   _transform_a=g/(_handler->xmax()-_handler->xmin());
   _transform_b=-_transform_a*_handler->xmin();
   _transform_c=-h/(_handler->ymax()-_handler->ymin());
   _transform_d=h-_transform_c*_handler->ymin();
}

void ProfileMap::loadImage(const QString & imageName)
{
   _pixmap.load(imageName);
}

double ProfileMap::distance(const double x1, const double y1, 
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

void ProfileMap::paint(QPainter *painter, QPaintEvent * event)
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

    setGeometry(5,frameGeometry().topLeft().y(),sirina,visina);

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

    for(int i=0;i<_viewHandler->nDrills();i++)
    {
       double xc=transformX(_handler->drillX(i));
       double yc=transformY(_handler->drillY(i));
       QPointF center(xc, yc);
       if(_viewHandler->isCheckedDrillProfile(i))
          pen.setColor(Qt::GlobalColor(Qt::red));
       else
          pen.setColor(Qt::GlobalColor(Qt::blue));
       painter->setPen(pen);
       painter->drawEllipse(center, _drillRadius, _drillRadius);
       painter->drawText(xc+_drillXOffset, yc+_drillYOffset, _viewHandler->drillName(i));
    }
    
    int activeProfile = _handler->activeProfile();
    for(int iprofile=0; iprofile<_handler->nProfiles();iprofile++)
    {
       if(iprofile==activeProfile)
          continue;

       pen.setColor(Qt::GlobalColor(Qt::black));
       painter->setPen(pen);
       for(int i=0; i<_handler->nProfilePoints(iprofile)-1; i++)
       {
          int first = _handler->profilePoint(iprofile, i);
          int second = _handler->profilePoint(iprofile, i+1);
          QPointF p1 = QPointF(transformX(_handler->drillX(first)), transformY(_handler->drillY(first)));
          QPointF p2 = QPointF(transformX(_handler->drillX(second)), transformY(_handler->drillY(second)));
          painter->drawLine(p1, p2);
          painter->drawText((p1+p2)/2, _handler->profileName(iprofile));
       }
    }
    
    pen.setColor(Qt::GlobalColor(Qt::red));
    painter->setPen(pen);
    for(int i=0; i<_handler->nProfilePoints(activeProfile)-1; i++)
    {
       int first = _handler->profilePoint(activeProfile, i);
       int second = _handler->profilePoint(activeProfile, i+1);
       QPointF p1 = QPointF(transformX(_handler->drillX(first)), transformY(_handler->drillY(first)));
       QPointF p2 = QPointF(transformX(_handler->drillX(second)), transformY(_handler->drillY(second)));
       painter->drawLine(p1, p2);
       painter->drawText((p1+p2)/2, _handler->profileName(activeProfile));
    }

    
    painter->setPen(palette().dark().color());
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRect(0, 0, width() - 1, height() - 1));
    
    QPainter painterW(this);
    QRect dirtyRect = event->rect();
    painterW.drawImage(dirtyRect, *_image, dirtyRect);  
    
}

void ProfileMap::repaint()
{
   update(0,0,width(),height());
}

void ProfileMap::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Image properties", this, SLOT(imageProp()));
   menu->addAction("Save image", this, SLOT(savePic()));
   
   menu->addAction("Objects properties", this, SLOT(setObjectProp()));
   menu->addAction("Set range",this, SLOT(setRange()));
   menu->addAction("Distance tool", this, SLOT(distanceTool()));

   menu->popup(mapToGlobal(pos));
}

void ProfileMap::imageProp()
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

void ProfileMap::savePic()
{
   QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save image"),".",tr("JPG Files (*.jpg)"));

   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");

   _image->save(fileName);
}

void ProfileMap::setObjectProp()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Object properties");
   QGridLayout * gridLayout = new QGridLayout(&dialog);

   QLabel * labelWellXOffset = new QLabel("Name x offset");
   gridLayout->addWidget(labelWellXOffset,0,0);
   QLineEdit * lineEditWellXOffset = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditWellXOffset,0,1);
   lineEditWellXOffset->setText(QString::number(_drillXOffset));
   QLabel * labelWellYOffset = new QLabel("Name y offset");
   gridLayout->addWidget(labelWellYOffset,1,0);
   QLineEdit * lineEditWellYOffset = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditWellYOffset,1,1);
   lineEditWellYOffset->setText(QString::number(-_drillYOffset));
   QLabel * labelWellRadius = new QLabel("Radius");
   gridLayout->addWidget(labelWellRadius,2,0);
   QLineEdit * lineEditWellRadius = new QLineEdit(&dialog);
   gridLayout->addWidget(lineEditWellRadius,2,1);
   lineEditWellRadius->setText(QString::number(_drillRadius));



   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   gridLayout->addWidget(&buttonBox, 3,0);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   // Show the dialog as modal
   if (dialog.exec() == QDialog::Accepted) {
      _drillXOffset=lineEditWellXOffset->text().toInt(); 
      _drillYOffset=-lineEditWellYOffset->text().toInt();
      _drillRadius=lineEditWellRadius->text().toInt();
   }
}

void ProfileMap::setRange()
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

void ProfileMap::distanceTool()
{
   _distanceT=true;
   this->setCursor(Qt::CrossCursor);
}

void ProfileMap::browse(/*QString & fileName*/)
{
   QString fileName = QFileDialog::getOpenFileName();
   if(!fileName.isEmpty())
   {
      _handler->setPictureName(fileName);
      _lineEdit->setText(_handler->pictureName());
   } 
}

void ProfileMap::repaintWrapper(const int &)
{
   repaint();
}

void ProfileMap::loadMap()
{
   loadImage(_handler->pictureName());
}

void ProfileMap::paintEvent(QPaintEvent * event)
{
    QSize newSize(width(),height());
    delete _image;
    _image=new QImage(newSize, QImage::Format_RGB32);
    _image->fill(qRgb(255, 255, 255));
    QPainter painter(_image);    
    paint(&painter, event);
}

