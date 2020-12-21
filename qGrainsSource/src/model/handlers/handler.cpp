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
#include "handler.h"

#include "model/modelNodeBase.h"
#include "model/modelEnums.h"
#include "model/pointSet.h"
#include "model/intPointSet.h"
#include <QMessageBox>
#include <assert.h>
#include <cmath>
#include <QDebug>

Handler::Handler(ModelNodeBase * model):_model(model)
{
}


void Handler::commandDrillAppendLoad(const std::vector<double> & v, 
      const QString & name)
{
   emit _model->loadDrill(v, name);
}

void Handler::drillSetChecks(const int & idrill, 
      const std::vector<int> & vec)
{
   ModelNodeBase * drillCheckNode =  _model->child(ModelEnums::Root::CHECKS)->child(idrill);

  if(vec[0])
     drillCheckNode->setProperty(true,ModelEnums::DrillChecksProp::Check);
  else
     drillCheckNode->setProperty(false,ModelEnums::DrillChecksProp::Check);

  drillCheckNode->setProperty(vec[1],ModelEnums::DrillChecksProp::dTick);
  if(vec[2])
     drillCheckNode->setProperty(true, 
        ModelEnums::DrillChecksProp::PercDrillCheck);
  else
     drillCheckNode->setProperty(false, 
        ModelEnums::DrillChecksProp::PercDrillCheck);
  drillCheckNode->setProperty(vec[3],ModelEnums::DrillChecksProp::PercTick);
  if(vec[4])
     drillCheckNode->setProperty(true, ModelEnums::DrillChecksProp::PlotCheck);
  else
     drillCheckNode->setProperty(false, ModelEnums::DrillChecksProp::PlotCheck);
  if(vec[5])
     drillCheckNode->setProperty(true, ModelEnums::DrillChecksProp::CondCheck);
  else
     drillCheckNode->setProperty(false, ModelEnums::DrillChecksProp::CondCheck);
  drillCheckNode->setProperty(vec[6],ModelEnums::DrillChecksProp::ProfileColor);
}

void Handler::commandDepthAppendLoad(const double & v0, const double & v1,
      const QModelIndex & index)
{
   emit _model->loadDepth(v0, v1, index);
}

void Handler::sampleSetChecks(const int & isample, const int & idrill,
      const bool & check, const int & color, const int & line, 
      const bool & condCheck)
{
   ModelNodeBase * sampleCheckNode =  _model->child(ModelEnums::Root::CHECKS)->child(idrill)->child(isample);

  sampleCheckNode->setProperty(check, ModelEnums::SampleProp::SampleCheck);
  sampleCheckNode->setProperty(color, ModelEnums::SampleProp::SampleColor);
  sampleCheckNode->setProperty(line, ModelEnums::SampleProp::SampleLine);
  sampleCheckNode->setProperty(condCheck, 
        ModelEnums::SampleProp::SampleCondCheck);
  if(check)
     emit _model->loadPlotTrue(idrill, isample);
  if(condCheck)
     emit _model->loadCondTableTrue(idrill, isample);
}

void Handler::commandCoorAppendLoad(const double * pnt, 
      const QModelIndex & index)
{
   emit _model->loadCoor(pnt, index);
}

void Handler::pointFromCsv(const QString & name, const double & type, 
      const double & d1, const double & d2, const double & value, 
      const double & x, const double & y)
{
   ModelNodeBase * drills = _model->child(ModelEnums::Root::DRILLS);
   int idrill = drills->children()->size()-1;

   std::vector<double> v(4, -1.0);
   v[0] = x;
   v[1] = y;

   if(idrill==-1)
   {
      emit _model->loadDrill(v, name);
      idrill=0;
   }
   ModelNodeBase * drill = drills->child(idrill);
   if(drill->name() != name)
   {
      emit _model->loadDrill(v, name);
      idrill = drills->children()->size()-1;
      drill = drills->child(idrill);
   }
   int foundSample = findSample(drill, d1, d2);
   
   double pnt[2];         
   pnt[0] = value;
   pnt[1] = type;
   if(foundSample==-1)
   {//new sample 
      commandDepthAppendLoad(d1,d2, QModelIndex());
      foundSample = findSample(drill, d1, d2);
   }

   PointSet * points = static_cast<PointSet*> (drill->child(foundSample)->child(0));         
   points->insertNewPoint(pnt);
   //else
     // emit _model->loadDrill(v, name);

}

int Handler::findSample(ModelNodeBase * drill, const double & d1, 
      const double & d2)
{
   int nsamples = drill->children()->size();
   int foundSample = -1;
   for(int isample=0;isample<nsamples;isample++)
   {
      double from = drill->child(isample)->properties()[0].toDouble();
      double to = drill->child(isample)->properties()[1].toDouble();
      if(fabs(from-d1)<1e-4 && fabs(to-d2)<1e-4)
      {
         foundSample = isample;
         break;
      }
   }
   return foundSample;
}

