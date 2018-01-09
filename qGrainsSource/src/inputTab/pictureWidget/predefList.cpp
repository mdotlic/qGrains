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
#include "predefList.h"

#include <QKeyEvent>
#include <QDebug>

PredefList::PredefList()
{
   connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(listItemClick(QListWidgetItem *)));

}

void PredefList::keyPressEvent(QKeyEvent * event)
{
   if(event->key() == Qt::Key_Delete)
   {
      //qDebug()<<" kliknuo na delete "<<_clicked<<"\n";
      delete this->takeItem(_clicked);
//      emit listChanged();
   }
   else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
   {
      if(currentRow()==0)
      {
         //double ddd = currentItem()->text().toDouble();
         closePersistentEditor(currentItem());
         bool ok;
         double ddd = currentItem()->text().toDouble(&ok);
         delete currentItem();
         if(ok && ddd>=0 && ddd<=100.0)
         {
//            currentItem()->setData(Qt::DisplayRole, ddd);
//            qDebug()<<" postavio sam ga na "<<ddd<<"\n";
            QListWidgetItem *item = new QListWidgetItem;
            item->setData(Qt::DisplayRole, ddd);
            addItem(item);
         }
  //       else
    //        delete currentItem();
         QListWidgetItem * itemEdit = new QListWidgetItem;
         addItem(itemEdit);
         itemEdit->setFlags (itemEdit->flags () | Qt::ItemIsEditable);
         openPersistentEditor(itemEdit);
         setCurrentItem(itemEdit);
      }
   }
   //qDebug()<<" key event \n";
}

void PredefList::listItemClick(QListWidgetItem * item)
{
   _clicked=this->row(item);
  // this->openPersistentEditor(item);
  /* for(int i=0;i<this->count();i++)
   {
      if(i!=_clicked)
         this->closePersistentEditor(this->item(i));
   }*/
   //qDebug()<<" kliknuo "<<item->text()<<"\n";
}

