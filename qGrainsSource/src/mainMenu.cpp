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
#include "mainMenu.h"
#include "qGrains.h"
#include "model/modelNodeBase.h"
#include "model/pointSet.h"
#include "model/intPointSet.h"
#include "model/modelEnums.h"
#include "dialogs/about.h"
#include "centralWindow.h"
#include "inputTab/inputTab.h"
#include "inputTab/leftWidget.h"
#include "depthTab/depthTab.h"
#include "depthTab/depthDTab/depthDTab.h"
#include "depthTab/depthDTab/chooserWidget.h"
#include "depthTab/depthDTab/dChooser/dChooser.h"
#include "depthTab/percTab/percTab.h"
#include "depthTab/percTab/percChooser.h"
#include "depthTab/percTab/sizeChooser/sizeChooser.h"
#include "model/handlers/dHandler.h"
#include "model/handlers/sizeHandler.h"
#include "model/handlers/handler.h"
#include <iostream>
#include <QDebug>
#include <QFileDialog>
#include <QStatusBar>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>


MainMenu::MainMenu(QGrains * qGrains):_qGrains(qGrains)
{
   createActions();
   createMenus();
}

void MainMenu::loadModel(const QString & fileN, const bool & onlyDrills)
{
   if(fileN.right(5)!=".qgra")
   {
      QString errMes("File ");
      errMes.append(fileN);
      errMes.append(" is not .qgra file!!!");
      throw errMes;
   }

   std::string fileName = fileN.toUtf8().constData();
   char s;

   std::ifstream in;    // Create an input file stream.
   in.open(fileName.c_str(),std::ifstream::in );  // Use it to read from a file.
   if ( ! in ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for reading!");
      throw errMes;
   } 
   std::string str;
   int nodeId = ModelEnums::Root::DRILLS; 
   int id = 0;
   int idrill = 0;
   int isample = 0;
   while(in.peek()!=EOF)
   {
      int treeDepth = 0;
      while(in.peek()==',')
      {
         treeDepth++;
         in>>s;//get coma
      }
      if(treeDepth == 1)
      {//drills or dChecked
         if(in.peek() == 'D')//Drills
         {
            std::string name;
            getline(in, name, ',');
            double v;//value of tolerance
            in>>v;
            _qGrains->centralWindow()->handler()->setTolerance(v);
            if(in.peek()==',')
            {
               in>>s;
               in>>v;
               _qGrains->centralWindow()->handler()->setTemp(v);
               in>>s;
               in>>v;
               _qGrains->centralWindow()->handler()->setVisc(v);
               in>>s;
               in>>v;
               _qGrains->centralWindow()->handler()->setTerc(v);
               in>>s;
               in>>v;
               _qGrains->centralWindow()->handler()->setCunk(v);
            }

            nodeId = ModelEnums::Root::DRILLS;
         }
         else
         {
            in>>str;
            if(str.find("Checked")!=std::string::npos)
            {
               nodeId = ModelEnums::Root::DCHECKED;
               std::string str;
               getline(in, str);
               if(str.find(',') != std::string::npos)
               {//used from version 1.35, older qgra would just ignore this
                  std::string s= str.substr(str.find(',')+1);
                  _qGrains->centralWindow()->handler()->setUseDElev(std::stoi(s));                  
               }
               continue;
            }else if(str.find("Checks") != std::string::npos)
            {
               nodeId = ModelEnums::Root::CHECKS; 
               continue; 
               if(in.peek() == ',')
               {//used from version 1.35, older qgra would just ignore this
                  std::string name;
                  getline(in, name, ',');
                  in>>s;
                  _qGrains->centralWindow()->handler()->setUsePlotElev(s);                  
               }
            }else if(str.find("Size") !=std::string::npos)
            {
               nodeId = ModelEnums::Root::PERCCHECK;
               std::string str;
               getline(in, str);
               if(str.find(',') != std::string::npos)
               {//used from version 1.35, older qgra would just ignore this
                  std::string s= str.substr(str.find(',')+1);
                  _qGrains->centralWindow()->handler()->setUsePercElev(std::stoi(s));               
               }
               continue;
            }else if(str.find("CondProp")!=std::string::npos)
            {
               char *cstr = new char[str.length() + 1];
               strcpy(cstr, str.c_str());
               char * novi = strtok(cstr, ",");
               novi = strtok (NULL, ",");
               int i=0;
               while (novi != NULL)
               {
                  _qGrains->centralWindow()->handler()->setCond(i, std::string(novi)=="1");

                  novi = strtok (NULL, ",");
                  i++;
               }
               nodeId = ModelEnums::Root::CONDUCTIVITY;
               delete cstr;
               delete novi;
            }else if(str.find("Surfaces") !=std::string::npos)
            {
               nodeId = ModelEnums::Root::SURFACES;
            }
            else if(str.find("Profile") != std::string::npos)
            {
               std::string name;//Tab
               getline(in, name, ',');
               int type;
               double from, to;
               in>>type>>s>>from>>s>>to;
               _qGrains->centralWindow()->handler()->setProfileType(
                     type, from, to);
               nodeId = ModelEnums::Root::PROFILETAB;
            }
            else if(str.find("Plan") != std::string::npos)
            {
               std::string name;//Tab
               getline(in, name, ',');

               int planType, surfaceId, surfaceFrom, surfaceTo, calcType;
               double planFrom, planTo, planFromSize, planToSize;
               in>>planType>>s>>planFrom>>s>>planTo>>s>>surfaceId>>s>>surfaceFrom>>s>>surfaceTo>>s>>calcType>>s>>planFromSize>>s>>planToSize;
               _qGrains->centralWindow()->handler()->setPlanType(planType,
                     planFrom,  planTo, surfaceId, surfaceFrom, surfaceTo,
                     calcType, planFromSize, planToSize);
               nodeId = ModelEnums::Root::PLAN;
            }
         }

      }
      if(treeDepth == 2)
      {//drill
         if(nodeId != ModelEnums::Root::DRILLS && onlyDrills)
         {
            getline(in,str);
            continue;
         }
         if(nodeId == ModelEnums::Root::DRILLS)
         {
            std::string name;
            std::vector<double> v(4);
            getline(in, name, ',');
            in>>v[0];
            in>>s;
            in>>v[1];
            in>>s;
            in>>v[2];
            in>>s;
            in>>v[3];
            _qGrains->centralWindow()->handler()->commandDrillAppendLoad(v, QString::fromStdString(name));
         }
         else if (nodeId == ModelEnums::Root::DCHECKED && !onlyDrills)
         {
            std::string name;
            std::vector<double> v(3);
            getline(in, name, ',');
            in>>v[0];
            in>>s;
            in>>v[1];
            in>>s;
            in>>v[2];
            _qGrains->centralWindow()->depthTab()->depthDTab()->chooserWidget()->dChooser()->dHandler()->setDLoad(v, QString::fromStdString(name), id);
            id++;
         }
         else if(nodeId == ModelEnums::Root::PERCCHECK && !onlyDrills)
         {
            std::string name;
            std::vector<double> v(6);
            getline(in, name, ',');
            in>>v[0];
            in>>s;
            in>>v[1];
            in>>s;
            in>>v[2];
            in>>s;
            in>>v[3];
            in>>s;
            in>>v[4];
            _qGrains->centralWindow()->depthTab()->percTab()->percChooser()->sizeChooser()->sizeHandler()->commandSizeAppendLoad(v);
         }
         else if(nodeId == ModelEnums::Root::CHECKS && !onlyDrills)
         {
            std::vector<int> vec(7);
            std::string name;
            getline(in, name, ',');
            in>>vec[0]>>s>>vec[1]>>s>>vec[2]>>s>>vec[3]>>s>>vec[4]>>s>>
               vec[5]>>s>>vec[6];

            _qGrains->centralWindow()->handler()->drillSetChecks(idrill, vec);
            idrill++;
            isample = 0;
         }
         else if(nodeId == ModelEnums::Root::PROFILETAB && !onlyDrills)
         {
            std::string name;
            getline(in, name, ',');
            if(name=="Profiles")
            {
               int active;
               in>>active;
               _qGrains->centralWindow()->handler()->setActiveProf(active);
               _qGrains->centralWindow()->handler()->loadDelDefaultProf();
            }
            else
            {
               std::vector<double> v(9);
               in>>v[0]>>s>>v[1]>>s>>v[2]>>s>>v[3]>>s>>v[4]>>s>>
                  v[5]>>s>>v[6]>>s>>v[7]>>s>>v[8];
               _qGrains->centralWindow()->handler()->profileSetMap(v, 
                     QString::fromStdString(name));
            }
         }
         else if(nodeId == ModelEnums::Root::SURFACES && !onlyDrills)
         {
            std::string name;
            getline(in, name, ',');
            bool surfCheck=false;
            in>>surfCheck;
            _qGrains->centralWindow()->handler()->loadSurface(surfCheck,
                  QString::fromStdString(name));
         }
         else if(nodeId == ModelEnums::Root::PLAN && !onlyDrills)
         {
            std::string name;
            getline(in, name, ',');
            double xmin, xmax, ymin, ymax;
            int xdiv, ydiv;
            in>>xmin>>s>>xmax>>s>>ymin>>s>>ymax>>s>>xdiv>>s>>ydiv;
            _qGrains->centralWindow()->handler()->setIsoXmin(xmin);
            _qGrains->centralWindow()->handler()->setIsoXmax(xmax);
            _qGrains->centralWindow()->handler()->setIsoYmin(ymin);
            _qGrains->centralWindow()->handler()->setIsoYmax(ymax);
            _qGrains->centralWindow()->handler()->setIsoXdiv(xdiv);
            _qGrains->centralWindow()->handler()->setIsoYdiv(ydiv);
         }
         else 
         {
            qDebug()<<" Something is wrong in file reading !!! \n";
         }
      }
      else if(treeDepth == 3)
      {
         if(nodeId == ModelEnums::Root::DRILLS)
         {
            std::string name;
            double v0, v1;
            getline(in, name, ',');
            in>>v0;
            in>>s;
            in>>v1;
            _qGrains->centralWindow()->handler()->commandDepthAppendLoad(v0,v1);
         }else if(nodeId == ModelEnums::Root::CHECKS && !onlyDrills)
         {
            std::string name;
            bool check, condCheck;
            int tmp, tmp1;
            getline(in, name, ',');
            in>>check;
            in>>s;
            in>>tmp;
            in>>s;
            in>>tmp1; 
            in>>s;
            in>>condCheck;
            _qGrains->centralWindow()->handler()->sampleSetChecks(isample,
                  idrill-1, check, tmp, tmp1, condCheck);
            isample++;
         }
         else if(nodeId == ModelEnums::Root::PROFILETAB && !onlyDrills)
         {
            std::string name;
            getline(in, name);
            in.putback('\n');
            _qGrains->centralWindow()->handler()->addNewProfile(QString::fromStdString(name));
         }
      }
      else if(treeDepth == 4)
      {
         if(nodeId == ModelEnums::Root::SURFACES && !onlyDrills)
         {
            double pnt[4];
            in>>pnt[0]>>s>>pnt[1]>>s>>pnt[2]>>s>>pnt[3];
            _qGrains->centralWindow()->handler()->surfacePointLoad(pnt);
         }
         else if(nodeId == ModelEnums::Root::PLAN && !onlyDrills)
         {
            while(true)
            {
               if(in.peek()==',' || in.peek()=='\n' || in.peek()== '\r' || 
                     (in.peek()==EOF && in.eof()))
               {
                  break;
               }
               double ppp;
               in>>ppp>>s;
               _qGrains->centralWindow()->handler()->addIsoValue(ppp);
            }
         }
      }
      else if(treeDepth == 5)
      {
         if(nodeId == ModelEnums::Root::PROFILETAB && !onlyDrills)
         {
            while(true)
            {
               if(in.peek()==',' || in.peek()=='\n' || in.peek()=='\r')
               {
                  break;
               }
               int ppp;
               in>>ppp>>s;
               _qGrains->centralWindow()->handler()->addProfilePoint(ppp);
            }
         }
         else
         {
            double pnt[2];         
            in>>pnt[0];
            in>>s;
            in>>pnt[1];
            _qGrains->centralWindow()->handler()->commandCoorAppendLoad(pnt);
         }
      }

      getline(in, str);
   }
   if(!onlyDrills)
      emit _qGrains->model()->loadFinished();
   _qGrains->setModified(true);
}

