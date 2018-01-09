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
#include "surfaceChooser.h"
#include "model/handlers/handler.h"
#include "model/handlers/surfHandler.h"
#include "model/modelEnums.h"
#include "model/tables/surfTable.h"
#include <fstream>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>
#include <QFileDialog>



SurfaceChooser::SurfaceChooser(Handler * handler, SurfHandler * surfHandler):_handler(handler), _surfHandler(surfHandler)
{
   QVBoxLayout * layout = new QVBoxLayout;
   _table = new SurfTable(_surfHandler);
   _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   _table->setModel(_surfHandler);
   _table->setRootIndex(_surfHandler->index(ModelEnums::Root::SURFACES, 0));
   
   layout->addWidget(_table);
   setLayout(layout);
   
   connect(_table, SIGNAL(selChange(const QModelIndex &)), _surfHandler,
         SLOT(selectionChangedSlot(const QModelIndex &)));

   connect(_surfHandler, SIGNAL(newSurf(const QModelIndex &)), this,
         SLOT(newSurfSlot(const QModelIndex &)));

   connect(_surfHandler, SIGNAL(lineSelected(const int &)), this, 
         SLOT(surfSelected(const int &)));
   
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));


}

void SurfaceChooser::newSurfSlot(const QModelIndex & index)
{
   _table->setCurrentIndex(index);
}

void SurfaceChooser::surfSelected(const int & isurf)
{
   QModelIndex surfIndex = _table->currentIndex().sibling(isurf,0);
   _table->setCurrentIndex(surfIndex);
}

void SurfaceChooser::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Export to csv", this, SLOT(exportToCsv()));
   menu->popup(mapToGlobal(pos));
}

void SurfaceChooser::exportToCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save csv"),".",tr("CSV Files (*.csv)"));

   if(!fileName.endsWith(".csv"))
      fileName.append(".csv");

   std::string fileN = fileName.toUtf8().constData();
   std::fstream in;    // Create an output file stream.
   const char* fname=fileN.c_str();

   in.open(fname, std::fstream::out | std::ios::trunc);  // Use it to write to a file.
   if ( !in.is_open() ) {
      QString errMes("Error opening file ");
      errMes.append(fileName);
      errMes.append(" for writing!");

      return;
      //throw errMes;
   }

   for(int i=0; i<_handler->nSurfaces(); i++)
   {
      in<<_handler->surfName(i).toStdString()<<"\n";
      for(int j=0;j<_handler->nSurfPoints(i); j++)
      {    
         in.precision(10);
         in<<_handler->surfPointX(i,j)<<", "<<_handler->surfPointY(i,j)<<", "<<_handler->surfPointZ(i,j)<<"\n";         
      }
   }

}


