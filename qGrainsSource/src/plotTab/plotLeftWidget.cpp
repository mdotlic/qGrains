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

#include "plotLeftWidget.h"
#include "model/handlers/plotHandler.h"
#include "model/handlers/inputHandler.h"
#include "model/modelEnums.h"
#include "model/modelNodeBase.h"
#include "model/tables/depthHeaderLineColorNames.h"
#include "model/tables/coorHeaderNames.h"
#include "model/tables/tables.h"
#include "model/tables/myTableView.h"
#include "widgets/comboBoxDelegate.h"
#include <QLabel>
#include <QModelIndex>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
//#include <QItemSelection>

PlotLeftWidget::PlotLeftWidget(InputHandler * inputHandler, 
      ModelNodeBase * model):_inputHandler(inputHandler)
{
   _dHeaderNames = new DepthHeaderLineColorNames();
   _cHeaderNames = new CoorHeaderNames();


   _plotHandler = new PlotHandler(model);

   QVBoxLayout * layout = new QVBoxLayout;
   _namesTable = new MyTableView;//(_plotHandler);
   _namesTable->setMaximumHeight(250);
   //namesTable->scrollToBottom();
   //_namesTable->horizontalHeader()->setStretchLastSection(true);
   //_namesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _namesTable->setModel(_plotHandler);
   _namesTable->setRootIndex(_plotHandler->index(ModelEnums::Root::DRILLS, 0));

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
   //_depthTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _depthTable->setModel(_plotHandler);
   _dlabel = new QLabel("    Depths ");
   _dlabel->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(_dlabel);
   _dlabel->hide();
   layout->addWidget(_depthTable);
   _depthTable->hide();
   ComboBoxDelegate * delegate = new ComboBoxDelegate(1, this);
   _depthTable->setItemDelegateForColumn(2, delegate);
   _depthTable->setItemDelegateForColumn(3, delegate);

   _coorTable = new Tables(inputHandler);//(_plotHandler);
   _coorTable->setMaximumHeight(250);
   //_coorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   //_coorTable->horizontalHeader()->setStretchLastSection(true);
   _coorTable->setModel(inputHandler);
   //_coorTable->horizontalHeader()->hide();
   _clabel = new QLabel(" Coor ");
   _clabel->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(_clabel);
   layout->addWidget(_coorTable);
   _clabel->hide();
   _coorTable->hide();

   layout->addStretch();
   setLayout(layout);

   connect(_fromLineEdit, SIGNAL(returnPressed()), _toLineEdit, 
         SLOT(setFocus()));
   connect(_toLineEdit, SIGNAL(returnPressed()), selButton, 
         SLOT(setFocus()));

   connect(_namesTable->selectionModel(),
         SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
         SLOT(showDepth(const QItemSelection &, const QItemSelection &)));

   connect(_depthTable->selectionModel(),
         SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
         SLOT(showCoor(const QItemSelection &, const QItemSelection &)));

   connect(model, SIGNAL(addedDrill(const int &)), _plotHandler,
         SLOT(addDrill(const int &)));
   connect(model, SIGNAL(deletedDrill(const int &)), _plotHandler,
         SLOT(deleteDrill(const int &)));
   connect(model, SIGNAL(deletedSample(const int &, const int &)), _plotHandler,
         SLOT(deleteSample(const int &, const int &)));

   connect(label, SIGNAL(stateChanged(int)), _plotHandler,
         SLOT(allDrillsChanged(int)));

   connect(_plotHandler, SIGNAL(splineSelected(const int &, const int &)), 
         this, SLOT(splineSelectedSlot(const int &, const int &)));

   connect(_plotHandler, SIGNAL(deselectedSpline()), 
         this, SLOT(deselectedSplineSlot()));

   connect(selButton, SIGNAL(clicked()), this, SLOT(intervalSelect()));

   /*  connect(_namesTable, SIGNAL(selChange(const QModelIndex &)), this,
       SLOT(showDepth(const QModelIndex &)));
       connect(_depthTable, SIGNAL(selChange(const QModelIndex &)), this, 
       SLOT(showCoor(const QModelIndex &)));*/
}