void MainMenu::setCurrentFile(const QString &fileName)
{
   _currentFile = fileName;

   QString shownName = _currentFile;
   if (_currentFile.isEmpty()) 
      shownName = "untitled.qgra";
   _qGrains->setWindowTitle(_qGrains->title()+"  "+shownName);
}


void MainMenu::quit()
{
   if(maybeSave())
   {
      QApplication::quit();
   }
}

void MainMenu::save()
{
   if (_currentFile.isEmpty()) 
      saveAs();
   else 
      saveFile(_currentFile);
}

void MainMenu::saveAs()
{
   QString str=".";
   if(_currentFile!="")
      str=QFileInfo(_currentFile).absoluteDir().absolutePath();
   
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Save model"),str,tr("Model Files (*.qgra)"));
   if (fileName.isEmpty()) 
   {
      qDebug()<<" must have save name ";
      return;
   }
   if(!fileName.endsWith(".qgra"))
      fileName.append(".qgra");
   return saveFile(fileName);
}

void MainMenu::newFile()
{
   if (maybeSave()) 
   {
      _qGrains->clear();
      _qGrains->initialize();
      _currentFile.clear();
      _qGrains->setWindowTitle(_qGrains->title());
   }
}

void MainMenu::open()
{
   if (maybeSave()) 
   {
      QString str=".";
      if(_currentFile!="")
         str=QFileInfo(_currentFile).absoluteDir().absolutePath();
      QString fileName = QFileDialog::getOpenFileName(this, tr("Open"),str,
            tr("Model Files (*.qgra)"));
      if (!fileName.isEmpty()) 
      {
         _qGrains->clear();
         _qGrains->initialize();
         try{
            loadModel(fileName);
         }
         catch(const QString & err)
         {
            _qGrains->error(err);
         }
         //emit _qGrains->centralWindow()->depthTab()->depthDTab()->chooserWidget()->dChooser()->dHandler()->loadFinished();
         setCurrentFile(fileName);
      }
   }
}

