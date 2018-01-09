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
#include "planLeftWidget.h"
#include "planChooser.h"
#include "isolinesWidget.h"

#include <QVBoxLayout>

PlanLeftWidget::PlanLeftWidget(QGrains * qGrains, Handler * handler)
{
   QVBoxLayout * layout = new QVBoxLayout;
   _planChooser = new PlanChooser(qGrains, handler);
   layout->addWidget(_planChooser);
   _isolinesWidget = new IsolinesWidget(qGrains, handler);
   layout->addWidget(_isolinesWidget);
   layout->addStretch();
      /*
   QGridLayout * layout = new QGridLayout;
   QLabel * byLabel = new QLabel("By");
   layout->addWidget(byLabel, 0, 0, 1, 1);
   _byComboBox = new QComboBox;
   _byComboBox->addItem(QString("elevation"));
   _byComboBox->addItem(QString("depth"));
   _byComboBox->addItem(QString("surface"));
   layout->addWidget(_byComboBox, 0, 1, 1, 1);
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
   layout->addWidget(_valComboBox, 1, 0, 1, 1);
   QLabel * lll = new QLabel("XXXX");
   layout->addWidget(lll);*/
   setLayout(layout);
}
