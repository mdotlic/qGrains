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
#include "profileChooser.h"
#include "model/handlers/viewHandler.h"
#include "model/handlers/profileHandler.h"
#include "model/handlers/handler.h"
#include "model/handlers/plotHandler.h"
#include "model/tables/profileTable.h"
#include "model/modelEnums.h"
#include "model/modelNodeBase.h"
#include "widgets/comboBoxDelegate.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QDebug>

ProfileChooser::ProfileChooser(ModelNodeBase * model, Handler * handler,
      ViewHandler * viewHandler, ProfileHandler * profileHandler, 
      PlotHandler * plotHandler):_handler(handler), _viewHandler(viewHandler),
   _profileHandler(profileHandler), _plotHandler(plotHandler)
{
   QVBoxLayout * layout = new QVBoxLayout;

   ProfileTable * profileTable = new ProfileTable(_profileHandler);
   
   profileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   profileTable->setModel(_profileHandler);
   QModelIndex profileTabIndex = _profileHandler->index(
         ModelEnums::Root::PROFILETAB, 0);
   profileTable->setRootIndex(_profileHandler->index(ModelEnums::ProfileTab::Profiles, 0, profileTabIndex));

   layout->addWidget(profileTable);

   _label = new QLabel("Profile 1 ");
   _label->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(_label);

   _table = new QTableView;//(_plotHandler);
  // _table->setMaximumHeight(250);
   
   _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _table->setModel(_viewHandler);
   _table->setRootIndex(_viewHandler->index(ModelEnums::Root::CHECKS, 0));
   
   layout->addWidget(_table);

   setLayout(layout);

   ComboBoxDelegate * delegate = new ComboBoxDelegate(0, this);
   _table->setItemDelegateForColumn(1, delegate);

   connect(model, SIGNAL(addedDrill(const int &)), _viewHandler,
         SLOT(addDrill(const int &)));
   connect(model, SIGNAL(deletedDrill(const int &)), _viewHandler,
         SLOT(deleteDrill(const int &)));
   
   connect(model, SIGNAL(addedDrill(const int &)), this, SLOT(addDrillPersistent(const int &)));
   connect(model, SIGNAL(deletedDrill(const int &)), this, SLOT(deleteDrillPersistent(const int &)));
   
   connect(model, SIGNAL(loadFinished()), this, SLOT(remakePers()));

   connect(profileTable, SIGNAL(selChange(const QModelIndex &)), 
         _profileHandler, SLOT(profileSelChangedSlot(const QModelIndex)));

   connect(_profileHandler, SIGNAL(selectedInTable(const int &)), 
         viewHandler, SLOT(profileChanged(const int &)));

   connect(_profileHandler, SIGNAL(selectedInTable(const int &)),
         this, SLOT(setProfileName(const int &)));
   
   connect(_profileHandler, SIGNAL(changeName(const int &)),
         this, SLOT(setProfileName(const int &)));
   
   connect(_profileHandler, SIGNAL(hideDrillsTable()),
         this, SLOT(hideDrills()));

   connect(model, SIGNAL(deletedDrill(const int &)), _profileHandler,
         SLOT(deleteDrill(const int &)));
  /* connect(_fromLineEdit, SIGNAL(editingFinished()), this, 
         SLOT(setProfileFrom()));
   connect(_toLineEdit, SIGNAL(editingFinished()), this,
         SLOT(setProfileTo()));*/

   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
         this, SLOT(contextMenuRequest(QPoint)));
}

void ProfileChooser::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction(" Select all drills from profile "+_handler->profileName(_handler->activeProfile())+" on Plot tab", this, SLOT(selectOnPlotTab()));
   menu->popup(mapToGlobal(pos));
}

void ProfileChooser::selectOnPlotTab()
{
   _plotHandler->allDrillsChanged(0);//uncheck all
   int iProfile = _handler->activeProfile();
   for(size_t idrill=0; idrill<_handler->nProfilePoints(iProfile); idrill++)
   {
      _plotHandler->showAllSamples(_handler->profilePoint(iProfile, idrill));
   }
}

void ProfileChooser::addDrillPersistent(const int & idrill)
{
   _table->closePersistentEditor( _viewHandler->index(idrill, 1, _viewHandler->index(ModelEnums::Root::CHECKS, 0)) );
   _table->openPersistentEditor( _viewHandler->index(idrill, 1, _viewHandler->index(ModelEnums::Root::CHECKS, 0)) );
}

void ProfileChooser::deleteDrillPersistent(const int & idrill)
{
   _table->closePersistentEditor( _viewHandler->index(idrill, 1, _viewHandler->index(ModelEnums::Root::CHECKS, 0)) );
}

void ProfileChooser::remakePers()
{
   for(int i=0;i<_viewHandler->nDrills();i++)
   {
      addDrillPersistent(i);
   }
}

void ProfileChooser::setProfileName(const int & iprofile)
{
   _label->setText(_handler->profileName(iprofile));
   _label->show();
   _table->show();

}

void ProfileChooser::hideDrills()
{
   _label->hide();
   _table->hide();
}

