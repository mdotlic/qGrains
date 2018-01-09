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
#ifndef CHOOSERWIDGET_H
#define CHOOSERWIDGET_H

#include <QWidget>

class ModelNodeBase;
//class QVBoxLayout;
class DChooser;
class DrillChooser;

class ChooserWidget : public QWidget
{
   Q_OBJECT
   public:
      ChooserWidget(ModelNodeBase *);
      DChooser * dChooser(){return _dChooser;}
      DrillChooser * drillChooser(){return _drillChooser;}
   private:
      DChooser * _dChooser;
      DrillChooser * _drillChooser;
};

#endif
