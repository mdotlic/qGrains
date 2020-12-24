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
#include "planChooser.h"
#include "qGrains.h"
#include "model/modelNodeBase.h"
#include "model/handlers/handler.h"
#include "model/handlers/plotHandler.h"
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDebug>
#include <math.h>

PlanChooser::PlanChooser(QGrains * qGrains, Handler * handler, PlotHandler * plotHandler):_handler(handler), _plotHandler(plotHandler)
{   
   QVBoxLayout * layout = new QVBoxLayout;
   _hByLayout = new QHBoxLayout;
   QLabel * byLabel = new QLabel("By");
   _hByLayout->addWidget(byLabel);
   _byComboBox = new QComboBox;
   _byComboBox->addItem(QString("elevation"));
   _byComboBox->addItem(QString("depth"));
   _byComboBox->addItem(QString("surface"));
   _byComboBox->addItem(QString("thickness"));
   _byComboBox->setFixedWidth(95);
   _hByLayout->addWidget(_byComboBox);

   QLabel * from = new QLabel("From");
   _hByLayout->addWidget(from);
   _fromLineEdit = new QLineEdit(QString::number(_handler->planFrom()));
   _hByLayout->addWidget(_fromLineEdit);
   QLabel * to = new QLabel("To");
   _hByLayout->addWidget(to);
   _toLineEdit = new QLineEdit(QString::number(_handler->planTo()));
   _hByLayout->addWidget(_toLineEdit);
   connect(_fromLineEdit, SIGNAL(editingFinished()), this, 
         SLOT(setPlanFrom()));
   connect(_toLineEdit, SIGNAL(editingFinished()), this,
         SLOT(setPlanTo()));

   _hByLayout->addStretch();
   layout->addLayout(_hByLayout);
   QLabel * valLabel = new QLabel("Value");
   _hValLayout = new QHBoxLayout;
   _hValLayout->addWidget(valLabel);
   _valComboBox = new QComboBox;
   _valComboBox->addItem(QString("d10"));
   _valComboBox->addItem(QString("d15"));
   _valComboBox->addItem(QString("d20"));
   _valComboBox->addItem(QString("d50"));
   _valComboBox->addItem(QString("d60"));
   _valComboBox->addItem(QString("d85"));
   _valComboBox->addItem(QString("Unif. coef."));
   _valComboBox->addItem(QString("Porosity"));
   _valComboBox->addItem(QString("Hazen"));
   _valComboBox->addItem(QString("Slichter"));
   _valComboBox->addItem(QString("Terzaghi"));
   _valComboBox->addItem(QString("Beyer"));
   _valComboBox->addItem(QString("Sauerbrei"));
   _valComboBox->addItem(QString("Kruger"));
   _valComboBox->addItem(QString("Kozeny"));
   _valComboBox->addItem(QString("Zunker"));
   _valComboBox->addItem(QString("Zamarin"));
   _valComboBox->addItem(QString("USBR"));
   _valComboBox->addItem(QString("Grain size"));
   _valComboBox->setFixedWidth(95);
   _hValLayout->addWidget(_valComboBox);
   _hValLayout->addStretch();
   layout->addLayout(_hValLayout);
   layout->addStretch();

   setLayout(layout);

   connect(_byComboBox, SIGNAL(currentIndexChanged(int)), this, 
         SLOT(indexChangedBySlot(int)));
   
   connect(_valComboBox, SIGNAL(currentIndexChanged(int)), this, 
         SLOT(indexChangedValSlot(int)));

   connect(qGrains->model(), SIGNAL(loadFinished()), this, SLOT(setLoad()));

   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
         this, SLOT(contextMenuRequest(QPoint)));

}

void PlanChooser::contextMenuRequest(QPoint pos)
{
   if(_handler->planType() == 2)
      return;
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction(" Select on Plot tab", this, SLOT(selectOnPlotTab()));
   menu->popup(mapToGlobal(pos));
}

