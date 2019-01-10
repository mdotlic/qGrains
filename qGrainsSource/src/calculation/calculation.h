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
#ifndef CALCULATION_H
#define CALCULATION_H

#include <cmath>

namespace Calculation
{
   inline double interp(const double & unkX, const double & x0, 
         const double & x1, const double & y0, const double & y1)
   {
      return y0+(unkX-x0)*(y1-y0)/(x1-x0);
   }

   inline double distanceFromFirst(const int & drillID, Handler * handler)
   {
      double rho = 0.0;
      int iprofile = handler->activeProfile();
      for(int i=0;i<handler->nProfilePoints(iprofile); i++)
      {
         int idrill = handler->profilePoint(iprofile, i);
         if(i!=0)
         {
            int first = handler->profilePoint(iprofile, i-1);
            rho+=sqrt(pow(handler->drillX(first)-handler->drillX(idrill), 2) + pow(handler->drillY(first)-handler->drillY(idrill), 2) );
         }
         if(idrill == drillID)
            return rho;
      }
      return rho;
   }

   inline double findUniformity(const int & idrill, const int & jsample, 
         Handler * handler) 
   {
      double d10 = -1.0, d60=-1.0, uniformity = -1;
      int npoints = handler->nPoints(idrill, jsample);
      for(int i=0; i<npoints; i++)
      {
         if(fabs(handler->pointPercent(idrill, jsample, i) - 10.0) < handler->tolerance())
         {
            d10=handler->pointGrainSize(idrill, jsample, i);
         }
         if(fabs(handler->pointPercent(idrill, jsample, i) - 60.0) < handler->tolerance())
         {
            d60=handler->pointGrainSize(idrill, jsample, i);
         }
      }
      if(d10!=-1 && d60!=-1)
         uniformity = d60/d10;
      return uniformity;
   }

   inline double findPorosity(const int & idrill, const int & jsample, 
         Handler * handler)
   {
      double porosity = -1;
      double uniformity = findUniformity(idrill, jsample, handler);
      if(uniformity != -1)
         porosity = 0.255 * (1+pow(0.83, uniformity));
      return porosity;
   }

