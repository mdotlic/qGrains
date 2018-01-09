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
#include "sizeChooser.h"
#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/handlers/sizeHandler.h"
#include "model/tables/myTableView.h"
#include "widgets/comboBoxDelegate.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QDebug>

SizeChooser::SizeChooser(ModelNodeBase * model)
{
   QVBoxLayout * layout = new QVBoxLayout;
   _table = new MyTableView;
   _table->setEditTriggers(QAbstractItemView::AllEditTriggers);

   _sizeHandler = new SizeHandler(model);

   _table->setFixedHeight(230);
   //namesTable->scrollToBottom();
   //_namesTable->horizontalHeader()->setStretchLastSection(true);
   //_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   //table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _table->setModel(_sizeHandler);
   _table->setRootIndex(_sizeHandler->index(ModelEnums::Root::PERCCHECK, 0));


   layout->addWidget(_table);
   setLayout(layout);
   //CheckBoxDelegate * cdelegate = new CheckBoxDelegate;
   //_table->setItemDelegateForColumn(0, cdelegate);
   ComboBoxDelegate * delegate = new ComboBoxDelegate(1, this);
   _table->setItemDelegateForColumn(2, delegate);
   _table->setItemDelegateForColumn(3, delegate);


   _table->openPersistentEditor( _sizeHandler->index(0, 2, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
   _table->openPersistentEditor( _sizeHandler->index(0, 3, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );


    connect(model, SIGNAL(loadFinished()), this,
          SLOT(remakePersistant()));
    connect(_sizeHandler, SIGNAL(sizeDataAppend(const int &, const int &)),
          this, SLOT(sizeChangePos(const int &, const int &)));

}

void SizeChooser::remakePersistant()
{
   int nsizes = _sizeHandler->nsizes();
   for ( int i = 0; i < nsizes; ++i )
   {
      _table->closePersistentEditor( _sizeHandler->index(i, 2, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
      _table->closePersistentEditor( _sizeHandler->index(i, 3, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
      _table->openPersistentEditor( _sizeHandler->index(i, 2, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
      _table->openPersistentEditor( _sizeHandler->index(i, 3, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
   }
}

void SizeChooser::sizeChangePos(const int & row, const int & col)
{
   if(col==0)
   {
      QModelIndex newIndex;
      newIndex = _table->currentIndex().sibling(row, 1);
      _table->setCurrentIndex(newIndex);
      return;
   }else
   {
      QModelIndex newIndex = _table->currentIndex().sibling(row+1, 0);;
      _table->setCurrentIndex(newIndex);
      _table->scrollToBottom();
      for(int i=0;i<=row;i++)
      {
         _table->closePersistentEditor( _sizeHandler->index(i, 2, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
         _table->closePersistentEditor( _sizeHandler->index(i, 3, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
         _table->openPersistentEditor( _sizeHandler->index(i, 2, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
         _table->openPersistentEditor( _sizeHandler->index(i, 3, _sizeHandler->index(ModelEnums::Root::PERCCHECK, 0)) );
      }
   }
}
