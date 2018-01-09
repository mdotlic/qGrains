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

#include "condLeftWidget.h"
#include "model/handlers/condHandler.h"
#include "model/modelEnums.h"
#include "model/modelNodeBase.h"
#include "model/tables/depthHeaderNames.h"
#include "model/tables/myTableView.h"
#include "condTab/condTable.h"
#include <QLabel>
#include <QModelIndex>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QButtonGroup>

CondLeftWidget::CondLeftWidget(ModelNodeBase * model, CondHandler * condHandler, CondTable * condTable):_condHandler(condHandler), _condTable(condTable)
{
   _dHeaderNames = new DepthHeaderNames();

   //_condHandler = new CondHandler(model);
   
   QVBoxLayout * layout = new QVBoxLayout;
   
   //QFrame * checkFrame = new QFrame;
   QGroupBox * group = new QGroupBox("Show column");
   QGridLayout * gridCheckLayout = new QGridLayout;
//   QLabel * condLabel = new QLabel("Show column");
//   gridCheckLayout->addWidget(condLabel,0,0,1,1);
   QCheckBox * uniCheck = new QCheckBox("Unif. coef.");
   uniCheck->setToolTip("<img src=':resources/images/neravnomernost.png'/>");
   gridCheckLayout->addWidget(uniCheck,1,0,1,1);
   QCheckBox * porCheck = new QCheckBox("Porosity");
   porCheck->setToolTip("<img src=':resources/images/poroznost.png'/>");
   gridCheckLayout->addWidget(porCheck,1,1,1,1);
   QCheckBox * hazCheck = new QCheckBox("Hazen");
   hazCheck->setToolTip("<img src=':resources/images/hazen.png'/>");
   gridCheckLayout->addWidget(hazCheck,1,2,1,1);
   QCheckBox * sliCheck = new QCheckBox("Slichter");
   sliCheck->setToolTip("<img src=':resources/images/slihter.png'/>");
   gridCheckLayout->addWidget(sliCheck,2,0,1,1);
   QCheckBox * terCheck = new QCheckBox("Terzaghi");
   terCheck->setToolTip("<img src=':resources/images/tercagi.png'/>");
   gridCheckLayout->addWidget(terCheck,2,1,1,1);
   QCheckBox * bejCheck = new QCheckBox("Beyer");
   bejCheck->setToolTip("<img src=':resources/images/bejer.png'/>");
   gridCheckLayout->addWidget(bejCheck,2,2,1,1);
   QCheckBox * zauCheck = new QCheckBox("Sauerbrei");
   zauCheck->setToolTip("<img src=':resources/images/Zauerbrej.png'/>");
   gridCheckLayout->addWidget(zauCheck,3,0,1,1); 
   QCheckBox * kriCheck = new QCheckBox("Kruger");
   kriCheck->setToolTip("<img src=':resources/images/kriger.png'/>");
   gridCheckLayout->addWidget(kriCheck,3,1,1,1);
   QCheckBox * kozCheck = new QCheckBox("Kozeny");
   kozCheck->setToolTip("<img src=':resources/images/kozeni.png'/>");
   gridCheckLayout->addWidget(kozCheck,3,2,1,1);
   QCheckBox * cunCheck = new QCheckBox("Zunker");
   cunCheck->setToolTip("<img src=':resources/images/cunker.png'/>");
   gridCheckLayout->addWidget(cunCheck,4,0,1,1); 
   QCheckBox * zamCheck = new QCheckBox("Zamarin");
   zamCheck->setToolTip("<img src=':resources/images/zamarin.png'/>");
   gridCheckLayout->addWidget(zamCheck,4,1,1,1);
   QCheckBox * usbrCheck = new QCheckBox("USBR");
   usbrCheck->setToolTip("<img src=':resources/images/usbr.png'/>");
   gridCheckLayout->addWidget(usbrCheck,4,2,1,1);

   _buttonGroup = new QButtonGroup;
   _buttonGroup->addButton(uniCheck, 2);
   _buttonGroup->addButton(porCheck, 3);
   _buttonGroup->addButton(hazCheck, 4);
   _buttonGroup->addButton(sliCheck, 5);
   _buttonGroup->addButton(terCheck, 6);
   _buttonGroup->addButton(bejCheck, 7);
   _buttonGroup->addButton(zauCheck, 8);
   _buttonGroup->addButton(kriCheck, 9);
   _buttonGroup->addButton(kozCheck, 10);
   _buttonGroup->addButton(cunCheck, 11);
   _buttonGroup->addButton(zamCheck, 12);
   _buttonGroup->addButton(usbrCheck, 13);
   _buttonGroup->setExclusive(false);

   group->setLayout(gridCheckLayout);
   group->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");
  // group->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(group);

   _namesTable = new MyTableView;//(_plotHandler);
   _namesTable->setMaximumHeight(250);
   //namesTable->scrollToBottom();
   //_namesTable->horizontalHeader()->setStretchLastSection(true);
   _namesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _namesTable->setModel(_condHandler);
   _namesTable->setRootIndex(_condHandler->index(ModelEnums::Root::DRILLS, 0));

   QCheckBox * label = new QCheckBox(" All drills ");
   //label->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(label);
   QHBoxLayout * hlayout = new QHBoxLayout;
   QLabel * fromLabel = new QLabel("All samples from");
   hlayout->addWidget(fromLabel);
   _fromLineEdit = new QLineEdit;
   //_fromLineEdit->setFixedWidth(50);
   //fromLineEdit->setSizePolicy(QSizePolicy::Minimum);
   hlayout->addWidget(_fromLineEdit);
   QLabel * toLabel = new QLabel("to");
   hlayout->addWidget(toLabel);
   _toLineEdit = new QLineEdit; 
   //_toLineEdit->setFixedWidth(50);
   hlayout->addWidget(_toLineEdit);
   QPushButton * selButton = new QPushButton("Select");
   hlayout->addWidget(selButton);


   layout->addLayout(hlayout);


   layout->addWidget(_namesTable);

   _depthTable = new MyTableView;//(_plotHandler);
   _depthTable->setMaximumHeight(250);
   _depthTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _depthTable->setModel(_condHandler);
   _dlabel = new QLabel("    Depths ");
   _dlabel->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(_dlabel);
   _dlabel->hide();
   layout->addWidget(_depthTable);
   _depthTable->hide();

   layout->addStretch();
   setLayout(layout);
   
   connect(_fromLineEdit, SIGNAL(returnPressed()), _toLineEdit, 
         SLOT(setFocus()));
   connect(_toLineEdit, SIGNAL(returnPressed()), selButton, 
         SLOT(setFocus()));

   connect(_namesTable->selectionModel(),
     SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
     SLOT(showDepth(const QItemSelection &, const QItemSelection &)));
   
  /* connect(_depthTable->selectionModel(),
     SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
     SLOT(showCoor(const QItemSelection &, const QItemSelection &)));*/

   connect(model, SIGNAL(addedDrill(const int &)), _condHandler,
         SLOT(addDrill(const int &)));
   connect(model, SIGNAL(deletedDrill(const int &)), _condHandler,
         SLOT(deleteDrill(const int &)));

   connect(label, SIGNAL(stateChanged(int)), _condHandler,
         SLOT(allDrillsChanged(int)));

  /* connect(_condHandler, SIGNAL(splineSelected(const int &, const int &)), 
         this, SLOT(splineSelectedSlot(const int &, const int &)));
   
   connect(_condHandler, SIGNAL(deselectedSpline()), 
         this, SLOT(deselectedSplineSlot()));*/

   connect(selButton, SIGNAL(clicked()), this, SLOT(intervalSelect()));

   connect(_buttonGroup, SIGNAL(buttonReleased(int)), this,
         SLOT(buttonReleasedSlot(int)));

   /*connect(_namesTable, SIGNAL(selChange(const QModelIndex &)), this,
         SLOT(showDepth(const QModelIndex &)));*/
   connect(model, SIGNAL(loadFinished()), this, SLOT(loadCondSlot()));
}