void PlanChooser::selectOnPlotTab()
{
   _plotHandler->allDrillsChanged(0);//uncheck all

   if(_handler->planType() == 3)
   {
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
            for(int isample=0; isample<_handler->nSample(idrill); isample++)
            {
               if(_handler->sampleVal(idrill, isample, 1) >= 
                     _handler->elev(idrill)-fromZ[idrill] 
                     && 
                     _handler->sampleVal(idrill, isample, 0) <=
                     _handler->elev(idrill)-toZ[idrill])
               {
                  _plotHandler->showSample(idrill, isample);
               }
            }
         }
      }
   }
   else
   {
      if(_handler->planFrom() > _handler->planTo())
         return;
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

         for(int isample=0; isample<_handler->nSample(i); isample++)
         {
            if(_handler->sampleVal(i, isample, 1)>=from && 
                  _handler->sampleVal(i, isample, 0)<=to)
            {
               _plotHandler->showSample(i, isample);
            }
         }
      }
   }
}

void PlanChooser::indexChangedBySlot(int index)
{
   if(index == 2)
   {
      if(_hByLayout->count()>2)
      {
         QLayoutItem *child;
         while ((child = _hByLayout->takeAt(2)) != 0) {
            delete child->widget();
            delete child;
         }
      }
      QComboBox * surfComboBox = new QComboBox;
      for(int i=0;i<_handler->nSurfaces();i++)
      {
         surfComboBox->addItem(_handler->surfName(i));
      }
      surfComboBox->setCurrentIndex(_handler->planSurface());
      _hByLayout->addWidget(surfComboBox);
      _hByLayout->addStretch();
      connect(surfComboBox, SIGNAL(currentIndexChanged(int)), this, 
         SLOT(setSurfaceId(int)));
      _handler->setPlanType(index);
      emit changeSurfPicture();
      _valComboBox->setEnabled(false);
   }
   else if(index==3)   
   {
      if(_hByLayout->count()>2)
      {
         QLayoutItem *child;
         while ((child = _hByLayout->takeAt(2)) != 0) {
            delete child->widget();
            delete child;
         }
      }
      QComboBox * surfFromComboBox = new QComboBox;
      for(int i=0;i<_handler->nSurfaces();i++)
      {
         surfFromComboBox->addItem(_handler->surfName(i));
      }
      surfFromComboBox->setCurrentIndex(_handler->planSurfaceFrom());
      _hByLayout->addWidget(surfFromComboBox);
      QComboBox * surfToComboBox = new QComboBox;
      for(int i=0;i<_handler->nSurfaces();i++)
      {
         surfToComboBox->addItem(_handler->surfName(i));
      }
      surfToComboBox->setCurrentIndex(_handler->planSurfaceTo());
      _hByLayout->addWidget(surfToComboBox);
      _hByLayout->addStretch();
      connect(surfFromComboBox, SIGNAL(currentIndexChanged(int)), this, 
         SLOT(setSurfaceFrom(int)));
      connect(surfToComboBox, SIGNAL(currentIndexChanged(int)), this, 
         SLOT(setSurfaceTo(int)));

      _handler->setPlanType(index);
      if(_valComboBox->count() == 19)
      {
         _valComboBox->addItem(QString("thickness"));
         _valComboBox->setCurrentIndex(19);
      }
      _valComboBox->setEnabled(true);
      emit changeSurfThicknessPicture();
   }
   else
   {
      if(_hByLayout->count()>2)
      {
         QLayoutItem *child;
         while ((child = _hByLayout->takeAt(2)) != 0) {
            delete child->widget();
            delete child;
         }
      }
      QLabel * from = new QLabel("From");
      _hByLayout->addWidget(from);
      _fromLineEdit = new QLineEdit(QString::number(_handler->planFrom()));
      _hByLayout->addWidget(_fromLineEdit);
      QLabel * to = new QLabel("To");
      _hByLayout->addWidget(to);
      _toLineEdit = new QLineEdit(QString::number(_handler->planTo()));
      _hByLayout->addWidget(_toLineEdit);
      connect(_fromLineEdit, SIGNAL(editingFinished()), this, 
         SLOT(setPlanFrom()));
      connect(_toLineEdit, SIGNAL(editingFinished()), this,
         SLOT(setPlanTo()));
      _handler->setPlanType(index);

      if(_valComboBox->count() == 20)
      {
         _valComboBox->setCurrentIndex(0);
         _valComboBox->removeItem(19);
      }
      emit changePicture();
      _valComboBox->setEnabled(true);
   }

}