   inline QString calcValue(const int & idrill, const int & jsample, 
         Handler * handler, const int & typeCalc, const double & from, 
         const double & to, bool & correct) 
   {
      QString res;
      double gSaNi = 9.81/handler->visc();
      int npoints = handler->nPoints(idrill, jsample);
      if(npoints==0)
         return QString("");
      switch(typeCalc)
      {
         case 0:
            {
               correct=true;
               for(int i=0; i<npoints; i++)
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 10.0) < handler->tolerance())
                     res=QString::number(
                           handler->pointGrainSize(idrill, jsample, i), 'e',2);
            }
            break;
         case 1:
            {
               correct = true;
               for(int i=0; i<npoints; i++)
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 15.0) < handler->tolerance())
                     res=QString::number(
                           handler->pointGrainSize(idrill, jsample, i), 'e',2);
            }
            break;
         case 2:
            {
               correct = true;
               for(int i=0; i<npoints; i++)
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 20.0) < handler->tolerance())
                     res=QString::number(
                           handler->pointGrainSize(idrill, jsample, i), 'e',2);
            }
            break;
         case 3:
            {
               correct = true;
               for(int i=0; i<npoints; i++)
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 50.0) < handler->tolerance())
                     res=QString::number(
                           handler->pointGrainSize(idrill, jsample, i), 'e',2);
            }
            break;
         case 4:
            {
               correct = true;
               for(int i=0; i<npoints; i++)
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 60.0) < handler->tolerance())
                     res=QString::number(
                           handler->pointGrainSize(idrill, jsample, i), 'e',2);
            }
            break;
         case 5:
            {
               correct = true;
               for(int i=0; i<npoints; i++)
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 85.0) < handler->tolerance())
                     res=QString::number(
                           handler->pointGrainSize(idrill, jsample, i), 'e',2);
            }
            break;
         case 6:
            {//uniformity
               correct = true;
               double d10 = -1.0;
               double d60 =-1.0;
               for(int i=0; i<npoints; i++)
               {
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 10.0) < handler->tolerance())
                  {
                     d10=handler->pointGrainSize(idrill, jsample, i);
                  }
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 60.0) < handler->tolerance())
                  {
                     d60=handler->pointGrainSize(idrill, jsample, i);
                  }
               }
               if(d10==-1)
                  res=QString("no d10");
               else if(d60==-1)
                  res=QString("no d60");
               else
                  res=QString::number(d60/d10, 'f', 3);
            }
            break;
         case 7:
            {//porosity
               correct =true;
               double uniformity = findUniformity(idrill, jsample, handler);
               if(uniformity == -1)
                  res=QString("no uniformity");
               else
                  res=QString::number(0.255 * (1+pow(0.83, uniformity)), 'f', 3);
            }
            break;
         case 8:
            {//hazen
               double porosity = findPorosity(idrill, jsample, handler);
               double d10 = -1.0;
               for(int i=0; i<npoints; i++)
               {
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 10.0) < handler->tolerance())
                  {
                     d10=handler->pointGrainSize(idrill, jsample, i);
                  }
               }
               if(d10==-1)
                  res=QString("no d10");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  res=QString::number(gSaNi*0.0006*(1+10*(porosity-0.26))*d10*d10, 'e', 2);
                  if(d10>0.1 && d10<3 && 
                        findUniformity(idrill, jsample, handler)<5)
                     correct=true;
               }
            }
            break;
         case 9:
            {//slichter
               double porosity = findPorosity(idrill, jsample, handler);
               double d10 = -1.0;
               for(int i=0; i<npoints; i++)
               {
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 10.0) < handler->tolerance())
                  {
                     d10=handler->pointGrainSize(idrill, jsample, i);
                  }
               }
               if(d10==-1)
                  res=QString("no d10");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  res=QString::number(gSaNi*0.01*pow(porosity, 3.287)*d10*d10, 'e', 2);
                  if(d10>0.01 && d10<5)
                     correct=true;
               }
            }
            break;
         case 10:
            {//terzaghi
               double porosity = findPorosity(idrill, jsample, handler);
               double d10 = -1.0;
               for(int i=0; i<npoints; i++)
               {
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 10.0) < handler->tolerance())
                  {
                     d10=handler->pointGrainSize(idrill, jsample, i);
                  }
               }

               if(d10==-1)
                  res=QString("no d10");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  double porosityFunc = pow((porosity-0.13)/pow(1.0-porosity, 1.0/3.0), 2);
                  res=QString::number(gSaNi*handler->terc()*porosityFunc*d10*d10, 'e', 2);
                  if(d10>1 && d10<2)
                     correct=true;
               }
            }
            break;
         case 11:
            {//beyer
               double uniformity = findUniformity(idrill, jsample, handler);
               double d10=-1;
               for(int i=0; i<npoints; i++)
               {
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 10.0) < handler->tolerance())
                  {
                     d10=handler->pointGrainSize(idrill, jsample, i);
                  }
               }

               if(d10==-1)
                  res=QString("no d10");
               else if(uniformity == -1)
                  res=QString("no uniformity");
               else
               {
                  res=QString::number(gSaNi*0.00052*log10(500/uniformity)*d10*d10,'e', 2);
                  if(d10>0.06 && d10<0.6 && uniformity>1 && uniformity<20)
                     correct=true;
               }
            }
            break;
         case 12:
            {//Sauerbrei
               double porosity = findPorosity(idrill, jsample, handler);
               double d17 = -1.0;
               for(int i=0; i<npoints; i++)
               {
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 17.0) < handler->tolerance())
                  {
                     d17=handler->pointGrainSize(idrill, jsample, i);
                  }
               }

               if(d17==-1)
                  res=QString("no d17");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  double porosityFunc = pow(porosity, 3)/pow(1-porosity, 2);
                  res=QString::number(gSaNi*3.75e-3*porosityFunc*d17*d17, 'e', 2);
                  if(d17>0.5)
                     correct=true;
               }
            }
            break;
         case 13:
            {//Kruger
               double sum=0.0;
               for(int i=0; i<npoints-1; i++)
               {
                  sum+=2*(handler->pointPercent(idrill, jsample, i+1)*0.01 - 
                          handler->pointPercent(idrill, jsample, i)*0.01)/
                     (handler->pointGrainSize(idrill, jsample, i) +
                      handler->pointGrainSize(idrill, jsample, i+1));
               }
               double de = 1/sum;
               double porosity = findPorosity(idrill, jsample, handler);
               if(de==-1)
                  res=QString("no de");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  double oneMinPorSq = pow(1-porosity, 2);
                  res=QString::number(gSaNi*5.06e-4*(porosity/oneMinPorSq)*de*de, 'e', 2);
                  if(de>0.2 && de<1.0 && 
                        findUniformity(idrill, jsample, handler)>5)
                     correct=true;
               }
            }
            break;
         case 14:
            {//Kozeny
               double sum = 0.0;
               for(int i=0; i<npoints-1; i++)
               {
                  sum+=((handler->pointPercent(idrill, jsample, i+1)*0.01 - 
                         handler->pointPercent(idrill, jsample, i)*0.01) * 
                        (handler->pointGrainSize(idrill, jsample, i) +
                         handler->pointGrainSize(idrill, jsample, i+1)))/
                     (2*handler->pointGrainSize(idrill, jsample, i) *
                      handler->pointGrainSize(idrill, jsample, i+1));
                  if(i==0 && fabs(handler->pointGrainSize(idrill,jsample,i+1)-
                           0.0025)<1e-5)
                  {
                     sum = 3/(2*0.0025)*handler->pointPercent(idrill, jsample, 
                           i+1)*0.01;
                  }
               }
               double de = 1/sum;
               double porosity = findPorosity(idrill, jsample, handler);
               if(de==-1)
                  res=QString("no de");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  double oneMinPorSq = pow(1-porosity, 2);
                  res=QString::number( gSaNi*8.3e-3*(pow(porosity,3)/oneMinPorSq)*de*de, 'e', 2);
                  if(de>1.0 && de<2.0)
                     correct=true;

               }
            }
            break;
         case 15:
            {//Zunker
               double sum = 0.0;
               for(int i=0; i<npoints-1; i++)
               {
                  sum+=((handler->pointPercent(idrill, jsample, i+1)*0.01 - 
                           handler->pointPercent(idrill, jsample, i)*0.01) * 
                        (handler->pointGrainSize(idrill, jsample, i+1) -
                         handler->pointGrainSize(idrill, jsample, i)))/
                     (handler->pointGrainSize(idrill, jsample, i) *
                      handler->pointGrainSize(idrill, jsample, i+1) *
                      log(handler->pointGrainSize(idrill, jsample, i+1) / 
                         handler->pointGrainSize(idrill, jsample, i)));
                  if(i==0 && fabs(handler->pointGrainSize(idrill,jsample,i+1)
                           - 0.0025)<1e-5)
                  {
                     sum = 3/(2*0.0025)*handler->pointPercent(idrill, jsample, i+1)*0.01;
                  }
               }
               double de = 1/sum;
               double porosity = findPorosity(idrill, jsample, handler);
               if(de==-1)
                  res=QString("no de");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  double oneMinPorSq = pow(1-porosity, 2);
                  res=QString::number(gSaNi*handler->zunk()*(pow(porosity,2)/oneMinPorSq)*de*de, 'e', 2);
                  if(de>0.1 && de<1.0)
                     correct=true;
               }
            }
            break;
         case 16:
            {//Zamarin
               double sum = 0.0;
               for(int i=0; i<npoints-1; i++)
               {

                  /*if(fabs(handler->pointGrainSize(idrill, jsample, i+1) -
                      handler->pointGrainSize(idrill, jsample, i))>1e-16)
                  */
                     sum+=((handler->pointPercent(idrill, jsample, i+1)*0.01 - 
                           handler->pointPercent(idrill, jsample, i)*0.01) * 
                        log(handler->pointGrainSize(idrill, jsample, i+1)/
                           handler->pointGrainSize(idrill, jsample, i)))/
                     (handler->pointGrainSize(idrill, jsample, i+1) -
                      handler->pointGrainSize(idrill, jsample, i));
                  
                  
                  if(i==0 && fabs(handler->pointGrainSize(idrill, jsample, i+1)
                           - 0.0025)<1e-5)
                  {
                     sum = 3/(2*0.0025)*handler->pointPercent(idrill, jsample, i+1)*0.01;
                  }
               }
               double de = 1/sum;
               double porosity = findPorosity(idrill, jsample, handler);
               if(de==-1)
                  res=QString("no de");
               else if(porosity==-1)
                  res=QString("no porosity");
               else
               {
                  double oneMinPorSq = pow(1-porosity, 2);
                  res=QString::number(gSaNi*8.65e-3*(pow(porosity,3)/oneMinPorSq)*pow((1.275-1.5*porosity),2)*de*de, 'e', 2);
                  if(de>1.0 && de<2.0)
                     correct=true;
               }
            }
            break;
         case 17:
            {//USBR
               double d20 = -1.0;
               for(int i=0; i<npoints; i++)
               {
                  if(fabs(handler->pointPercent(idrill, jsample, i) - 20.0) < handler->tolerance())
                  {
                     d20=handler->pointGrainSize(idrill, jsample, i);
                  }
               }
               if(d20 == -1)
                  res=QString("no d20");
               else
               {
                  res=QString::number(gSaNi*0.00042*pow(d20,0.3)*d20*d20, 'e', 2);
                  if(d20>0.2 && d20<1.0 && 
                        findUniformity(idrill, jsample, handler)<5)
                     correct=true;
               }
            }
            break;
         case 18:
            {
               correct=true;
               if(fabs(from-to)<1e-10)
                  res=QString("");
               else
               {
                  int nPoints = handler->nPoints(idrill, jsample);
                  double down=0.0;

                  if(handler->pointGrainSize(idrill, jsample, 0)>from)
                     down = 0;//if there is no smaller grain size than from
                  else if(handler->pointGrainSize(idrill, jsample, nPoints-1)<from)
                     down = 100;//if there is no bigger grain size than from
                  else
                  {
                     for(int k=1; k<nPoints; k++)
                     {
                        if(handler->pointGrainSize(idrill, jsample, k) > from)
                        {//first bigger grain size
                           down = interp(from, 
                                 handler->pointGrainSize(idrill, jsample, k-1),
                                 handler->pointGrainSize(idrill, jsample, k),
                                 handler->pointPercent(idrill, jsample, k-1),
                                 handler->pointPercent(idrill, jsample, k));
                           break;
                        }
                     }
                  }

                  double up=0.0;
                  if(handler->pointGrainSize(idrill, jsample, 0)>to)
                     up = 0;//if there is no smaller grain size than to
                  else if(handler->pointGrainSize(idrill, jsample, nPoints-1)<=to)
                     up = 100;//if there is no bigger grain size than to
                  else
                  {
                     for(int k=1; k<nPoints; k++)
                     {
                        if(handler->pointGrainSize(idrill, jsample, k) > to)
                        {//first bigger grain size
                           up = interp(to, 
                                 handler->pointGrainSize(idrill, jsample, k-1),
                                 handler->pointGrainSize(idrill, jsample, k),
                                 handler->pointPercent(idrill, jsample, k-1),
                                 handler->pointPercent(idrill, jsample, k));
                           break;
                        }
                     }
                  }
                  res=QString::number(up-down, 'f', 2);
               }
            }
            break;
         default:
            res=QString("nepoznat");
            break;
      }
      return res;
   }

}


#endif
