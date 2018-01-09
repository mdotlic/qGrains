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
#include "isolinesWidget.h"
#include "planPicture.h"
#include "model/handlers/handler.h"
#include "calculation/extendedNN.hh"
#include "widgets/isoList.h"
#include "qGrains.h"
#include "model/modelNodeBase.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>

IsolinesWidget::IsolinesWidget(QGrains * qGrains, Handler * handler):_qGrains(qGrains), _handler(handler)
{
   QGridLayout * isoLayout = new QGridLayout;

   QLabel * xminLabel = new QLabel("Xmin");
   isoLayout->addWidget(xminLabel, 0, 0);
   _xmin = new QLineEdit;
   _xmin->setText(QString::number(_handler->isoXmin(), 'f', 2));
   isoLayout->addWidget(_xmin, 0, 1);

   QLabel * xmaxLabel = new QLabel("Xmax");
   isoLayout->addWidget(xmaxLabel, 0, 2);
   _xmax = new QLineEdit;
   _xmax->setText(QString::number(_handler->isoXmax(), 'f', 2));
   isoLayout->addWidget(_xmax, 0, 3);
   
   QLabel * yminLabel = new QLabel("Ymin");
   isoLayout->addWidget(yminLabel, 1, 0);
   _ymin = new QLineEdit;
   _ymin->setText(QString::number(_handler->isoYmin(), 'f', 2));
   isoLayout->addWidget(_ymin, 1, 1);
   
   QLabel * ymaxLabel = new QLabel("Ymax");
   isoLayout->addWidget(ymaxLabel, 1, 2);
   _ymax = new QLineEdit;
   _ymax->setText(QString::number(_handler->isoYmax(), 'f', 2));
   isoLayout->addWidget(_ymax, 1, 3);


   QLabel * divLabel = new QLabel("Division");
   isoLayout->addWidget(divLabel, 2, 0);
   QLabel * xLabel = new QLabel("X");
   isoLayout->addWidget(xLabel, 3, 0);
   _xdiv = new QLineEdit;
   _xdiv->setText(QString::number(_handler->isoXdiv()));
   isoLayout->addWidget(_xdiv, 3, 1);

   QLabel * yLabel = new QLabel("Y");
   isoLayout->addWidget(yLabel, 3, 2);
   _ydiv = new QLineEdit;
   _ydiv->setText(QString::number(_handler->isoYdiv()));
   isoLayout->addWidget(_ydiv, 3, 3);

   QVBoxLayout * allLayout = new QVBoxLayout;
   allLayout->addLayout(isoLayout);
   _calculateButton = new QPushButton(tr("Interpolate"));
   allLayout->addWidget(_calculateButton);

   QLabel * isoValue = new QLabel("Isoline values");
   allLayout->addWidget(isoValue);
   
   _isoVal = new QLineEdit;
   QPushButton * automatic = new QPushButton(tr("Automatic"));
   QHBoxLayout * horLayout = new QHBoxLayout;
   horLayout->addWidget(_isoVal);
   horLayout->addWidget(automatic);
   allLayout->addLayout(horLayout);
   _isoVal->setEnabled(false);
   _list = new IsoList;
   int niso=_handler->isoValuesSize();
   for(int j=0;j<niso;j++)
   {
      QListWidgetItem *item = new QListWidgetItem;
      item->setData(Qt::DisplayRole, _handler->isoValue(j)); 
      _list->addItem(item);
   }
   _list->setSortingEnabled(true);
   allLayout->addWidget(_list); 

   _showButton = new QPushButton(tr("Show"));
   _showButton->setEnabled(false);
   allLayout->addWidget(_showButton);
   
   allLayout->addStretch();

   setLayout(allLayout);

   setFrameShape(QFrame::StyledPanel);
   setFrameShadow(QFrame::Sunken);

   connect(_calculateButton, SIGNAL(clicked()), this, SLOT(calculate()));
   connect(_xmin, SIGNAL(editingFinished()), this, SLOT(changeXmin()));
   connect(_xmax, SIGNAL(editingFinished()), this, SLOT(changeXmax()));
   connect(_ymin, SIGNAL(editingFinished()), this, SLOT(changeYmin()));
   connect(_ymax, SIGNAL(editingFinished()), this, SLOT(changeYmax()));
   connect(_xdiv, SIGNAL(editingFinished()), this, SLOT(changeXdiv()));
   connect(_ydiv, SIGNAL(editingFinished()), this, SLOT(changeYdiv()));
   connect(_showButton, SIGNAL(clicked()), this, SLOT(calcMarchingSquares()));
   connect(_isoVal, SIGNAL(returnPressed()), this, SLOT(addToList()));
   connect(_list, SIGNAL(listChanged()), this, SLOT(listCh()));
   connect(_list, SIGNAL(elementDeleted(int)), this, SLOT(deleteFromList(int)));
   connect(automatic, SIGNAL(clicked()), this, SLOT(automaticIsoValues()));
   connect(this, SIGNAL(somethingChanged()), 
            this, SLOT(somethingChangedSlot()));
   connect(qGrains->model(), SIGNAL(loadFinished()), this, SLOT(loadFinishedSlot()));

}

