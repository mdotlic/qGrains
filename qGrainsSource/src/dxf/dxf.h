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
#ifndef DXF_H
#define DXF_H

namespace DXF
{
   inline void writeLineDxf(const double p1x, const double p1y, 
         const double p2x, const double p2y, const QString & lineLayer, 
         QTextStream & in)
   {
      in.setRealNumberPrecision(10);
      in<<"LINE\n100\n";
      in<<"AcBdEntity\n  8\n";
      in<<lineLayer<<"\n";
      in<<"100\nAcDbLine\n";
      in<<" 10\n"<<p1x<<"\n";
      in<<" 20\n"<<p1y<<"\n";
      in<<" 30\n0.0\n";
      in<<" 11\n"<<p2x<<"\n";
      in<<" 21\n"<<p2y<<"\n";
      in<<" 31\n0.0\n";
      in<<"  0\n";
   }

   inline void writeTextDxf(const double p1x, const double p1y, 
         const QString & textLayer, QTextStream & in, const int & hpos, 
         const int & vpos, const QString & text)
   {
      //hpos 0 = Left; 1= Center; 2 = Right
      //vpos 0 = Baseline; 1 = Bottom; 2 = Middle; 3 = Top
      in<<"TEXT\n100\n";
      in<<"AcBdEntity\n  8\n";
      in<<textLayer<<"\n";
      in<<"100\nAcDbText\n";
      if(hpos==0 && vpos==0)
      {
         in<<" 10\n"<<p1x<<"\n";
         in<<" 20\n"<<p1y<<"\n";
      }
      else
      {
         in<<" 10\n0.0\n";
         in<<" 20\n0.0\n";
      }
      in<<" 30\n0.0\n 40\n1.0\n";
      in<<" 1\n";
      in<<text<<"\n";
      in<<" 72\n";
      in<<hpos<<"\n";
      if(hpos!=0 || vpos!=0)
      {
         in<<" 11\n"<<p1x<<"\n";
         in<<" 21\n"<<p1y<<"\n";
         in<<" 31\n0.0\n";
      }
      in<<" 73\n";
      in<<vpos<<"\n";
      in<<"  0\n";
   }

   inline void writePointDxf(const double p1x, const double p1y,
         const QString & pointLayer, QTextStream & in)
   {
      in<<"POINT\n100\n";
      in<<"AcBdEntity\n  8\n";
      in<<pointLayer<<"\n";
      in<<"100\nAcDbPoint\n";
      in<<" 10\n"<<p1x<<"\n";
      in<<" 20\n"<<p1y<<"\n";
      in<<" 30\n0.0\n";
      in<<"  0\n";
   }

}

#endif