void Handler::profileSetMap(const std::vector<double> & v, 
      const QString & fileName)
{
   ModelNodeBase * mapNode =  _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map);
   for(size_t i=0;i<9;i++)
   {
      mapNode->setProperty(v[i], i);
   }
   mapNode->setName(fileName);
}

void Handler::setProfileType(const int & type, const double & from, 
      const double & to)
{
   ModelNodeBase * profileTabNode = _model->child(ModelEnums::Root::PROFILETAB);
   profileTabNode->setProperty(type, ModelEnums::ProfileTabProp::ProfType);
   profileTabNode->setProperty(from, ModelEnums::ProfileTabProp::ProfFrom);
   profileTabNode->setProperty(to, ModelEnums::ProfileTabProp::ProfTo);
}

void Handler::setPlanType(const int & planType, const double & planFrom, 
      const double & planTo, const int & surfaceId, const int & surfaceFrom,
      const int & surfaceTo, const int & calcType, const double & planFromSize, 
      const double & planToSize)
{
   ModelNodeBase * planNode = _model->child(ModelEnums::Root::PLAN);
   planNode->setProperty(planType, ModelEnums::PlanProp::PlanType);
   planNode->setProperty(planFrom, ModelEnums::PlanProp::PlanFrom);
   planNode->setProperty(planTo, ModelEnums::PlanProp::PlanTo);
   planNode->setProperty(surfaceId, ModelEnums::PlanProp::SurfaceID);
   planNode->setProperty(surfaceFrom, ModelEnums::PlanProp::SurfaceFrom);
   planNode->setProperty(surfaceTo, ModelEnums::PlanProp::SurfaceTo);
   planNode->setProperty(calcType, ModelEnums::PlanProp::CalcType);
   planNode->setProperty(planFromSize, ModelEnums::PlanProp::PlanFromSize);
   planNode->setProperty(planToSize, ModelEnums::PlanProp::PlanToSize);
}

void Handler::setActiveProf(const int & active)
{
   ModelNodeBase * profiles = _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles);
   profiles->setProperty(active, 0);
}

void Handler::loadSurface(bool check, const QString & name)
{
   ModelNodeBase * surfaces =  _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = new ModelNodeBase(surfaces, 1);
   surface->setName(name);
   surface->setProperty(check, 0);
   PointSet * coors = new PointSet(4, surface);
   coors->setName("Surface points");
   surface->appendChild(coors);
   surfaces->appendChild(surface);
}

void Handler::surfacePointLoad(const double * v)
{//append new coor
   ModelNodeBase * surfaces =  _model->child(ModelEnums::Root::SURFACES);
   const int isurf = surfaces->children()->size()-1;
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));

   points->appendPoint(v);

}

void Handler::addNewProfile(const QString & name)
{
   ModelNodeBase * profiles =  _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles);
   ModelNodeBase * profile = new ModelNodeBase (profiles);
   profile->setName(name);
   IntPointSet * drillsID = new IntPointSet(1, profile);
   drillsID->setName("array Drill Ids");
   profile->appendChild(drillsID);
   
   profiles->appendChild(profile);
}

void Handler::addProfilePoint(const int & idrill)
{
   ModelNodeBase * profiles =  _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles);
   int iprofil = profiles->children()->size()-1;
   ModelNodeBase * profile = profiles->child(iprofil);
   IntPointSet * points = static_cast<IntPointSet*> (profile->child(ModelEnums::Profile::ProfileArray));

   int pnt[1];
   pnt[0]=idrill;

   //qDebug()<<" sta je sa tobom "<<idrill<<"\n";
   points->appendPoint(pnt);
}

void Handler::loadDelDefaultProf()
{
   ModelNodeBase * profiles =  _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles);
   profiles->removeChildren(0,1);
}

double Handler::elev(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->properties()[ModelEnums::DrillProp::Elevation].toDouble();
}

double Handler::depth(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->properties()[ModelEnums::DrillProp::Depth].toDouble();
}

int Handler::typeCalc()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->properties()[ModelEnums::ProfileTabProp::ProfType].toInt();
}

double Handler::fromProfile()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->properties()[ModelEnums::ProfileTabProp::ProfFrom].toDouble();
}

double Handler::toProfile()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->properties()[ModelEnums::ProfileTabProp::ProfTo].toDouble();
}

void Handler::setTolerance(const double & value)
{
   _model->child(ModelEnums::Root::DRILLS)->setProperty(value, ModelEnums::Drills::EPS);
}

void Handler::setTemp(const double & value)
{
   _model->child(ModelEnums::Root::DRILLS)->setProperty(value, ModelEnums::Drills::TEMP);
}

