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
#ifndef LEFTWIDGET_H
#define LEFTWIDGET_H

#include <QWidget>
class ModelNodeBase;
class InputHandler;
class QTableView;
class QVBoxLayout;
class QLabel;
class QItemSelection;
class Tables;
class DepthHeaderNames;
class CoorHeaderNames;
class Handler;
class QGrains;

class LeftWidget : public QWidget
{
   Q_OBJECT
   public:
      LeftWidget(InputHandler *, Handler *, QGrains *);
      Tables * namesTable(){return _namesTable;}
      Tables * depthTable(){return _depthTable;}
      Tables * coorTable(){return _coorTable;}
      InputHandler * inputHandler() {return _inputHandler;}
   private slots:
      //void scrollDown();
      void showDepth(const QModelIndex &);
      void showCoor(const QModelIndex &);
      void handleCoorSel(const QModelIndex &);
      void newCoorHandle();
      void coorNotAdded();
      void handleDelete(const QModelIndex &);
      void drillChangePos(const QModelIndex &);
      void depthChangePos(const QModelIndex &);
      void newDrill(const QModelIndex &);
      void newDepth(const QModelIndex &);
      void pasteNamesTable(const QModelIndex &);
      void pasteDepthTable(const QModelIndex &);
      void pasteCoorTable(const QModelIndex &);
   private:
      InputHandler * _inputHandler;
      Handler * _handler;
      QGrains * _qGrains;
      QLabel * _dlabel;
      QLabel * _clabel;
      Tables * _namesTable;
      Tables * _depthTable;
      Tables * _coorTable;
      DepthHeaderNames * _dHeaderNames;
      CoorHeaderNames * _cHeaderNames;
};

#endif
