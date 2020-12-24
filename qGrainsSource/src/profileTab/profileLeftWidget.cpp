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
#include "profileLeftWidget.h"
#include "profileMap.h"
#include "profileTab/profileSmallTabs/profileSmallTabs.h"
#include "qGrains.h"
#include "model/handlers/handler.h"
#include "model/handlers/viewHandler.h"
#include "model/modelNodeBase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

ProfileLeftWidget::ProfileLeftWidget(QGrains * qGrains, Handler * handler, ViewHandler * viewHandler, SurfHandler * surfHandler, ProfileHandler * profileHandler, PlotHandler * plotHandler):QSplitter(Qt::Vertical), _handler(handler), _viewHandler(viewHandler)
{
   QFrame * frame = new QFrame;
   QVBoxLayout * layout = new QVBoxLayout;
   _comboBox = new QComboBox;
   _comboBox->addItem(QString("d10"));
   _comboBox->addItem(QString("d15"));
   _comboBox->addItem(QString("d20"));
   _comboBox->addItem(QString("d50"));
   _comboBox->addItem(QString("d60"));
   _comboBox->addItem(QString("d85"));
   _comboBox->addItem(QString("Unif. coef."));
   _comboBox->addItem(QString("Porosity"));
   _comboBox->addItem(QString("Hazen"));
   _comboBox->addItem(QString("Slichter"));
   _comboBox->addItem(QString("Terzaghi"));
   _comboBox->addItem(QString("Beyer"));
   _comboBox->addItem(QString("Sauerbrei"));
   _comboBox->addItem(QString("Kruger"));
   _comboBox->addItem(QString("Kozeny"));
   _comboBox->addItem(QString("Zunker"));
   _comboBox->addItem(QString("Zamarin"));
   _comboBox->addItem(QString("USBR"));
   _comboBox->addItem(QString("Grain size"));
   _comboBox->setFixedWidth(95);
   _hlayout = new QHBoxLayout;
   _hlayout->addWidget(_comboBox);
   _hlayout->addStretch();
   layout->addLayout(_hlayout);

   ProfileSmallTabs * profileSmallTabs = new ProfileSmallTabs(qGrains->model(),
         handler, viewHandler, surfHandler, profileHandler, plotHandler);
//   ProfileChooser * profileChooser = new ProfileChooser(qGrains->model(),
  //       viewHandler);
   ProfileMap * map = new ProfileMap(handler, viewHandler, profileHandler,
         qGrains);

   layout->addWidget(profileSmallTabs);

   frame->setLayout(layout);

   addWidget(frame);
   addWidget(map);

   connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, 
         SLOT(indexChangedSlot(int)));

   connect(qGrains->model(), SIGNAL(loadFinished()), this, SLOT(setCombo()));
}

void ProfileLeftWidget::indexChangedSlot(int index)
{
   if(index==18)
   {//grain size
      QLayoutItem *child;
      child = _hlayout->takeAt(1);
      delete child->widget();
      delete child;
      QLabel * from = new QLabel("From");
      _hlayout->addWidget(from);
      _fromLineEdit = new QLineEdit(
            QString::number(_handler->fromProfile()));
      _hlayout->addWidget(_fromLineEdit);
      QLabel * to = new QLabel("To");
      _hlayout->addWidget(to);
      _toLineEdit = new QLineEdit(QString::number(_handler->toProfile()));
      _hlayout->addWidget(_toLineEdit);
       connect(_fromLineEdit, SIGNAL(editingFinished()), this, 
         SLOT(setProfileFrom()));
       connect(_toLineEdit, SIGNAL(editingFinished()), this,
         SLOT(setProfileTo()));
   }
   else
   {
      if(_hlayout->count()>2)
      {
         QLayoutItem *child;
         while ((child = _hlayout->takeAt(1)) != 0) {
            delete child->widget();
            delete child;
         }
         _hlayout->addStretch();
      }
   }
   _viewHandler->setTypeCalc(index);
}

void ProfileLeftWidget::setProfileFrom()
{
   bool ok;
   double val= _fromLineEdit->text().toDouble(&ok);
   if(ok)
      _viewHandler->setProfileFrom(val);
}

void ProfileLeftWidget::setProfileTo()
{
   bool ok;
   double val= _toLineEdit->text().toDouble(&ok);
   if(ok)
      _viewHandler->setProfileTo(val);
}

void ProfileLeftWidget::setCombo()
{
   _comboBox->setCurrentIndex(_handler->typeCalc());
}
