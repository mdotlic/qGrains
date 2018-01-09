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
#include "drillPercChooser.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/handlers/viewHandler.h"
#include "model/tables/myTableView.h"
#include "widgets/comboBoxDelegate.h"
#include <QVBoxLayout>
#include <QHeaderView>


DrillPercChooser::DrillPercChooser(ModelNodeBase * model)
{
   QVBoxLayout * layout = new QVBoxLayout;
   _table = new MyTableView;
   _table->setEditTriggers(QAbstractItemView::AllEditTriggers);

   _viewHandler = new ViewHandler(model, 1);

   _table->setMaximumHeight(250);
   //namesTable->scrollToBottom();
   //_namesTable->horizontalHeader()->setStretchLastSection(true);
   _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _table->setModel(_viewHandler);
   _table->setRootIndex(_viewHandler->index(ModelEnums::Root::CHECKS, 0));


   layout->addWidget(_table);
   setLayout(layout);
   
   ComboBoxDelegate * delegate = new ComboBoxDelegate(2, this);
   _table->setItemDelegateForColumn(1, delegate);
   //CheckBoxDelegate * cdelegate = new CheckBoxDelegate;
   //_table->setItemDelegateForColumn(0, cdelegate);
   //ComboBoxDelegate * delegate = new ComboBoxDelegate;
   //_table->setItemDelegateForColumn(1, delegate);
   //_table->setItemDelegateForColumn(2, delegate);

  connect(model, SIGNAL(addedDrill(const int &)), _viewHandler, SLOT(addDrill(const int &)));
   connect(model, SIGNAL(deletedDrill(const int &)), _viewHandler, SLOT(deleteDrill(const int &)));

   connect(model, SIGNAL(addedDrill(const int &)), this, SLOT(addDrillPersistent(const int &)));
   connect(model, SIGNAL(deletedDrill(const int &)), this, SLOT(deleteDrillPersistent(const int &)));
   
   connect(model, SIGNAL(loadFinished()), this, SLOT(remakePers()));
}

void DrillPercChooser::addDrillPersistent(const int & idrill)
{
   
   _table->closePersistentEditor( _viewHandler->index(idrill, 1, _viewHandler->index(ModelEnums::Root::CHECKS, 0)) );
   _table->openPersistentEditor( _viewHandler->index(idrill, 1, _viewHandler->index(ModelEnums::Root::CHECKS, 0)) );
   
}

void DrillPercChooser::deleteDrillPersistent(const int & idrill)
{
   _table->closePersistentEditor( _viewHandler->index(idrill, 1, _viewHandler->index(ModelEnums::Root::CHECKS, 0)) );
   
}

void DrillPercChooser::remakePers()
{
   for(int i=0;i<_viewHandler->nDrills();i++)
   {
      addDrillPersistent(i);
   }
}
