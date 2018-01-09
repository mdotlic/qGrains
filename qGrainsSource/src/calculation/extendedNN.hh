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
#ifndef EXTENDEDNN_HH
#define EXTENDEDNN_HH
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <math.h>

//#include "convexCombination/orientation.hh"
//#define DEBUG_EXTENDEDNN
typedef size_t siZe_t;


extern "C" {
#include "../../thirdparty/nn/nn.h"
}

namespace NaturalNeighbors {

   inline static point makePoint (const double x, const double y, const double z)
   {
      point p;
      p.x = x;
      p.y = y;
      p.z = z;
      return p;
   }

   struct ExtendedNNGrid
   {
      ExtendedNNGrid (const std::vector<point> & inPoints) : _triPtr (NULL)
      {
         const siZe_t np = inPoints.size();
#ifdef DEBUG_EXTENDEDNN
         for (siZe_t i=0; i<np; i++)
            std::cout << i << " in point: " << inPoints[i].x << " " << inPoints[i].y << " " << inPoints[i].z << "\n";
#endif
         assert (np != 0);

         if (np == 1) {
            _dimension = 0;
            _boundary.resize (1,0);
         } else { 
            if (np == 2)
               _dimension = 1;
            else {

               // Check if all inPoints are on one line

               const double x0 = inPoints[0].x, y0 = inPoints[0].y;
               double d00 = 0.0, d01 = 0.0, d11 = 0.0; // d10==d01
               _dimension = 1;
               double scale2 = 1.0;
               for (siZe_t i=1; i<np; i++) {
                  const double difx = inPoints[i].x-x0, dify = inPoints[i].y-y0;
                  d00 += difx*difx;
                  d01 += difx*dify;
                  d11 += dify*dify;
                  const double det = (d00-d11)*d01;
                  scale2 *= difx*difx + dify*dify;
                  if (det*det > scale2 * 1e-12) {
                     _dimension = 2;
                     break;
                  }
               }
            }

            if (_dimension == 1) {
               const std::pair<double,double> v (inPoints[1].x-inPoints[0].x,
                     inPoints[1].y-inPoints[0].y);
               _boundary.resize (np);
               siZe_t count = 0;
               generate (_boundary.begin(), _boundary.end(),
                     [&count]() { return count++; });
               std::sort (_boundary.begin(), _boundary.end(),
                     [&v, &inPoints](siZe_t i, siZe_t j)
                     { return ((inPoints[j].x-inPoints[i].x)*v.first+
                        (inPoints[j].y-inPoints[i].y)*v.second > 0.0); });
            } else { // _dimension == 2
               _triPtr = delaunay_build (np, const_cast<point *>(inPoints.data()),
                     0, NULL, 0, NULL);
               const int nseg = numberofsegments (_triPtr);
               const int * seglist = segmentlist (_triPtr);
               _boundary.resize (nseg);
               for (int i=0; i<nseg; i++) {
                  _boundary[i] = seglist[2*i];
               }
            }
         }

         const double bsz = _boundary.size();
         _bPoints.reserve (bsz);
         for (siZe_t i=0; i<bsz; i++) {
            const auto & p (inPoints[_boundary[i]]);
            _bPoints.push_back (std::pair<double,double> (p.x, p.y));
         }
      }

      ~ExtendedNNGrid() { if (_triPtr != NULL) delaunay_destroy (_triPtr); }
      //........................................................................

      inline delaunay & triangulation() const { return * _triPtr; }

      inline siZe_t boundary(const siZe_t i) const { return _boundary[i]; }

      inline const std::vector<siZe_t> & boundary() const { return _boundary; }

      inline const std::pair<double,double> & bPoint (const siZe_t i) const
      {
         return _bPoints[i];
      }

      inline siZe_t dimension () const { return _dimension; }

      private:
      delaunay * _triPtr;
      std::vector<siZe_t> _boundary;
      std::vector<std::pair<double,double> > _bPoints;
      siZe_t _dimension;
   };

/////////////////////////////////////////////////////////////////////////////