void PlotLeftWidget::showDepth(const QItemSelection & sel, 
      const QItemSelection & /*desel*/)
{
   QModelIndex index;
   if(sel.indexes().size()==1)
      index=sel.indexes().at(0);
   else
      return;

   _coorTable->hide();
   _clabel->hide();

   _depthTable->horizontalHeader()->setModel(_plotHandler);//budzevina da se ne buni
   QModelIndex index0 = index.sibling(index.row(),0);
   _depthTable->setRootIndex(index0);

   _depthTable->horizontalHeader()->setModel(_dHeaderNames);

   int nsample = _plotHandler->rowCount(index0);
   for(int i=0;i<nsample;i++)
   {
      _depthTable->openPersistentEditor( _plotHandler->index(i, 2, index0));
      _depthTable->openPersistentEditor( _plotHandler->index(i, 3, index0));
   }

   //_depthTable->clearSelection();
   _depthTable->show();
   /* if(_plotHandler->rowCount(index0)>0)
      _depthTable->setCurrentIndex(_plotHandler->index(_plotHandler->rowCount(index0)-1, 0, index0)); //seting current to last element
      else
      _coorTable->hide();
    */
   _dlabel->setText(" Samples of the drill "+_plotHandler->name(index0));
   _dlabel->show();
}

void PlotLeftWidget::showCoor(const QItemSelection & sel, 
      const QItemSelection & /*desel*/)
{//show coordinates   
   QModelIndex index;
   if(sel.indexes().size()==1)
      index=sel.indexes().at(0);
   else
      return;

   _coorTable->horizontalHeader()->setModel(_inputHandler);//budzevina da se ne buni
  // QModelIndex index0 = index.sibling(index.row(),0);
  QModelIndex drillsIndex = _inputHandler->index(ModelEnums::Root::DRILLS, 0, QModelIndex());
  QModelIndex drillIndex = _inputHandler->index(_namesTable->currentIndex().row(), 0, drillsIndex);
  QModelIndex sampleIndex = _inputHandler->index(_depthTable->currentIndex().row(), 0, drillIndex);
  
   QModelIndex index1 = _inputHandler->index(0,0,sampleIndex);
   _coorTable->setRootIndex(index1);

   _coorTable->horizontalHeader()->setModel(_cHeaderNames);
   _coorTable->clearSelection();
   _coorTable->show();
  // QModelIndex index1 = _modelHandler->index(0,0,index);  
  //_coorTable->setCurrentIndex(_plotHandler->index(_plotHandler->rowCount(index1)-1, 0, index1)); //seting current to last element


  QModelIndex index2 = _plotHandler->index(index.row(),0,_plotHandler->parent(index));
  QModelIndex index3 = _plotHandler->index(index.row(),1,_plotHandler->parent(index));
   _clabel->setText("Particle size/percentage at depth "+QString::number(_plotHandler->data(index2).toDouble(), 'f', 2)+ " - " + QString::number(_plotHandler->data(index3).toDouble(), 'f',2));
   _clabel->show();

   emit _plotHandler->sampleSelected(_namesTable->currentIndex().row(), index.row());
}

void PlotLeftWidget::splineSelectedSlot(const int & idrill, const int & jsample)
{
//   QModelIndex index = _namesTable->currentIndex().sibling(idrill,0);
   if(!_namesTable->currentIndex().isValid())   
      _namesTable->setCurrentIndex(_plotHandler->index(0,0,_namesTable->rootIndex()));
   QModelIndex drillIndex = _namesTable->currentIndex().sibling(idrill,0);
   _namesTable->setCurrentIndex(drillIndex);

   _depthTable->setCurrentIndex(_plotHandler->index(0,0,_depthTable->rootIndex()));

   _depthTable->setCurrentIndex(_depthTable->currentIndex().sibling(jsample,0));
}

void PlotLeftWidget::deselectedSplineSlot()
{
   //_namesTable->clearSelection();
   _depthTable->clearSelection();
}

void PlotLeftWidget::intervalSelect()
{
   bool fromOk, toOk;

   double fromValue = _fromLineEdit->text().toDouble(&fromOk);
   double toValue = _toLineEdit->text().toDouble(&toOk);
   if(fromOk && toOk)
   {
      _plotHandler->allDrillsChanged(0);
      _plotHandler->selectInInterval(fromValue, toValue);
   }
}

