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
#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMenuBar>
#include <fstream>
class ModelNodeBase;
class QGrains;

class MainMenu : public QMenuBar
{
   Q_OBJECT
   public:
      MainMenu(QGrains * qGrains);
      void loadModel(const QString &, const bool & onlyDrills = false);
      void setCurrentFile(const QString &);
   public slots:
      void quit();
   private slots:
      void save();
      void saveAs();
      void newFile();
      void open();
      void openFromCsv();
      void addDrills();
      void toleranceDialog();
      void parametersDialog();
      void showHelp();
      void showAbout();
   private:
      void saveFile(const QString &);
      void readCsv(const QString &);
      void traverseTree(ModelNodeBase *, int treeDepth, std::fstream & in);
      void createActions();
      void createMenus();
      bool maybeSave();

      QString _currentFile;
      QMenu * _fileMenu;
      QMenu * _settingsMenu;
      QMenu * _helpMenu;
      QAction * _newAct;
      QAction * _saveAct;
      QAction * _saveAsAct;
      QAction * _openAct;
      QAction * _addDrillsAct;
      QAction * _readCsvAct;
      QAction * _quitAct;
      QAction * _toleranceAct;
      QAction * _parametersAct;
      QAction * _helpAct;
      QAction * _aboutAct;

      QGrains * _qGrains;
};

#endif
