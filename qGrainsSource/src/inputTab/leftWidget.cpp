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

#include "leftWidget.h"
#include "qGrains.h"
#include "model/handlers/handler.h"
#include "model/handlers/inputHandler.h"
#include "model/modelEnums.h"
#include "model/tables/tables.h"
#include "model/tables/depthHeaderNames.h"
#include "model/tables/coorHeaderNames.h"
#include <QVBoxLayout>
#include <QPushButton>
//#include <QTableView>
#include <QHeaderView>
#include <QLabel>
#include <QApplication>
#include <QClipboard>
#include <QDebug>


LeftWidget::LeftWidget(InputHandler * inputHandler, Handler * handler,
      QGrains * qGrains):_inputHandler(inputHandler), _handler(handler),
   _qGrains(qGrains)
{
   _dHeaderNames = new DepthHeaderNames();
   _cHeaderNames = new CoorHeaderNames();


   QVBoxLayout * layout = new QVBoxLayout;
   _namesTable = new Tables(_inputHandler);
   _namesTable->setMaximumHeight(250);
   //namesTable->scrollToBottom();
   //_namesTable->horizontalHeader()->setStretchLastSection(false);
   //_namesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   
   //_namesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
   _namesTable->setModel(_inputHandler);
   _namesTable->setRootIndex(_inputHandler->index(ModelEnums::Root::DRILLS, 0));
   _namesTable->scrollToBottom();
   //_namesTable->setColumnWidth(0, this->width()/6);
   /*_namesTable->horizontalHeader()->model()->setHeaderData(0, Qt::Horizontal, QVariant("ID"));
   qDebug()<<" sta li ti je "<<_namesTable->horizontalHeader()->model()->setHeaderData(0, Qt::Horizontal, QVariant("ID"))<<"\n";
   qDebug()<<" imas da je "<<_namesTable->horizontalHeader()->model()->headerData(0, Qt::Horizontal)<<"\n";*/
   QLabel * label = new QLabel("    Drills ");
   label->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(label);
   layout->addWidget(_namesTable);

   _depthTable = new Tables(_inputHandler);
   _depthTable->setMaximumHeight(250);
   _depthTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   //_depthTable->horizontalHeader()->setStretchLastSection(true);
   _depthTable->setModel(_inputHandler);
   //_depthTable->horizontalHeader()->hide();
   _dlabel = new QLabel("    Depths ");
   _dlabel->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(_dlabel);
   _dlabel->hide();
   layout->addWidget(_depthTable);
   _depthTable->hide();

   _coorTable = new Tables(_inputHandler);
   _coorTable->setMaximumHeight(250);
   _coorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   //_coorTable->horizontalHeader()->setStretchLastSection(true);
   _coorTable->setModel(_inputHandler);
   //_coorTable->horizontalHeader()->hide();
   _clabel = new QLabel(" Coor ");
   _clabel->setFrameShape(QFrame::StyledPanel);
   layout->addWidget(_clabel);
   layout->addWidget(_coorTable);
   _clabel->hide();
   _coorTable->hide();

   layout->addStretch();
   setLayout(layout);

   connect(_inputHandler, SIGNAL(addedDrill(const QModelIndex &)), this,
         SLOT(newDrill(const QModelIndex &)));
   connect(_inputHandler, SIGNAL(addedDepth(const QModelIndex &)), this,
         SLOT(newDepth(const QModelIndex &)));
   connect(_inputHandler, SIGNAL(addedCoor()), this, SLOT(newCoorHandle()));
   connect(_inputHandler, SIGNAL(coorNotAdded()), this, SLOT(coorNotAdded()));
   connect(_inputHandler, SIGNAL(drillDataAppend(const QModelIndex &)), this,
         SLOT(drillChangePos(const QModelIndex &)));
   connect(_inputHandler, SIGNAL(depthDataAppend(const QModelIndex &)), this,
         SLOT(depthChangePos(const QModelIndex &)));

   connect(_inputHandler, SIGNAL(coorChanged(const QModelIndex &)), this, 
         SLOT(handleCoorSel(const QModelIndex &)));
   connect(_inputHandler, SIGNAL(rowDeleted(const QModelIndex &)), this, 
         SLOT(handleDelete(const QModelIndex &)));

   connect(_namesTable, SIGNAL(selChange(const QModelIndex &)), this,
         SLOT(showDepth(const QModelIndex &)));
   connect(_depthTable, SIGNAL(selChange(const QModelIndex &)), this, 
         SLOT(showCoor(const QModelIndex &)));

   connect(_namesTable, SIGNAL(pasteData(const QModelIndex &)), this, SLOT(pasteNamesTable(const QModelIndex &)));
   connect(_depthTable, SIGNAL(pasteData(const QModelIndex &)), this, SLOT(pasteDepthTable(const QModelIndex &)));
   connect(_coorTable, SIGNAL(pasteData(const QModelIndex &)), this, SLOT(pasteCoorTable(const QModelIndex &)));

}