void IsolinesWidget::setXmin(const double & val)
{
   _xmin->setText(QString::number(val, 'f', 2));
   _handler->setIsoXmin(val);
   _calculateButton->setEnabled(true);
}

void IsolinesWidget::setXmax(const double & val)
{
   _xmax->setText(QString::number(val, 'f', 2));
   _handler->setIsoXmax(val);
   _calculateButton->setEnabled(true);
}

void IsolinesWidget::setYmin(const double & val)
{
   _ymin->setText(QString::number(val, 'f', 2));
   _handler->setIsoYmin(val);
   _calculateButton->setEnabled(true);
}

void IsolinesWidget::setYmax(const double & val)
{
   _ymax->setText(QString::number(val, 'f', 2));
   _handler->setIsoYmax(val);
   _calculateButton->setEnabled(true);
}

void IsolinesWidget::setDrillColor(const int & idrill, const int & value)
{
   _drillColor[idrill]=value;
}

void IsolinesWidget::setDrillNumber(const int & idrill, const double & value)
{
   _drillNumber[idrill]=value;
}

int IsolinesWidget::drillColor(const int & idrill)
{
   return _drillColor[idrill];
}

double IsolinesWidget::drillNumber(const int & idrill)
{
   return _drillNumber[idrill];
}

void IsolinesWidget::resizeDrillsColorNumber()
{
   _drillColor.resize(_handler->nDrills(), Qt::black);
   std::fill(_drillColor.begin(), _drillColor.end(), Qt::black);
   _drillNumber.resize(_handler->nDrills(), std::numeric_limits<size_t>::max());
   std::fill(_drillNumber.begin(), _drillNumber.end(), 
         std::numeric_limits<size_t>::max());
}

bool IsolinesWidget::drillsColorExist()
{
   return _drillColor.size()>0 ? true : false;
}

int IsolinesWidget::isoSize()
{
   return _lines.size();
}

int IsolinesWidget::isoLinesSize(const int & isol)
{
   return _lines[isol].size();
}

QPointF IsolinesWidget::line1P(const int & isol, const int & iline)
{
   return _lines[isol][iline].p1();
}

QPointF IsolinesWidget::line2P(const int & isol, const int & iline)
{
   return _lines[isol][iline].p2();
}

void IsolinesWidget::clearIsoList()
{
   int limit = _list->count();
   for(int i=0;i<limit;i++)
   {
      delete _list->takeItem(0);
      _handler->deleteIsoValue(0);
   }
}