void MainMenu::openFromCsv()
{
   if (maybeSave()) 
   {
      QString str=".";
      if(_currentFile!="")
         str=QFileInfo(_currentFile).absoluteDir().absolutePath();
      QString fileName = QFileDialog::getOpenFileName(this, tr("Open"),str,
            tr("Model Files (*.csv)"));
      if (!fileName.isEmpty()) 
      {
         _qGrains->clear();
         _qGrains->initialize();
         try{
            readCsv(fileName);
         }
         catch(const QString & err)
         {
            _qGrains->error(err);
         }
        // setCurrentFile(fileName);
      }
   }
}

void MainMenu::addDrills()
{
   QString fileName = QFileDialog::getOpenFileName(this, tr("Add drills from file"));
   if (!fileName.isEmpty()) 
      loadModel(fileName, true);
   //emit _newCode->centralWindow()->drillsTab()->chooserWidget()->dChooser()->dHandler()->loadFinished();

}

void MainMenu::toleranceDialog()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set tolerance");
   
   QGridLayout * layout = new QGridLayout;
   QLabel * tolLabel = new QLabel("Tolerance:");
   layout->addWidget(tolLabel,0,0,1,1);
   QLineEdit * tolLineEdit = new QLineEdit(dialog);
   tolLineEdit->setText(QString::number(_qGrains->model()->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::EPS].toDouble()));
   layout->addWidget(tolLineEdit,0,1,1,1);
  
   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,1,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      _qGrains->model()->child(ModelEnums::Root::DRILLS)->setProperty(tolLineEdit->text().toDouble(), ModelEnums::Drills::EPS);
   }
}