void CondLeftWidget::showDepth(const QItemSelection & sel, 
      const QItemSelection & /*desel*/)
{
   QModelIndex index;
   if(sel.indexes().size()==1)
      index=sel.indexes().at(0);
   else
      return;
   
   _depthTable->horizontalHeader()->setModel(_condHandler);//budzevina da se ne buni
   QModelIndex index0 = index.sibling(index.row(),0);
   _depthTable->setRootIndex(index0);
   
  _depthTable->horizontalHeader()->setModel(_dHeaderNames);

   //_depthTable->clearSelection();
   _depthTable->show();
  /* if(_plotHandler->rowCount(index0)>0)
      _depthTable->setCurrentIndex(_plotHandler->index(_plotHandler->rowCount(index0)-1, 0, index0)); //seting current to last element
   else
      _coorTable->hide();
      */
   _dlabel->setText(" Samples of the drill "+_condHandler->name(index0));
   _dlabel->show();
}

void CondLeftWidget::intervalSelect()
{
   bool fromOk, toOk;

   double fromValue = _fromLineEdit->text().toDouble(&fromOk);
   double toValue = _toLineEdit->text().toDouble(&toOk);
   if(fromOk && toOk)
   {
      _condHandler->allDrillsChanged(0);
      _condHandler->selectInInterval(fromValue, toValue);
   }
}

void CondLeftWidget::buttonReleasedSlot(int id)
{
   _condHandler->setCondCheck(id, _buttonGroup->button(id)->isChecked());
   _condTable->showHideCol(id, _buttonGroup->button(id)->isChecked());
}

void CondLeftWidget::loadCondSlot()
{
   for(size_t id=2;id<14;id++)
   {
      _buttonGroup->button(id)->setChecked(_condHandler->condCheck(id));
      _condTable->showHideCol(id, _buttonGroup->button(id)->isChecked());
   }
}