void PlanChooser::indexChangedValSlot(int index)
{
   if(index==18)
   {//grain size
      QLayoutItem *child;
      child = _hValLayout->takeAt(2);
      delete child->widget();
      delete child;
      QLabel * from = new QLabel("From");
      _hValLayout->addWidget(from);
      _fromSizeLineEdit = new QLineEdit(QString::number(
               _handler->planFromSize()));
      _hValLayout->addWidget(_fromSizeLineEdit);
      QLabel * to = new QLabel("To");
      _hValLayout->addWidget(to);
      _toSizeLineEdit = new QLineEdit(QString::number(_handler->planToSize()));
      _hValLayout->addWidget(_toSizeLineEdit);
      connect(_fromSizeLineEdit, SIGNAL(editingFinished()), this, 
         SLOT(setPlanFromSize()));
      connect(_toSizeLineEdit, SIGNAL(editingFinished()), this,
         SLOT(setPlanToSize()));
   }
   else
   {
      if(_hValLayout->count()>3)
      {
         QLayoutItem *child;
         while ((child = _hValLayout->takeAt(2)) != 0) {
            delete child->widget();
            delete child;
         }
         _hValLayout->addStretch();
      }
   }
   _handler->setPlanCalcType(index);
   if(_handler->planType()==2)
      emit changeSurfPicture();
   else if(_handler->planType()==3)
      emit changeSurfThicknessPicture();
   else
      emit changePicture();
}

void PlanChooser::setPlanFrom()
{
   bool ok;
   double val= _fromLineEdit->text().toDouble(&ok);
   if(ok)
   {
      if(fabs(val-_handler->planFrom())>1e-15)
      {
         _handler->setPlanFrom(val);
         emit changePicture();
      }
   }
}

void PlanChooser::setPlanTo()
{
   bool ok;
   double val= _toLineEdit->text().toDouble(&ok);
   if(ok)
   { 
      if(fabs(val-_handler->planTo())>1e-15)
      {
         _handler->setPlanTo(val);
         emit changePicture();
      }
   }
}

void PlanChooser::setSurfaceId(int id)
{
   _handler->setPlanSurface(id);
   emit changeSurfPicture();
}

void PlanChooser::setSurfaceFrom(int id)
{
   _handler->setPlanSurfaceFrom(id);
   emit changeSurfThicknessPicture();
}

void PlanChooser::setSurfaceTo(int id)
{
   _handler->setPlanSurfaceTo(id);
   emit changeSurfThicknessPicture();
}

void PlanChooser::setPlanFromSize()
{
   bool ok;
   double val= _fromSizeLineEdit->text().toDouble(&ok);
   if(ok)
   {
      if(fabs(val-_handler->planFromSize())>1e-15)
      {
         _handler->setPlanFromSize(val);
         if(_handler->planType()==2)
            emit changeSurfPicture();
         else
            emit changePicture();
      }
   }
}

void PlanChooser::setPlanToSize()
{
   bool ok;
   double val= _toSizeLineEdit->text().toDouble(&ok);
   if(ok)
   {
      if(fabs(val-_handler->planToSize())>1e-15)
      {
         _handler->setPlanToSize(val);
         if(_handler->planType()==2)
            emit changeSurfPicture();
         else
            emit changePicture();
      }
   }
}

void PlanChooser::setLoad()
{
   _byComboBox->setCurrentIndex(_handler->planType());
   _valComboBox->setCurrentIndex(_handler->planCalcType());
   if(_handler->planType()<2)
   {
      _fromLineEdit->setText(QString::number(_handler->planFrom()));
      _toLineEdit->setText(QString::number(_handler->planTo()));
      if(_handler->planCalcType()==18)
      {
         _fromSizeLineEdit->setText(QString::number(_handler->planFromSize()));
         _toSizeLineEdit->setText(QString::number(_handler->planToSize()));
      }
   }
}
