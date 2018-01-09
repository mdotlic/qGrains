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
#ifndef PROFILECHOOSER_H
#define PROFILECHOOSER_H
#include <QWidget>

class QHBoxLayout;
class ModelNodeBase;
class ViewHandler;
class ProfileHandler;
class QTableView;
class QLineEdit;
class QComboBox;
class Handler;
class ProfileHandler;
class QLabel;

class ProfileChooser : public QWidget
{
   Q_OBJECT
   public:
      ProfileChooser(ModelNodeBase *, Handler *, ViewHandler *, ProfileHandler *);
   private slots:
      void addDrillPersistent(const int &);
      void deleteDrillPersistent(const int &);
      void remakePers();
      void setProfileName(const int &);
      void hideDrills();
   private:
      Handler * _handler;
      ViewHandler * _viewHandler;
      ProfileHandler * _profileHandler;
      QLabel * _label;
      QTableView * _table;
};
#endif