void LeftWidget::showDepth(const QModelIndex & index)
{
   if(index.row() >= _inputHandler->rowCount(_inputHandler->parent(index))-1)
   {//-1 because of additional row, when additional row is clicked ignore
      _depthTable->hide();
      _dlabel->hide();
      return;
   }
   /*_coorTable->hide();
   _clabel->hide();*/
   
   _depthTable->horizontalHeader()->setModel(_inputHandler);//budzevina da se ne buni
   _depthTable->setRootIndex(index);
   
  _depthTable->horizontalHeader()->setModel(_dHeaderNames);
 
  //QTableWidgetItem* headerItem = _depthTable->horizontalHeaderItem(1);
  //headerItem->setToolTip("[-]");
   
   _depthTable->clearSelection();
   _depthTable->show();
   _depthTable->setCurrentIndex(_inputHandler->index(_inputHandler->rowCount(index)-1, 0, index)); //seting current to last element
   _dlabel->setText(" Samples of the drill "+_inputHandler->name(index));
   _dlabel->show();
}

void LeftWidget::showCoor(const QModelIndex & index)
{//show coordinates
   if(index.row() >= _inputHandler->rowCount(_inputHandler->parent(index))-1)
   {//-1 because of additional row. When additional row is clicked ignore
      _coorTable->hide();
      _clabel->hide();
      return;
   }
   
   _coorTable->horizontalHeader()->setModel(_inputHandler);//budzevina da se ne buni
   QModelIndex index1 = _inputHandler->index(0,0,index);
   _coorTable->setRootIndex(index1);

   _coorTable->horizontalHeader()->setModel(_cHeaderNames);

   _coorTable->show();
  // QModelIndex index1 = _inputHandler->index(0,0,index);  
  _coorTable->setCurrentIndex(_inputHandler->index(_inputHandler->rowCount(index1)-1, 0, index1)); //seting current to last element


  QModelIndex index2 = _inputHandler->index(index.row(),0,_inputHandler->parent(index));
  QModelIndex index3 = _inputHandler->index(index.row(),1,_inputHandler->parent(index));
   _clabel->setText("Particle size/percentage at depth "+QString::number(_inputHandler->data(index2).toDouble(), 'f', 2)+ " - " + QString::number(_inputHandler->data(index3).toDouble(), 'f', 2));
   _clabel->show();
}

void LeftWidget::handleCoorSel(const QModelIndex & index)
{//handle selection when only one number is entered
   int col = index.column()==0 ? 1 : 0;
   QModelIndex newIndex = _coorTable->currentIndex().sibling(index.row(), col);
   _coorTable->setCurrentIndex(newIndex);
}

void LeftWidget::newCoorHandle()
{//handle selection for new row
   _coorTable->scrollToBottom();
   QModelIndex index = _coorTable->currentIndex().sibling(_coorTable->currentIndex().row()+1, 0);
   _coorTable->setCurrentIndex(index);
}

void LeftWidget::coorNotAdded()
{
   _qGrains->error("This point is not added because it has smaller grain size and larger percentage than one of the existing points!!!");
}

void LeftWidget::handleDelete(const QModelIndex & index)
{
   if(index.row()==0)//first row is deleted
   {
      if(index==_namesTable->currentIndex())
      {
         _depthTable->hide();
         _dlabel->hide();
         _coorTable->hide();
         _clabel->hide();

      }
      if(index==_depthTable->currentIndex())
      {
         _coorTable->hide();
         _clabel->hide();
      }
      return;
   }
   if(index.sibling(index.row()-1, index.column()) == 
         _namesTable->currentIndex())
   {//row is deleted in names table, this will set the index for depth table
      showDepth(_namesTable->currentIndex());
   }
   else if(index.sibling(index.row()-1, index.column()) ==
         _depthTable->currentIndex())
   {//row is deleted in depth table, this will set the index for coor table
      showCoor(_depthTable->currentIndex());
   }
 /*  else
      qDebug()<<" brises u coor \n"; */
}

void LeftWidget::drillChangePos(const QModelIndex & index)
{
   int col = index.column()+1;
   QModelIndex newIndex;
   if(col==5)
   {
      col = _inputHandler->unknownValue()+1;
   }
   newIndex = _namesTable->currentIndex().sibling(index.row(), col);
   _namesTable->setCurrentIndex(newIndex);
}

void LeftWidget::depthChangePos(const QModelIndex & index)
{
   int col = index.column()+1;
   QModelIndex newIndex;
   if(col==2)
   {
      col = _inputHandler->unknownDepthValue();
   }
   newIndex = _depthTable->currentIndex().sibling(index.row(), col);
   _depthTable->setCurrentIndex(newIndex);

}