void MainMenu::parametersDialog()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set parameters for conductivity");
   
   QGridLayout * layout = new QGridLayout;
   QLabel * tempLabel = new QLabel("Temperature:");
   layout->addWidget(tempLabel,0,0,1,1);
   QLineEdit * tempLineEdit = new QLineEdit(dialog);
   tempLineEdit->setText(QString::number(_qGrains->model()->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::TEMP].toDouble()));
   layout->addWidget(tempLineEdit,0,1,1,1);
      
   QLabel * tercLabel = new QLabel("Terzaghi coef:");
   layout->addWidget(tercLabel,1,0,1,1);
   QLineEdit * tercLineEdit = new QLineEdit(dialog);
   tercLineEdit->setText(QString::number(_qGrains->model()->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::TERC].toDouble()));
   layout->addWidget(tercLineEdit,1,1,1,1);

   QLabel * cunkLabel = new QLabel("Zunker coef:");
   layout->addWidget(cunkLabel,2,0,1,1);
   QLineEdit * cunkLineEdit = new QLineEdit(dialog);
   cunkLineEdit->setText(QString::number(_qGrains->model()->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::CUNK].toDouble()));
   layout->addWidget(cunkLineEdit,2,1,1,1);

  
   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,3,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      double t=tempLineEdit->text().toDouble();
      double visc = 3.0277e-14*t*t*t*t-8.731e-12*t*t*t+9.7155e-10*t*t-5.5088e-8*t+1.7766e-6;
      _qGrains->model()->child(ModelEnums::Root::DRILLS)->setProperty(tempLineEdit->text().toDouble(), ModelEnums::Drills::TEMP);
      _qGrains->model()->child(ModelEnums::Root::DRILLS)->setProperty(visc*1e+6,
            ModelEnums::Drills::VISC);
      _qGrains->model()->child(ModelEnums::Root::DRILLS)->setProperty(tercLineEdit->text().toDouble(), ModelEnums::Drills::TERC);
      _qGrains->model()->child(ModelEnums::Root::DRILLS)->setProperty(cunkLineEdit->text().toDouble(), ModelEnums::Drills::CUNK);
   }
}

