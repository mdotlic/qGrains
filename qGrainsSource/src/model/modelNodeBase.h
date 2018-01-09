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
#ifndef MODELNODEBASE_H
#define MODELNODEBASE_H
#include <QObject>
#include <QVariant>

class ModelNodeBase : public QObject
{
   Q_OBJECT
public:
   ModelNodeBase (ModelNodeBase * parent = 0, const int numberOfProperties = 0);
   virtual ~ModelNodeBase();
   virtual QVariant data (int row, int col) const;
   virtual bool setData (int, int, QVariant);
   ModelNodeBase * child (int row) {return _children[row]; }
   const ModelNodeBase * child (int row) const {return _children[row]; }
   std::vector<ModelNodeBase*> * children() { return &_children; }
   void appendChild (ModelNodeBase * child);
   void insertChild (ModelNodeBase * child, size_t i);
   void removeChildren(const int first, const int len);
   ModelNodeBase * parent () { return _parent; }
   virtual int rowCount() const { return _children.size(); }
   virtual int row() const;
   virtual int columnCount() const { return (*_properties).size(); }
   virtual Qt::ItemFlags flags (int row, int col) const;
   const std::vector<QVariant> & properties() const { return *_properties; }
   void setProperty (const QVariant & p, const int i);
   void setName (const QString & s);
   const QString & name() const;
signals:
   void addedDrill(const int &);
   void deletedDrill(const int &);
   void deletedSample(const int &, const int &);
   void loadPlotTrue(const int &, const int &);
   void loadCondTableTrue(const int &, const int &);
   void loadFinished();
   void modelChanged();
   void loadDrill(const std::vector<double> &, const QString &);
   void loadDepth(const double &, const double &, const QModelIndex &);
   void loadCoor(const double *, const QModelIndex &);
   void imageLoad();
private slots:
   void modelChSlot();

protected:
   std::vector<ModelNodeBase*> _children;
   QString * _name;
   std::vector<QVariant> * _properties;
private:
   ModelNodeBase * const _parent;
};

#endif