void LeftWidget::newDrill(const QModelIndex & index)
{
   _namesTable->setCurrentIndex(index);
   _namesTable->scrollToBottom();
}

void LeftWidget::newDepth(const QModelIndex & index)
{
   _depthTable->setCurrentIndex(index);
   _depthTable->scrollToBottom();
}

void LeftWidget::pasteNamesTable(const QModelIndex &)
{
   QClipboard * clipboardPtr = QApplication::clipboard();
   const QString clipboardString = clipboardPtr->text();
   
   const QString endRowMark = QString("\n");
   const QString endColMark = QString("\t");
   
   QString rowString;
   std::vector<QString> dataString(5);
   
   // parse the clipboard string and check contents for errors
   const int noOfRows = clipboardString.count(endRowMark);
   for (int i=0;i<noOfRows;i++)
   {
      rowString = clipboardString.section(endRowMark,i,i);
      int noOfCols = rowString.count(endColMark)+1;
      if(noOfCols != 5)
      {
         _qGrains->error("There is no 5 columns!!!");
         return;
      }
      std::vector<double> v(4);
      for (int j=0;j<noOfCols;j++)
      {
         dataString[j] = rowString.section(endColMark,j,j).trimmed();
         if (dataString[j]=="") 
         {
            _qGrains->error("Data in row "+QString::number(i)+" and column "+QString::number(j)+" is missing!");
            return;
         }
         if(j>0)
         {
            bool ok;
            double val= dataString[j].toDouble(&ok);
            if(!ok)
            {
               _qGrains->error("Data in row "+QString::number(i)+" and column "+QString::number(j)+" is not a number!");
               return;
            }
            v[j-1]=val;
         }         
      }
      _inputHandler->loadDrillSlot(v, dataString[0]);
   }
}

void LeftWidget::pasteDepthTable(const QModelIndex & index)
{
   QClipboard * clipboardPtr = QApplication::clipboard();
   const QString clipboardString = clipboardPtr->text();
   
   const QString endRowMark = QString("\n");
   const QString endColMark = QString("\t");
   
   QString rowString;
   std::vector<QString> dataString(2);
   
   // parse the clipboard string and check contents for errors
   const int noOfRows = clipboardString.count(endRowMark);
   for (int i=0;i<noOfRows;i++)
   {
      rowString = clipboardString.section(endRowMark,i,i);
      int noOfCols = rowString.count(endColMark)+1;
      if(noOfCols != 2)
      {
         _qGrains->error("There is no 2 columns!!!");
         return;
      }
      std::vector<double> v(2);
      for (int j=0;j<noOfCols;j++)
      {
         dataString[j] = rowString.section(endColMark,j,j).trimmed();
         if (dataString[j]=="") 
         {
            _qGrains->error("Data in row "+QString::number(i)+" and column "+QString::number(j)+" is missing!");
            return;
         }
         bool ok;
         double val= dataString[j].toDouble(&ok);
         if(!ok)
         {
            _qGrains->error("Data in row "+QString::number(i)+" and column "+QString::number(j)+" is not a number!");
            return;
         }
         v[j]=val;
      }
      _handler->commandDepthAppendLoad(v[0], v[1], index);
   }
}

void LeftWidget::pasteCoorTable(const QModelIndex & index)
{
   QClipboard * clipboardPtr = QApplication::clipboard();
   const QString clipboardString = clipboardPtr->text();
   
   const QString endRowMark = QString("\n");
   const QString endColMark = QString("\t");
   
   QString rowString;
   std::vector<QString> dataString(2);
   
   // parse the clipboard string and check contents for errors
   const int noOfRows = clipboardString.count(endRowMark);
   for (int i=0;i<noOfRows;i++)
   {
      rowString = clipboardString.section(endRowMark,i,i);
      int noOfCols = rowString.count(endColMark)+1;
      if(noOfCols != 2)
      {
         _qGrains->error("There is no 2 columns!!!");
         return;
      }
      double v[2];
      for (int j=0;j<noOfCols;j++)
      {
         dataString[j] = rowString.section(endColMark,j,j).trimmed();
         if (dataString[j]=="") 
         {
            _qGrains->error("Data in row "+QString::number(i)+" and column "+QString::number(j)+" is missing!");
            return;
         }
         bool ok;
         double val= dataString[j].toDouble(&ok);
         if(!ok)
         {
            _qGrains->error("Data in row "+QString::number(i)+" and column "+QString::number(j)+" is not a number!");
            return;
         }
         v[j]=val;
      }
      _handler->commandCoorAppendLoad(v, index);
   }   
}