void Handler::setVisc(const double & value)
{
   _model->child(ModelEnums::Root::DRILLS)->setProperty(value, ModelEnums::Drills::VISC);
}

void Handler::setTerc(const double & value)
{
   _model->child(ModelEnums::Root::DRILLS)->setProperty(value, ModelEnums::Drills::TERC);
}

void Handler::setCunk(const double & value)
{
   _model->child(ModelEnums::Root::DRILLS)->setProperty(value, ModelEnums::Drills::CUNK);
}

void Handler::setCond(const int &id, const bool & check)
{
   _model->child(ModelEnums::Root::CONDUCTIVITY)->setProperty(check, id);
}

int Handler::nDrills()
{
   return _model->child(ModelEnums::Root::CHECKS)->rowCount();
}

int Handler::nSample(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->rowCount();
}

QString Handler::drillName(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->name();
}

QString Handler::sampleName(const int & idrill, const int & jsample)
{
   return QString::number(_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(jsample)->properties()[0].toDouble(), 'f', 2)+" - "+QString::number(_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(jsample)->properties()[1].toDouble(), 'f', 2);
}

double Handler::sampleVal(const int & idrill, const int & isample, 
      const int & iv)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(isample)->properties()[iv].toDouble();
}

double Handler::drillX(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->properties()[ModelEnums::DrillProp::X].toDouble();
}

double Handler::drillY(const int & idrill)
{
   return _model->child(ModelEnums::Root::DRILLS)->child(idrill)->properties()[ModelEnums::DrillProp::Y].toDouble();
}

double Handler::tolerance()
{
   return _model->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::EPS].toDouble();
}

double Handler::visc()
{
   return _model->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::VISC].toDouble();
}

double Handler::terc()
{
   return _model->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::TERC].toDouble();
}

double Handler::zunk()
{
   return _model->child(ModelEnums::Root::DRILLS)->properties()[ModelEnums::Drills::CUNK].toDouble();
}

int Handler::nPoints(const int & idrill, const int & isample)
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(isample)->child(0));
   return points->rowCount(); 
}

int Handler::sampleColor(const int & idrill, const int & isample)
{
   return _model->child(ModelEnums::Root::CHECKS)->child(idrill)->child(isample)->properties()[ModelEnums::SampleProp::SampleColor].toInt();
}

double Handler::pointGrainSize(const int & idrill, const int & isample, 
      const int & ipoint)
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(isample)->child(0));
   return points->data(ipoint, 0).toDouble();
}

double Handler::pointPercent(const int & idrill, const int & isample, 
      const int & ipoint)
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::DRILLS)->child(idrill)->child(isample)->child(0));
   return points->data(ipoint, 1).toDouble();
}

int Handler::activeProfile()
{
   return static_cast<ModelNodeBase*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles))->properties()[0].toInt();
}

int Handler::nProfiles()
{
   return static_cast<ModelNodeBase*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles))->rowCount();
}

int Handler::nProfilePoints(const int & iprofile)
{
   IntPointSet * points = static_cast<IntPointSet*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles)->child(iprofile)->child(0));
   return points->rowCount();
}

int Handler::profilePoint(const int & iprofile, const int & ipoint)
{
   IntPointSet * points = static_cast<IntPointSet*> (_model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles)->child(iprofile)->child(0));
   return points->data(ipoint,0).toInt();
}

QString Handler::profileName(const int & iprofile)
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Profiles)->child(iprofile)->name();
}

double Handler::xmin()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::xmin].toDouble();
}

double Handler::xmax()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::xmax].toDouble();
}

double Handler::ymin()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::ymin].toDouble();
}

double Handler::ymax()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::ymax].toDouble();
}

double Handler::picXmin()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::picXmin].toDouble();
}

double Handler::picXmax()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::picXmax].toDouble();
}

double Handler::picYmin()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::picYmin].toDouble();
}

double Handler::picYmax()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::picYmax].toDouble();
}

double Handler::ratio()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->properties()[ModelEnums::MapProp::ratio].toDouble();
}

QString Handler::pictureName()
{
   return _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->name();
}

void Handler::setXmin(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::xmin);
}

void Handler::setXmax(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::xmax);
}

void Handler::setYmin(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::ymin);
}

void Handler::setYmax(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::ymax);
}

void Handler::setPicXmin(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::picXmin);
}

void Handler::setPicXmax(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::picXmax);
}

void Handler::setPicYmin(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::picYmin);
}

void Handler::setPicYmax(const double & val)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::picYmax);
}

void Handler::setRatio()
{
   double val=(xmax()-xmin())/(ymax()-ymin());
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setProperty(val, ModelEnums::MapProp::ratio);
}

void Handler::setPictureName(const QString & name)
{
   _model->child(ModelEnums::Root::PROFILETAB)->child(ModelEnums::ProfileTab::Map)->setName(name);
}

