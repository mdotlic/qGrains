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
#ifndef MODEL_ENUMS_H
#define MODEL_ENUMS_H
namespace ModelEnums
{
   enum Root{DRILLS, DCHECKED, PERCCHECK, CHECKS, CONDUCTIVITY, PROFILETAB, SURFACES, PLAN};
   enum Drills{EPS, TEMP, VISC, TERC, CUNK, Drills_size};
   enum DrillProp{X, Y, Elevation, Depth, DrillProp_Size};
   enum DProp{DCheck, Color, Line, DCheck_Size};
   enum SizeRangeProp{From, To, PCheck, PercColor, PercLine, SizeRangeProp_Size};
   enum DrillChecksProp{Check, dTick, PercDrillCheck, PercTick, PlotCheck, CondCheck, ProfileColor, DrillChecksProp_Size};
   enum SampleProp{SampleCheck, SampleColor, SampleLine, SampleCondCheck, SampleProp_Size};
   enum CondProp{UNIF, POROSITY, HAZEN, SLICHTER, TERZAGHI, BEYER, SAUERBREI, KRUGER, KOZENY, ZUNKER, ZAMARIN, USBR, Coductivity_Size};
   enum ProfileTabProp{ProfType, ProfFrom, ProfTo, Profile_Size};
   enum ProfileTab{Map, Profiles};
   enum Profile{ProfileArray};
   enum MapProp{xmin, xmax, ymin, ymax, picXmin, picXmax, picYmin, picYmax, ratio, MapProp_Size};
   enum SurfacePoint{Xsurf, Ysurf, Zsurf, DrillSurf};
   enum PlanProp{PlanType, PlanFrom, PlanTo, SurfaceID, SurfaceFrom, SurfaceTo, CalcType, PlanFromSize, PlanToSize, PlanProp_Size};
   enum Plan{Isoline};
   enum IsolineProp{Xmin, Xmax, Ymin, Ymax, Xdiv, Ydiv, IsolineProp_Size};
}
#endif
