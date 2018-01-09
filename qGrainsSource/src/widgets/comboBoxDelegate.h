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
#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H
 
#include <string>
#include <vector>
 
#include <QStyledItemDelegate>
 
class QModelIndex;
class QWidget;
class QVariant;
 
class ComboBoxDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
  ComboBoxDelegate(int add, QObject *parent = 0);
 
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private slots:
  void valueChanged(int index);
 
private:
  std::vector<int> _color = {Qt::black, Qt::red, Qt::blue, 
     Qt::green, Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
  std::vector<int> _style = {Qt::SolidLine, Qt::DashLine, Qt::DotLine, 
     Qt::DashDotLine, Qt::DashDotDotLine};
  std::vector<QChar> _tick = {QChar(0x2022), 'x', QChar(0x002B), QChar(0x25CB),
     QChar(0x25CF), QChar(0x25A1), QChar(0x25C7), QChar(0x2734), QChar(0x25B3),
     QChar(0x25BD)};
  int _add = 0;

 
};
#endif