int Handler::nSurfaces()
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   return surfaces->rowCount();
}

int Handler::nSurfPoints(const int & isurf)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->rowCount();   
}

double Handler::surfPointX(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::Xsurf).toDouble();
}

double Handler::surfPointY(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::Ysurf).toDouble();
}

double Handler::surfPointZ(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::Zsurf).toDouble();
}

int Handler::surfPointDrill(const int & isurf, const int & ipoint)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   return points->data(ipoint, ModelEnums::SurfacePoint::DrillSurf).toInt();
}

QString Handler::surfName(const int & isurf)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   return surface->name();
}

bool Handler::isDrillInSurface(const int & isurf, const int & idrill)
{
   ModelNodeBase * surfaces = _model->child(ModelEnums::Root::SURFACES);
   ModelNodeBase * surface = surfaces->child(isurf);
   PointSet * points = static_cast<PointSet*> (surface->child(0));
   for(int i = 0; i<points->numberOfPoints(); i++)
   {
      if(idrill==points->data(i, ModelEnums::SurfacePoint::DrillSurf).toInt())
      {
         return true;
      }
   }
   return false;
}

void Handler::setPlanType(const int & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::PlanType);
}

void Handler::setPlanFrom(const double & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::PlanFrom);
}

void Handler::setPlanTo(const double & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::PlanTo);
}

void Handler::setPlanSurface(const int & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::SurfaceID);
}

void Handler::setPlanSurfaceFrom(const int & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::SurfaceFrom);
}

void Handler::setPlanSurfaceTo(const int & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::SurfaceTo);
}

void Handler::setPlanCalcType(const int & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::CalcType);
}

void Handler::setPlanFromSize(const double & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::PlanFromSize);
}

void Handler::setPlanToSize(const double & v)
{
   _model->child(ModelEnums::Root::PLAN)->setProperty(v, ModelEnums::PlanProp::PlanToSize);
}

int Handler::planType()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::PlanType].toInt();
}

double Handler::planFrom()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::PlanFrom].toDouble();
}

double Handler::planTo()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::PlanTo].toDouble();
}

int Handler::planSurface()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::SurfaceID].toInt();
}

int Handler::planSurfaceFrom()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::SurfaceFrom].toInt();
}

int Handler::planSurfaceTo()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::SurfaceTo].toInt();
}

int Handler::planCalcType()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::CalcType].toInt();
}

double Handler::planFromSize()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::PlanFromSize].toDouble();
}

double Handler::planToSize()
{
   return _model->child(ModelEnums::Root::PLAN)->properties()[ModelEnums::PlanProp::PlanToSize].toDouble();
}

void Handler::setIsoXmin(const double & val)
{
   _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->setProperty(val, ModelEnums::IsolineProp::Xmin);
}

void Handler::setIsoXmax(const double & val)
{
   _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->setProperty(val, ModelEnums::IsolineProp::Xmax);
}

void Handler::setIsoYmin(const double & val)
{
   _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->setProperty(val, ModelEnums::IsolineProp::Ymin);
}

void Handler::setIsoYmax(const double & val)
{
   _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->setProperty(val, ModelEnums::IsolineProp::Ymax);
}

void Handler::setIsoXdiv(const int & val)
{
   _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->setProperty(val, ModelEnums::IsolineProp::Xdiv);
}

void Handler::setIsoYdiv(const int & val)
{
   _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->setProperty(val, ModelEnums::IsolineProp::Ydiv);
}

double Handler::isoXmin()
{
   return _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->properties()[ModelEnums::IsolineProp::Xmin].toDouble();
}

double Handler::isoXmax()
{
   return _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->properties()[ModelEnums::IsolineProp::Xmax].toDouble();
}

double Handler::isoYmin()
{
   return _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->properties()[ModelEnums::IsolineProp::Ymin].toDouble();
}

double Handler::isoYmax()
{
   return _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->properties()[ModelEnums::IsolineProp::Ymax].toDouble();
}

int Handler::isoXdiv()
{
   return _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->properties()[ModelEnums::IsolineProp::Xdiv].toDouble();
}

int Handler::isoYdiv()
{
   return _model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->properties()[ModelEnums::IsolineProp::Ydiv].toDouble();
}

int Handler::isoValuesSize()
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->child(0));
   return points->rowCount();
}

double Handler::isoValue(const int & i)
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->child(0));
   return points->data(i, 0).toDouble();
}

void Handler::addIsoValue(const double & value)
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->child(0));
   points->appendPoint(&value);
}

void Handler::deleteIsoValue(const int & i)
{
   PointSet * points = static_cast<PointSet*> (_model->child(ModelEnums::Root::PLAN)->child(ModelEnums::Plan::Isoline)->child(0));
   points->removePoint(i);
}
