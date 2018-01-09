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
#include "dChooser.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/handlers/dHandler.h"
#include "model/tables/myTableView.h"
#include "widgets/comboBoxDelegate.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QDebug>

DChooser::DChooser(ModelNodeBase * model): _model(model)
{
   QVBoxLayout * layout = new QVBoxLayout;
   _table = new MyTableView;
   _table->setEditTriggers(QAbstractItemView::AllEditTriggers);

   _dHandler = new DHandler(model);

   _table->setFixedHeight(207);
   //namesTable->scrollToBottom();
   //_namesTable->horizontalHeader()->setStretchLastSection(true);
   _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   //table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _table->setModel(_dHandler);
   _table->setRootIndex(_dHandler->index(ModelEnums::Root::DCHECKED, 0));

   layout->addWidget(_table);
   setLayout(layout);
   //CheckBoxDelegate * cdelegate = new CheckBoxDelegate;
   //_table->setItemDelegateForColumn(0, cdelegate);
   ComboBoxDelegate * delegate = new ComboBoxDelegate(0, this);
   _table->setItemDelegateForColumn(1, delegate);
   _table->setItemDelegateForColumn(2, delegate);
   //_table->setItemDelegateForColumn(3, delegate);


    for ( int i = 0; i < 6; ++i )
    {
       _table->openPersistentEditor( _dHandler->index(i, 1, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
       _table->openPersistentEditor( _dHandler->index(i, 2, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
       //_table->openPersistentEditor( _dHandler->index(i, 3, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
    }  
    connect(_model, SIGNAL(loadFinished()), this, SLOT(remakePersistant()));

}

void DChooser::remakePersistant()
{
   for ( int i = 0; i < 6; ++i )
   {
      _table->closePersistentEditor( _dHandler->index(i, 1, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
      _table->closePersistentEditor( _dHandler->index(i, 2, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
      //_table->closePersistentEditor( _dHandler->index(i, 3, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
      _table->openPersistentEditor( _dHandler->index(i, 1, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
      _table->openPersistentEditor( _dHandler->index(i, 2, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
      //_table->openPersistentEditor( _dHandler->index(i, 3, _dHandler->index(ModelEnums::Root::DCHECKED, 0)) );
   }
}