   struct ExtendedNNCoordinates
   {
      ExtendedNNCoordinates (const ExtendedNNGrid & extgrid,
            const double * outPoints, siZe_t outSize, size_t tableWidth = 2) :
         _nngr (extgrid), _nnai(NULL)
      {
         _icoef.reserve (outSize);
         _coef.reserve (outSize);
         _ipointsInside.reserve (outSize);
         _ipointsOutside.reserve (outSize);

         if (_nngr.dimension() == 0) {
            for (siZe_t i=0; i<outSize; i++) {
               _icoef.push_back (std::vector<siZe_t> (1,0));
               _coef.push_back (std::vector<double> (1,1.0));
               _ipointsOutside.push_back (i);
            }
            return;
         }

         for (siZe_t i=0; i<outSize; i++) {
#ifdef DEBUG_EXTENDEDNN
            std::cout << i << " point " << outPoints[tableWidth*i] << " " << outPoints[tableWidth*i+1] << "\n";
#endif
            bool lastEdgeVisible (true), firstEdgeVisible (true);
            const siZe_t nb = _nngr.boundary().size();
            siZe_t i1 = nb-1, i2=0;
            int dir = 1;
            const double * p = &(outPoints[tableWidth*i]);
            if (_nngr.dimension() == 2) {
               point pp = makePoint (p[0], p[1], 0.0);
               if (delaunay_xytoi(&extgrid.triangulation(), &pp, 0) >= 0) {
                  // p belongs to the convex hull
                  _ipointsInside.push_back (i);
                  continue;
               } else {
                  _ipointsOutside.push_back (i);

                  bool starting = true;
                  bool visibleFound = false;
                  const std::pair<double,double> & p1 = _nngr.bPoint (i1);
                  std::pair<double,double> v1 (p1.first - p[0],
                        p1.second - p[1]);
                  double d1 = v1.first*v1.first + v1.second*v1.second;
                  do {
                     const std::pair<double,double> & p2 = _nngr.bPoint (i2);
                     std::pair<double,double> v2 (p2.first - p[0],
                           p2.second-p[1]);
                     const double det = v2.first*v1.second - v2.second*v1.first;
                     double d2 = v2.first*v2.first + v2.second*v2.second;
#ifdef DEBUG_EXTENDEDNN
                     std::cout << p2 << " det: " << det << " d2: " << d2 << " dir: " << dir << " starting: " << starting << "\n";
#endif
                     if (det*fabs(det)*dir >=  -1e-28 * d1 * d2) {
                        // segment p1,p2 is visible from p
                        if (!visibleFound) { // Previous segment was not visible
                           if (d1 <= d2) {
                              if (starting) { // We jumped straight into visible
                                 dir = -1;
                                 std::swap (i1, i2);
                                 std::swap (d1, d2);
                                 std::swap (v1, v2);
                              } else { // The first visible node is the closest
                                 firstEdgeVisible = false;
                                 break;
                              }
                           }
                           visibleFound = true;
                        } else if (d1 <= d2) break; // We found it
                     } else if (visibleFound) { // The last visible node is the
                        lastEdgeVisible = false; // closest
                        break;
                     }
                     i1 = i2;
                     v1 = v2;
                     d1 = d2;
                     if (dir == 1) {
                        if (++i2==nb) i2=0;
                     } else
                        i2 = (i2==0 ? nb-1 : i2-1);
                     starting = false;
                  } while (true);
               }
            } else { // _nngr.dimension() == 1
               _ipointsOutside.push_back (i);
               i1 = nb-1;
               const std::pair<double,double> & p1 (_nngr.bPoint (0));
               const std::pair<double,double> v (p1.first - p[0],
                     p1.second - p[1]);
               double mind = v.first*v.first + v.second*v.second;
               for (siZe_t j=1; j<nb; j++) {
                  const std::pair<double,double> & p1 (_nngr.bPoint (j));
                  const std::pair<double,double> v (p1.first - p[0],
                        p1.second - p[1]);
                  const double d = v.first*v.first + v.second*v.second;
                  if (d >= mind) {
                     i1 = j-1;
                     break;
                  }
                  mind = d;
               }
               firstEdgeVisible = (i1 != 0);
               if (i1 == nb-1)
                  lastEdgeVisible = false;
               else
                  i2 = i1+1;
            }
            // Now i1 is the closest node visible from p.
            if (firstEdgeVisible) {
               const siZe_t i0 = (dir==1 ? (i1==0 ? nb-1 : i1-1) :
                     (i1==nb-1 ? 0 : i1+1));
               const std::pair<double,double> & p0 (_nngr.bPoint (i0));
               const std::pair<double,double> v0 (p[0] - p0.first,
                     p[1] - p0.second);
               const std::pair<double,double> & p1 (_nngr.bPoint (i1));
               const std::pair<double,double> v1 (p1.first - p0.first,
                     p1.second - p0.second);
               const double v1norm2 = v1.first*v1.first + v1.second*v1.second;
               const double a = (v1.first*v0.first + v1.second*v0.second) / 
                  v1norm2;
               if (a<1.0) {
                  std::vector<siZe_t> ic (2);
                  ic[0] = _nngr.boundary (i0);
                  ic[1] = _nngr.boundary (i1);
                  _icoef.push_back (ic);
                  std::vector<double> c (2);
                  c[0] = 1.0 - a;
                  c[1] = a;
                  _coef.push_back (c);
                  continue;
               }
            }
            if (lastEdgeVisible) {
               const std::pair<double,double> & p2 (_nngr.bPoint (i2));
               const std::pair<double,double> v2 (p[0] - p2.first,
                     p[1] - p2.second);
               const std::pair<double,double> & p1 (_nngr.bPoint (i1));
               const std::pair<double,double> v1 (p1.first - p2.first,
                     p1.second - p2.second);
               const double v1norm2 = v1.first*v1.first + v1.second*v1.second;
               const double a = (v1.first*v2.first + v1.second*v2.second) /
                  v1norm2;
               if (a<1.0) {
                  std::vector<siZe_t> ic (2);
                  ic[0] = _nngr.boundary (i2);
                  ic[1] = _nngr.boundary (i1);
                  _icoef.push_back (ic);
                  std::vector<double> c (2);
                  c[0] = 1.0 - a;
                  c[1] = a;
                  _coef.push_back (c);
                  continue;
               }
            }
            _icoef.push_back (std::vector<siZe_t> (1,_nngr.boundary (i1)));
            _coef.push_back (std::vector<double> (1, 1.0));
         }
         const siZe_t nin = _ipointsInside.size();
         if (nin != 0) { // Some points are in the convex hull
            std::vector<double> xout, yout;
            xout.reserve (nin);
            yout.reserve (nin);
            for (siZe_t i : _ipointsInside) {
               const double * p (&outPoints[tableWidth*i]);
               xout.push_back (p[0]);
               yout.push_back (p[1]);
            }
            _nnai = nnai_build (&extgrid.triangulation(), nin, xout.data(),
                  yout.data());
            nnai_setwmin (_nnai, -1e-10);
         }
#ifdef DEBUG_EXTENDEDNN
         for (siZe_t i=0; i<_coef.size(); i++) {
            std::cout << outPoints[tableWidth*_ipointsOutside[i]] << " " << outPoints[tableWidth*_ipointsOutside[i]+1] << " coef: " << _coef[i] << " icoef: " << _icoef[i] << "\n";
         }
#endif
      }