void IsolinesWidget::calculate()
{
   std::vector<point> inPoints;
   std::vector<double> z;
   for(int i=0; i<_handler->nDrills(); i++)
   {
      if(_drillColor[i] != Qt::black)
      {
         inPoints.push_back(NaturalNeighbors::makePoint(_handler->drillX(i),
                  _handler->drillY(i), 0.0));   
         z.push_back(_drillNumber[i]);
      }
   }
   std::vector<double> outPoints;
   double deltax=(_handler->isoXmax()-_handler->isoXmin())/_handler->isoXdiv();
   double deltay=(_handler->isoYmax()-_handler->isoYmin())/_handler->isoYdiv();
   for(int i=0; i<_handler->isoXdiv()+1; i++)
   {
      for(int j=0; j<_handler->isoYdiv()+1; j++)
      {
         double pointx = _handler->isoXmin() + i*deltax;
         double pointy = _handler->isoYmin() + j*deltay;
         outPoints.push_back(pointx);
         outPoints.push_back(pointy);
      }
   }
   if(inPoints.size()<1 || outPoints.size()<1)
   {
      return;
   }
   NaturalNeighbors::ExtendedNNGrid nnGrid (inPoints);
   NaturalNeighbors::ExtendedNNCoordinates nnCoorsC (nnGrid,
         outPoints.data(), outPoints.size()/2);
   _values = nnCoorsC.interpolate(z);

   _minVal = std::numeric_limits<double>::max();
   _maxVal = -std::numeric_limits<double>::max();

   for(size_t i=0; i<_values.size(); i++)
   {
      if(_values[i]>_maxVal)
         _maxVal = _values[i];
      if(_values[i]<_minVal)
         _minVal = _values[i];
   }
   _showButton->setEnabled(true);
   _isoVal->setEnabled(true);
   _lines.clear();
   _calculateButton->setEnabled(false);
   emit repaintPict();
}

void IsolinesWidget::changeXmin()
{
   _calculateButton->setEnabled(true);   
   bool ok;
   double val= _xmin->text().toDouble(&ok);
   if(ok)
   {
      _handler->setIsoXmin(val);
   }else
   {
      _xmin->setText(QString::number(_handler->isoXmin(), 'f', 2));
   }
}

void IsolinesWidget::changeXmax()
{
   _calculateButton->setEnabled(true);
   bool ok;
   double val= _xmax->text().toDouble(&ok);
   if(ok)
   {
      _handler->setIsoXmax(val);
   }else
   {
      _xmax->setText(QString::number(_handler->isoXmax(), 'f', 2));
   }
}

void IsolinesWidget::changeYmin()
{
   _calculateButton->setEnabled(true);
   bool ok;
   double val= _ymin->text().toDouble(&ok);
   if(ok)
   {
      _handler->setIsoYmin(val);
   }else
   {
      _ymin->setText(QString::number(_handler->isoYmin(), 'f', 2));
   }
}

void IsolinesWidget::changeYmax()
{
   _calculateButton->setEnabled(true);
   bool ok;
   double val= _ymax->text().toDouble(&ok);
   if(ok)
   {
      _handler->setIsoYmin(val);
   }else
   {
      _ymax->setText(QString::number(_handler->isoYmax(), 'f', 2));
   }

}

void IsolinesWidget::changeXdiv()
{
   _calculateButton->setEnabled(true);
   bool ok;
   int val= _xdiv->text().toInt(&ok);
   if(ok)
   {
      _handler->setIsoXdiv(val);
   }else
   {
      _xdiv->setText(QString::number(_handler->isoXdiv()));
   }
}

void IsolinesWidget::changeYdiv()
{
   _calculateButton->setEnabled(true);
   bool ok;
   int val= _ydiv->text().toInt(&ok);
   if(ok)
   {
      _handler->setIsoYdiv(val);
   }else
   {
      _ydiv->setText(QString::number(_handler->isoYdiv()));
   }
}

