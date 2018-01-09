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
#ifndef HANDLER_H
#define HANDLER_H

//#include <QAbstractItemModel>
#include <vector>
#include <QDebug>
#include <QModelIndex>
class ModelNodeBase;
class QString;

class Handler //: public QAbstractItemModel
{
   //Q_OBJECT
   public:
      Handler(ModelNodeBase * model);
    
      void commandDrillAppendLoad(const std::vector<double> & v, 
          const QString & name);
      void drillSetChecks(const int & idrill, const std::vector<int> & vec);
      void commandDepthAppendLoad(const double & v0, const double & v1,
            const QModelIndex & index = QModelIndex());
      void sampleSetChecks(const int & isample, const int & idrill,
            const bool & check, const int & color, const int & line, 
            const bool & condCheck);
      void commandCoorAppendLoad(const double * v1, 
            const QModelIndex & index = QModelIndex());

      void pointFromCsv(const QString &, const double &, const double &, 
            const double &, const double &, const double &, const double &);
      int findSample(ModelNodeBase *, const double &, const double &);
      
      void profileSetMap(const std::vector<double> &, const QString &);
      void setProfileType(const int &, const double &, const double &);
      void setPlanType(const int &, const double &, const double &, const int &,
            const int &, const int &, const int &, const double &, 
            const double &);
      void setActiveProf(const int &);
      void loadSurface(bool, const QString &);
      void surfacePointLoad(const double * v);
      void addNewProfile(const QString &);
      void addProfilePoint(const int &);
      void loadDelDefaultProf();
      //const QString name(const QModelIndex & index);

      double elev(const int & idrill);
      double depth(const int & idrill);
      
      int typeCalc();
      double fromProfile();
      double toProfile();

      void setTolerance(const double &);
      void setTemp(const double &);
      void setVisc(const double &);
      void setTerc(const double &);
      void setCunk(const double &);
      void setCond(const int &, const bool &);

      int nDrills();
      int nSample(const int & idrill);
      QString drillName(const int &);
      QString sampleName(const int &, const int &);
      double sampleVal(const int & idrill, const int & isample, const int & iv);
      double drillX(const int & idrill);
      double drillY(const int & idrill);
      double tolerance();
      double visc();
      double terc();
      double zunk();

      int nPoints(const int & idrill, const int & isample);
      double pointGrainSize(const int & idrill, const int & isample, 
            const int & ipoint);
      double pointPercent(const int & idrill, const int & isample, 
            const int & ipoint);

      int activeProfile();
      int nProfiles();
      int nProfilePoints(const int &);
      int profilePoint(const int &, const int &);
      QString profileName(const int &);

      double xmin();
      double xmax();
      double ymin();
      double ymax();
      double picXmin();
      double picXmax();
      double picYmin();
      double picYmax();
      double ratio();
      QString pictureName();
      void setXmin(const double &);
      void setXmax(const double &);
      void setYmin(const double &);
      void setYmax(const double &);
      void setPicXmin(const double &);
      void setPicXmax(const double &);
      void setPicYmin(const double &);
      void setPicYmax(const double &);
      void setRatio();
      void setPictureName(const QString &);

      int nSurfaces();
      int nSurfPoints(const int &);
      double surfPointX(const int & isurf, const int & ipoint);
      double surfPointY(const int & isurf, const int & ipoint);
      double surfPointZ(const int & isurf, const int & ipoint);
      int surfPointDrill(const int & isurf, const int & ipoint);
      QString surfName(const int &);
      bool isDrillInSurface(const int &, const int &);
      
      void setPlanType(const int &);
      void setPlanFrom(const double &);
      void setPlanTo(const double &);
      void setPlanSurface(const int &);
      void setPlanSurfaceFrom(const int &);
      void setPlanSurfaceTo(const int &);
      void setPlanCalcType(const int &);
      void setPlanFromSize(const double &);
      void setPlanToSize(const double &);

      int planType();
      double planFrom();
      double planTo();
      int planSurface();
      int planSurfaceFrom();
      int planSurfaceTo();
      int planCalcType();
      double planFromSize();
      double planToSize();

      void setIsoXmin(const double &);
      void setIsoXmax(const double &);
      void setIsoYmin(const double &);
      void setIsoYmax(const double &);
      void setIsoXdiv(const int &);
      void setIsoYdiv(const int &);

      double isoXmin();
      double isoXmax();
      double isoYmin();
      double isoYmax();
      int isoXdiv();
      int isoYdiv();

      int isoValuesSize();
      double isoValue(const int &);
      void addIsoValue(const double &);
      void deleteIsoValue(const int &);

   private:         
      ModelNodeBase * _model;
};

#endif
