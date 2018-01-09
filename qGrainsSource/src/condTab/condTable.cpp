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
#include "condTable.h"
#include "model/handlers/condHandler.h"
#include "model/handlers/handler.h"
#include "model/modelNodeBase.h"
#include "calculation/calculation.h"
#include <math.h>
#include <fstream>
#include <QMenu>
#include <QHeaderView>
#include <QFileDialog>
#include <QDebug>

CondTable::CondTable(CondHandler * condHandler, Handler * handler):_condHandler(condHandler), _handler(handler)
{
   setEditTriggers(QAbstractItemView::NoEditTriggers);
   this->setColumnCount(14);
   QStringList headerLabels;
   headerLabels<<"Drill"<<"Sample"<<"Unif. coef."<<"Porosity"<<"Hazen"<<"Slichter"<<"Terzaghi"<<"Beyer"<<"Sauerbrei"<<"Kruger"<<"Kozeny"<<"Zunker"<<"Zamarin"<<"USBR";
   this->setHorizontalHeaderLabels(headerLabels);
   
   QTableWidgetItem* headerItem = horizontalHeaderItem(2);
   headerItem->setToolTip("[-]");
   headerItem = horizontalHeaderItem(3);
   headerItem->setToolTip("[-]");
   for(int i=4;i<14;i++)
   {
      headerItem = horizontalHeaderItem(i);
      headerItem->setToolTip("[m/s]");
   }

   for(int i=2;i<14;i++)
      this->setColumnHidden(i, true);
   
   QHeaderView* header = this->horizontalHeader();
   header->setSectionResizeMode(QHeaderView::Interactive);
   connect(condHandler, SIGNAL(changeRow(const int &, const int &)), this, 
         SLOT(changeRowSlot(const int &, const int &)));
   connect(condHandler->model(), SIGNAL(loadCondTableTrue(const int &, const int &)), this, SLOT(changeRowSlot(const int &, const int &)));

   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));

}

void CondTable::showHideCol(int id, bool isChecked)
{
   this->setColumnHidden(id, !isChecked);
}

void CondTable::changeRowSlot(const int & idrill, const int & jsample)
{
   int rows = rowCount();
   for(int i=0; i<rows; i++)
   {
      if(item(i, 0)->text() == _handler->drillName(idrill) &&
            item(i, 1)->text() == _handler->sampleName(idrill, jsample))
      {
         removeRow(i);
         break;
      }
   }
   if(_condHandler->isSampleChecked(idrill, jsample))
   {
      this->insertRow(rowCount());
      setItem(rowCount()-1, 0, new QTableWidgetItem(_handler->drillName(idrill)));
      setItem(rowCount()-1, 1, new QTableWidgetItem(_handler->sampleName(idrill, jsample)));
      calculate(idrill, jsample, rowCount()-1);
   }
   /*else
   {
      
   }*/

//   this->setItem(rowCount(), 0, new QTableWidgetItem(_condHandler->name()))
}

void CondTable::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Export value to csv", this, SLOT(exportValToCsv()));
   menu->addAction("Export color to csv", this, SLOT(exportColToCsv()));
   menu->popup(mapToGlobal(pos));

}

void CondTable::exportColToCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save csv file"),".",tr("CSV Files (*.csv)"));

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
   }

   int rows = rowCount();
   int cols = columnCount();
   in<<"Drill, Sample, Unif. coef., Porosity, Hazen, Slichter, Terzaghi, Beyer, Sauerbrei, Kruger, Kozeny, Zunker, Zamarin, USBR\n";
   for(int i=0; i<rows; i++)
   {
      for(int j=0;j<cols; j++)
      {       
         if(j<2 || i==0)
            in<<item(i,j)->text().toStdString()<<", ";         
         else
         {
            if(item(i,j)->background().color().red()==255)
               in<<"1, ";
            else
               in<<"0, ";
         }
      }
      in<<"\n";

   }

}
void CondTable::exportValToCsv()
{
    QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save csv file"),".",tr("CSV Files (*.csv)"));

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
      throw errMes;
   }

   int rows = rowCount();
   int cols = columnCount();
   in<<"Drill, Sample, Unif. coef., Porosity, Hazen, Slichter, Terzaghi, Beyer, Sauerbrei, Kruger, Kozeny, Zunker, Zamarin, USBR\n";
   for(int i=0; i<rows; i++)
   {
      for(int j=0;j<cols; j++)
      {        
         in<<item(i,j)->text().toStdString()<<", ";         
      }
      in<<"\n";

   }

}



void CondTable::calculate(const int & idrill, const int & jsample, 
      const int & irow)
{
   bool correct=false;
   setItem(irow, 2, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 6, -1, -1, correct)));
   setItem(irow, 3, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 7, -1, -1, correct)));

   correct = false;
   setItem(irow, 4, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 8, -1, -1, correct)));
   if(!correct)
      item(irow,4)->setBackground(Qt::red);
   
   correct=false;
   setItem(irow, 5, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 9, -1, -1, correct)));
   if(!correct)
      item(irow,5)->setBackground(Qt::red);
   
   correct=false;
   setItem(irow, 6, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 10, -1, -1, correct)));
   if(!correct)
      item(irow,6)->setBackground(Qt::red);

   correct=false;
   setItem(irow, 7, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 11, -1, -1, correct)));
   if(!correct)
      item(irow,7)->setBackground(Qt::red);

   correct=false;
   setItem(irow, 8, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 12, -1, -1, correct)));
   if(!correct)
      item(irow,8)->setBackground(Qt::red);

   correct=false;
   setItem(irow, 9, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 13, -1, -1, correct)));
   if(!correct)
      item(irow,9)->setBackground(Qt::red);
   
   correct=false;
   setItem(irow, 10, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 14, -1, -1, correct)));
   if(!correct)
      item(irow,10)->setBackground(Qt::red);

   correct=false;
   setItem(irow, 11, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 15, -1, -1, correct)));
   if(!correct)
      item(irow,11)->setBackground(Qt::red);

   correct=false;
   setItem(irow, 12, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 16, -1, -1, correct)));
   if(!correct)
      item(irow,12)->setBackground(Qt::red);

   correct=false;
   setItem(irow, 13, new QTableWidgetItem(
       Calculation::calcValue(idrill, jsample, _handler, 17, -1, -1, correct)));
   if(!correct)
      item(irow,13)->setBackground(Qt::red);

}

double CondTable::findDe(const int & idrill, const int & jsample, 
      const double & dePer)
{
   int npoints = _condHandler->npoints(idrill, jsample);
   for(int i=0; i<npoints; i++)
   {
      if(_condHandler->pointValue(idrill, jsample, i, 1) > dePer)//1 is for percent
      {
         if(i==0)
         {
            return _condHandler->pointValue(idrill, jsample, i, 0);
         }else
         {
            double lowPer = _condHandler->pointValue(idrill, jsample, i-1, 1);
            double highPer = _condHandler->pointValue(idrill, jsample, i, 1);
            double lowVal = _condHandler->pointValue(idrill, jsample, i-1, 0);
            double highVal = _condHandler->pointValue(idrill, jsample, i, 0);
            double razl = highPer-lowPer;
            return (highPer-dePer)*lowVal/razl + (dePer-lowPer)*highVal/razl;
         }
      }
   }
   return -1.0;
}