void IsolinesWidget::calcMarchingSquares()
{
   double xmin = _handler->isoXmin();
   double ymin = _handler->isoYmin();

   double dx=(_handler->isoXmax()-_handler->isoXmin())/_handler->isoXdiv();
   double dy=(_handler->isoYmax()-_handler->isoYmin())/_handler->isoYdiv();
   const double dmax=std::numeric_limits<double>::max();

   int xdiv = _handler->isoXdiv();
   int ydiv = _handler->isoYdiv();

   int niso=_handler->isoValuesSize();
   _lines.resize(niso);
   QVector<QVector<QPointF> >  verEdgesPoint (ydiv*(xdiv+1));
   QVector<QVector<QPointF> >  horEdgesPoint (xdiv*(ydiv+1));
   for(int iver=0;iver<(ydiv*(xdiv+1));iver++)
   {
      int xcount=iver/ydiv;
      int ycount=iver%ydiv;
      QPointF p0(xmin+xcount*dx, ymin+ycount*dy);
      QPointF p1(xmin+xcount*dx, ymin+(ycount+1)*dy);
      double value0=_values[xcount*(ydiv+1) + ycount];
      double value1=_values[xcount*(ydiv+1) + ycount+1];
      verEdgesPoint[iver]= QVector<QPointF> (niso, QPointF(dmax,dmax));
      for(int iisol=0;iisol<niso;iisol++)
      {
         if((_handler->isoValue(iisol)>=value0 && 
             _handler->isoValue(iisol)<=value1) ||
               (_handler->isoValue(iisol)<=value0 &&
                _handler->isoValue(iisol)>=value1))
         {
            verEdgesPoint[iver][iisol] = interp(p0, p1, value0,
                     value1, _handler->isoValue(iisol));
         }
      }
   }
   for(int ihor=0;ihor<(xdiv*(ydiv+1));ihor++)
   {
      int xcount=ihor/(ydiv+1);
      int ycount=ihor%(ydiv+1);
      QPointF p0(xmin+xcount*dx, ymin+ycount*dy);
      QPointF p1(xmin+(xcount+1)*dx, ymin+ycount*dy);
      double value0=_values[xcount*(ydiv+1) + ycount];
      double value1=_values[(xcount+1)*(ydiv+1) + ycount];

      horEdgesPoint[ihor]=QVector<QPointF>(niso, QPointF(dmax, dmax));
      
      for(int iisol=0;iisol<niso;iisol++)
      {
         if((_handler->isoValue(iisol)>=value0 && 
             _handler->isoValue(iisol)<=value1) ||
               (_handler->isoValue(iisol)<=value0 &&
                _handler->isoValue(iisol)>=value1))
         {
            horEdgesPoint[ihor][iisol] = interp(p0, p1, value0,
                     value1, _handler->isoValue(iisol));
         }
      }
   }
   for(int iisol=0;iisol<niso;iisol++)
   {
      for(int ix=0;ix<xdiv;ix++)
      {
         for(int iy=0;iy<ydiv;iy++)
         {//za svaku isoliniju prolazimo kroz sve kvadrate
            QVector<QPointF> points;
            if(verEdgesPoint[ix*ydiv+iy][iisol].x()!=dmax)//leftVert
            {
               points.push_back(verEdgesPoint[ix*ydiv+iy][iisol]);
            }
            if(verEdgesPoint[(ix+1)*ydiv+iy][iisol].x()!=dmax)//rightVert
            {
               points.push_back(verEdgesPoint[(ix+1)*ydiv+iy][iisol]);
            }
            if(horEdgesPoint[ix*(ydiv+1)+iy][iisol].x()!=dmax)//bottomHor
            {
               points.push_back(horEdgesPoint[ix*(ydiv+1)+iy][iisol]);
            }
            if(horEdgesPoint[ix*(ydiv+1)+iy+1][iisol].x()!=dmax)//topHor
            {
               points.push_back(horEdgesPoint[ix*(ydiv+1)+iy+1][iisol]);
            }

            if(points.size()==2)
            {
               _lines[iisol].push_back(QLineF(points[0], points[1]));
            }
            else if(points.size()==4)
            {
               _lines[iisol].push_back(QLineF(points[0], points[2]));
               _lines[iisol].push_back(QLineF(points[1], points[3]));
            }
            else if(points.size()==1 || points.size()==3)
            {
               qDebug()<<" OOOOOO "<<points.size()<<"\n";
            }
         }
      }
   }
   _showButton->setEnabled(false);
   emit repaintPict();
}