void MainMenu::showHelp()
{
   QDesktopServices::openUrl(QUrl::fromLocalFile("qGrains.pdf"));
//   QDesktopServices::openUrl(QUrl("file://"+QCoreApplication::applicationDirPath()+"/qGrains.pdf"));
}

void MainMenu::showAbout()
{
   About about(_qGrains);
}

void MainMenu::saveFile(const QString & fileN)
{
   std::string fileName = fileN.toUtf8().constData();
   std::fstream in;    // Create an output file stream.
   const char* fname=fileName.c_str();

   in.open(fname, std::fstream::out | std::ios::trunc);  // Use it to write to a file.
   if ( !in.is_open() ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for writing!");
      throw errMes;
   }
   in.precision(11);

   traverseTree(_qGrains->model(), 0, in);

   in.close();

   setCurrentFile(fileN);
   _qGrains->statusBar()->showMessage(tr("File saved"), 2000);
   _qGrains->setModified(false);
}

void MainMenu::readCsv(const QString & fileN)
{
  std::string fileName = fileN.toUtf8().constData();
  char s;

  std::ifstream in;    // Create an input file stream.
  in.open(fileName.c_str(),std::ifstream::in );  // Use it to read from a file.
  if ( ! in ) {
     QString errMes("Error opening file ");
     errMes.append(fileN);
     errMes.append(" for reading!");
     throw errMes;
  } 
  std::string str;
  getline(in, str);
  while(in.peek()!=EOF)
  {
     double d1, d2, val, val2, x, y;
     std::string name, type;
     QString qtype;
     getline(in, name, ',');
     getline(in, type, ',');
     if(type[0]!='d')
     {
        getline(in,str);
        continue;
     }
     qtype = QString::fromStdString(type);
     qtype.remove(0,1);
     bool ok;
     val2 = qtype.toDouble(&ok);
     if(!ok)
     {
        qDebug()<<"something is wrong with type !!!\n";
        getline(in,str);
        continue;
     }
     in>>d1>>s>>d2>>s>>val>>s>>x>>s>>y;
     getline(in, str);
     _qGrains->centralWindow()->handler()->pointFromCsv(
           QString::fromStdString(name), val2, d1, d2,
           val, x, y);
  }
}

void MainMenu::traverseTree(ModelNodeBase * node, int treeDepth, std::fstream & in)
{
   for(int i=0;i<treeDepth;i++)
      in<<",";
   in<<node->name().toStdString();
   int nrow = node->rowCount();
   int ncol = node->columnCount();
   for(int i=0;i<ncol;i++)
   {
      in<<","<<node->properties()[i].toDouble();
   }
   in<<"\n";
   if(treeDepth == 4)
   {
      if(node->name()=="array Drill Ids")
      {
         IntPointSet * points = static_cast<IntPointSet*> (node);
         int nPoints = points->rowCount();
         for(int i=0;i<treeDepth+1;i++)
               in<<",";
         for(int ip=0;ip<nPoints;ip++)
         {
            in<<points->data(ip, 0).toInt()<<",";
         }
         in<<"\n";
      }else{
         PointSet * points = static_cast<PointSet*> (node);
         int nPoints = points->rowCount();
         for(int ip=0;ip<nPoints;ip++)
         {
            for(int i=0;i<treeDepth+1;i++)
               in<<",";
            in<<points->data(ip, 0).toDouble()<<","<<points->data(ip, 1).toDouble()<<"\n";
         }
      }
   }
   else if(treeDepth==3 && node->name()=="Surface points")
   {
      PointSet * points = static_cast<PointSet*> (node); 
      int nPoints = points->rowCount();
      for(int ip=0;ip<nPoints;ip++)
      {
         for(int i=0;i<treeDepth+1;i++)
            in<<",";  
         in<<points->data(ip, 0).toDouble()<<","<<points->data(ip, 1).toDouble()<<","<<points->data(ip, 2).toDouble()<<","<<points->data(ip, 3).toInt()<<"\n";
      }
   }
   else if(treeDepth==3 && node->name()=="isoline value")
   {
      PointSet * points = static_cast<PointSet*> (node); 
      int nPoints = points->rowCount();
      for(int i=0;i<treeDepth+1;i++)
         in<<","; 
      for(int ip=0;ip<nPoints;ip++)
      {
         in<<points->data(ip, 0).toDouble()<<",";
      }
   }
   else
   {
      for(int j=0;j<nrow;j++)
      {
         traverseTree(node->child(j), treeDepth+1, in);
      }
   }
}

