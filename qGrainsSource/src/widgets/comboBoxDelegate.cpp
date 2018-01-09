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

#include "comboBoxDelegate.h"
 
#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>
#include <QLineEdit> 
#include <QPainter>
#include <QDebug>
#include <assert.h>

ComboBoxDelegate::ComboBoxDelegate(int add, QObject *parent)
:QStyledItemDelegate(parent), _add(add)
{
}
 
 
QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index ) const
{
   QComboBox * comboBox = new QComboBox(parent);
   comboBox->setEditable(true);
   // Second : Put the lineEdit in read-only mode
   comboBox->lineEdit()->setReadOnly(true);
   // Third  : Align the lineEdit to right
   comboBox->lineEdit()->setAlignment(Qt::AlignCenter);  
   // Fourth : Align each item in the combo to the right
   if(index.column()==1+_add)
   {
      comboBox->addItem(QIcon(":resources/images/colors/black.png"), QString("black"));
      comboBox->addItem(QIcon(":resources/images/colors/red.png"), QString("red"));
      comboBox->addItem(QIcon(":resources/images/colors/blue.png"), QString("blue"));
      comboBox->addItem(QIcon(":resources/images/colors/green.png"), QString("green"));
      comboBox->addItem(QIcon(":resources/images/colors/yellow.png"), QString("yellow"));
      comboBox->addItem(QIcon(":resources/images/colors/cyan.png"), QString("cyan"));
      comboBox->addItem(QIcon(":resources/images/colors/magenta.png"), QString("magenta"));
      comboBox->addItem(QIcon(":resources/images/colors/gray.png"), QString("gray"));
   }else if(index.column()==2+_add)
   {
      comboBox->addItem("solid");
      comboBox->addItem("dash");
      comboBox->addItem("dot");
      comboBox->addItem("dash-dot");
      comboBox->addItem("dash-dot-dot");
   }else{
      comboBox->addItem(QString(QChar(0x2022)));
      comboBox->addItem("x");
      comboBox->addItem(QString(QChar(0x002B)));
      comboBox->addItem(QString(QChar(0x25CB)));
      comboBox->addItem(QString(QChar(0x25CF)));
      comboBox->addItem(QString(QChar(0x25A1)));
      comboBox->addItem(QString(QChar(0x25C7)));
      comboBox->addItem(QString(QChar(0x2734)));
      comboBox->addItem(QString(QChar(0x25B3)));
      comboBox->addItem(QString(QChar(0x25BD)));
   }

   //comboBox->setCurrentIndex(defColor);//because default is blue
   for(int i = 0; i < comboBox->count(); i++)
      comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);

   connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(valueChanged(int)));

   return comboBox;
  
}

void ComboBoxDelegate::valueChanged(int /*index*/)
{
   commitData(static_cast<QComboBox*> (sender()));
}
 
void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  int value = index.model()->data(index, Qt::DisplayRole).toUInt();
  comboBox->setCurrentIndex(value);
}
 
void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
 //  value = comboBox->currentIndex();
  model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}
 
void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}
 
/*
void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   int value = index.model()->data(index).toInt();
   if(index.column()==1)
   {
      //qDebug()<<" ovo drugo crtam "<<x1<<"; "<<y1<<" --- "<<x2<<"; "<<y2<<"\n";
      QRect rect = option.rect;
      rect.setLeft(rect.left()+6);
      rect.setTop(rect.top()+6);
      rect.setWidth(rect.width()-6);
      rect.setHeight(rect.height()-6);
      
      //qDebug()<<index<<" ---- "<<index.model()<<" crtam te sa "<<value<<"\n";
      painter->fillRect(rect, Qt::GlobalColor(_color[value]));
   }
   if(index.column()==2)
   {
      //qDebug()<<" crtam "<<x1<<"; "<<y1<<" --- "<<x2<<"; "<<y2<<"\n";
     
      int x1 =  option.rect.bottomLeft().x();
      int y1 =  option.rect.bottomLeft().y();
      int x2 =  option.rect.topRight().x();
      int y2 =  option.rect.topRight().y();
      QPen pen;
      pen.setWidth(3);
      pen.setStyle(Qt::PenStyle(_style[value]));
      painter->setPen(pen);
      painter->drawLine(x1+6, (y1+y2)/2, x2-6, (y1+y2)/2);
   }
   if(index.column()==3)
   {
      QPen pen;
      pen.setWidth(3);
      painter->setPen(pen);
      int x1 =  option.rect.bottomLeft().x();
      int y1 =  option.rect.bottomLeft().y();
      int x2 =  option.rect.topRight().x();
      int y2 =  option.rect.topRight().y();
      const qreal size = 32767.0;
      QPointF corner((x1+x2)/2, (y1+y2)/2 - size);
      corner.rx() -= size/2.0;
      corner.ry() += size/2.0;
      Qt::Alignment flags = Qt::AlignHCenter | Qt::AlignVCenter;
      QRectF rect{corner.x(), corner.y(), size, size};      
//   painter.drawText(rect, flags, text, boundingRect);
      painter->drawText(rect, flags, QString(_tick[value]));
   }


//  QStyleOptionViewItem myOption = option;
//  QString text = Items[index.row()].c_str();
 
//  myOption.text = text;
 
//  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}*/