void IsolinesWidget::addToList()
{
   QListWidgetItem *item = new QListWidgetItem;
   item->setData(Qt::DisplayRole, _isoVal->text().toDouble());
   _list->addItem(item);
   _handler->addIsoValue(_isoVal->text().toDouble());
   _isoVal->setText("");
   emit _list->listChanged();
}

void IsolinesWidget::deleteFromList(int i)
{
   if(i>=0)
      _handler->deleteIsoValue(i);
}

void IsolinesWidget::listCh()
{
   _lines.clear();
   _showButton->setEnabled(true);
}

void IsolinesWidget::automaticIsoValues()
{
   if(!_isoVal->isEnabled())
      return;

   QDialog dialog(this);
   dialog.setWindowTitle("Automatic isoline values");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);
   QLabel * minLabel = new QLabel("Minimal calculated drawdown is "+QString::number(_minVal));
   QLabel * maxLabel = new QLabel("Maximal calculated drawdown is "+QString::number(_maxVal));
   dialogLayout->addWidget(minLabel);
   dialogLayout->addWidget(maxLabel);
   QLabel * numberOfIsoLabel = new QLabel("Number of isolines");
   QLineEdit * num = new QLineEdit(&dialog);
   dialogLayout->addWidget(numberOfIsoLabel);
   dialogLayout->addWidget(num);


   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
   if (dialog.exec() == QDialog::Accepted) {
      int limit = _list->count();
      for(int i=0;i<limit;i++)
      {
         delete _list->takeItem(0);
         _handler->deleteIsoValue(0);
      }
      int niso=num->text().toInt();

      for(int j=1;j<=niso;j++)
      {
         double original=_minVal+j*(_maxVal-_minVal)/(niso+1);
         QListWidgetItem *item = new QListWidgetItem;
         QString number=QString::number(original, 'f',2); 
         item->setData(Qt::DisplayRole, number.toDouble()); 
         _list->addItem(item);
         _handler->addIsoValue(original);
      }
      emit _list->listChanged();
   }
}

void IsolinesWidget::somethingChangedSlot()
{
   _lines.clear();
   _calculateButton->setEnabled(true);
   _showButton->setEnabled(false);
   _isoVal->setEnabled(false);
}

void IsolinesWidget::loadFinishedSlot()
{
   _xmin->setText(QString::number(_handler->isoXmin(), 'f', 2));
   _xmax->setText(QString::number(_handler->isoXmax(), 'f', 2));
   _ymin->setText(QString::number(_handler->isoYmin(), 'f', 2));
   _ymax->setText(QString::number(_handler->isoYmax(), 'f', 2));
   _xdiv->setText(QString::number(_handler->isoXdiv()));
   _ydiv->setText(QString::number(_handler->isoYdiv()));
   int niso=_handler->isoValuesSize();
   for(int j=0;j<niso;j++)
   {
      QListWidgetItem *item = new QListWidgetItem;
      item->setData(Qt::DisplayRole, _handler->isoValue(j)); 
      _list->addItem(item);
   }
}

QPointF IsolinesWidget::interp(const QPointF & p0, const QPointF & p1, 
      const double value0, const double value1, const double value) const
{
   double alpha=(value-value0)/(value1-value0);
   return p0+alpha*(p1-p0);
}