void MainMenu::createActions()
{
   _newAct = new QAction(QIcon::fromTheme("document-new",QIcon(":/resources/images/folder.png")), tr("&New"), this);
   _newAct->setShortcuts(QKeySequence::New);
   _newAct->setStatusTip(tr("Create a new file"));
   connect(_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

   _openAct = new QAction(QIcon::fromTheme("document-open", QIcon(":/resources/images/new_page.png")), tr("&Open..."), this);
   _openAct->setShortcuts(QKeySequence::Open);
   _openAct->setStatusTip(tr("Open an existing file"));
   connect(_openAct, SIGNAL(triggered()), this, SLOT(open()));

   _readCsvAct = new QAction(tr("&Open from csv..."), this);
   _readCsvAct->setStatusTip(tr("add drills from csv file"));
   connect(_readCsvAct, SIGNAL(triggered()), this, SLOT(openFromCsv()));

   _addDrillsAct = new QAction(tr("&Add..."), this);
   _addDrillsAct->setShortcut(Qt::Key_A | Qt::CTRL);
   _addDrillsAct->setStatusTip(tr("add drills form file"));
   connect(_addDrillsAct, SIGNAL(triggered()), this, SLOT(addDrills()));

   _saveAct = new QAction(QIcon::fromTheme("document-save",QIcon(":/resources/images/save.png")), tr("&Save..."), this);
   _saveAct->setShortcuts(QKeySequence::Save);
   _saveAct->setStatusTip(tr("Save to a .wjci file"));
   connect(_saveAct, SIGNAL(triggered()), this, SLOT(save()));
     
   _saveAsAct = new QAction(QIcon::fromTheme("document-save-as"), tr("&Save As..."), this);
   _saveAsAct->setShortcuts(QKeySequence::SaveAs);
   _saveAsAct->setStatusTip(tr("Save as"));
   connect(_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

   _quitAct = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit..."), this);
   _quitAct->setShortcut(Qt::Key_Q | Qt::CTRL);
   _quitAct->setStatusTip(tr("Quit qGrains"));
   connect(_quitAct, SIGNAL(triggered()), this, SLOT(quit()));

   _toleranceAct = new QAction(tr("Tolerance"), this);
   connect(_toleranceAct, SIGNAL(triggered()), this, SLOT(toleranceDialog()));
   
   _parametersAct = new QAction(tr("Parameters"), this);
   connect(_parametersAct, SIGNAL(triggered()), this, SLOT(parametersDialog()));

   _helpAct = new QAction(QIcon::fromTheme("help-contents",QIcon(":/resources/images/help.png")), tr("qGrains help"), this);
   connect(_helpAct, SIGNAL(triggered()), this, SLOT(showHelp()));
   
   _aboutAct = new QAction(QIcon::fromTheme("help-about",QIcon(":/resources/images/info.png")), tr("About"), this);
   connect(_aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
}

void MainMenu::createMenus()
{
   _fileMenu = addMenu(tr("File"));
   _fileMenu->addAction(_newAct);
   _fileMenu->addAction(_openAct);
   _fileMenu->addAction(_readCsvAct);
   _fileMenu->addAction(_addDrillsAct);
   _fileMenu->addAction(_saveAct);
   _fileMenu->addAction(_saveAsAct);
   _fileMenu->addSeparator();
   _fileMenu->addAction(_quitAct);
   _settingsMenu = addMenu(tr("Settings"));
   _settingsMenu->addAction(_toleranceAct);
   _settingsMenu->addAction(_parametersAct);
   _helpMenu = addMenu(tr("Help"));
   _helpMenu->addAction(_helpAct);
   _helpMenu->addAction(_aboutAct);
}

bool MainMenu::maybeSave()
{
   if (_qGrains->modified()) 
   {
      QMessageBox::StandardButton ret;
      ret = QMessageBox::warning(this, tr("Application"),
            tr("The document has been modified.\n"
                         "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      if (ret == QMessageBox::Save) 
      {
         save();
         return true;
      }
      else if (ret == QMessageBox::Cancel) return false;
   }
   return true;
}