      //........................................................................

      ~ExtendedNNCoordinates()
      {
         if (_nnai!=NULL) nnai_destroy (_nnai);
      }

      inline const std::vector<double> interpolate
         (const std::vector<double> & inValues) const
      {
         const siZe_t insz = _ipointsInside.size();
         const siZe_t outsz = _ipointsOutside.size();
         std::vector<double> ret (insz+outsz);
         interpolate (inValues.data(), ret.data());
         return ret;
      }

      inline void interpolate (const double * inValues, double * ret,
            const size_t inWidth = 1, const size_t dimension = 1) const
      {
         const siZe_t insz = _ipointsInside.size();
         const siZe_t outsz = _ipointsOutside.size();
         for (siZe_t i=0; i<outsz; i++) {
            for (size_t idim=0; idim<dimension; idim++) {
               double & reti (ret[_ipointsOutside[i]*dimension+idim]);
               reti = 0.0;
               const auto & ic (_icoef[i]);
               const auto & c (_coef[i]);
               const siZe_t sz = ic.size();
               for (siZe_t j=0; j<sz; j++) {
                  reti += c[j] * inValues[ic[j]*inWidth+idim];
               }
            }
         }

         if (insz != 0) {
            std::vector<double> ret1 (insz*dimension);
            nnai_interpolate_flex (_nnai, inValues, ret1.data(), inWidth,
                  dimension);
            for (siZe_t i=0; i<insz; i++)
               memcpy (&(ret[_ipointsInside[i]*dimension]),
                     &(ret1[i*dimension]), dimension*sizeof (double));
         }
      }
      private:
      const ExtendedNNGrid & _nngr;
      nnai * _nnai;
      std::vector<std::vector<siZe_t> > _icoef;
      std::vector<std::vector<double> > _coef;
      std::vector<siZe_t> _ipointsInside;
      std::vector<siZe_t> _ipointsOutside;
   };
}

#endif
