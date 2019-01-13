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
#include "qGrains.h"
#include "percPicture.h"
#include "model/handlers/viewHandler.h"
#include "model/handlers/sizeHandler.h"
#include "model/handlers/handler.h"
#include "model/modelNodeBase.h"
#include "centralWindow.h"
#include "calculation/calculation.h"
//#include "model/pointSet.h"
//#include "model/modelEnums.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QStatusBar>
#include <QDialog>

PercPicture::PercPicture(Handler * handler, ViewHandler * viewHandler, SizeHandler * sizeHandler, QGrains * qGrains):_viewHandler(viewHandler), _sizeHandler(sizeHandler), _qGrains(qGrains), _handler(handler)
{
   this->xAxis2->setVisible(true);
   this->xAxis2->setTickLabels(false);
   this->yAxis2->setVisible(true);
   this->yAxis2->setTickLabels(true);
   this->yAxis->setRangeReversed(true);
   this->yAxis2->setRangeReversed(true);
   this->xAxis->setLabel("%");
   this->yAxis->setLabel("depth [m]");

   //color of grid changed to black
   QPen pen(this->xAxis->grid()->pen());
   pen.setColor(Qt::GlobalColor(Qt::black));
   this->xAxis->grid()->setPen(pen);
   this->yAxis->grid()->setPen(pen);
   this->xAxis->grid()->setSubGridPen(pen);
   this->yAxis->grid()->setSubGridPen(pen);

   this->legend->setVisible(_isLegend);

   connect(sizeHandler, SIGNAL(sizeSelectionChange(const int &)), this, 
         SLOT(sizeSelChangeSlot(const int &)));

   connect(viewHandler, SIGNAL(drillPercSelectionChange(const int &)), this, 
         SLOT(drillSelChangeSlot(const int &)));
   connect(sizeHandler, SIGNAL(styleChange(const int &)), this, 
         SLOT(styleChangeSlot(const int &)));

   connect(sizeHandler->model(), SIGNAL(loadFinished()), this, 
         SLOT(allPlotSlot()));

   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));

   connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
   connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
//   connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
   //radi this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
   connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
   this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
         QCP::iSelectLegend | QCP::iSelectPlottables);
   connect(this, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showCoord(QMouseEvent*)));

   connect(sizeHandler->model(), SIGNAL(addedDrill(const int &)), this, 
         SLOT(changeGraphSize(const int &)));
   connect(sizeHandler->model(), SIGNAL(deletedDrill(const int &)), this,
         SLOT(changeGraphSize(const int &)));
   connect(viewHandler, SIGNAL(drillPercTickChange(const int &)), this,
         SLOT(drillTickPercSlot(const int &)));
   connect(_sizeHandler, SIGNAL(sizeAppended(const int &)), this, 
         SLOT(changeGraphSize(const int &)));
}

void PercPicture::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Set range", this, SLOT(setRange()));
   menu->addAction("Save image", this, SLOT(saveImage()));
   if(_isDepthPlot)
      menu->addAction("Change y axis to elevation", this,
            SLOT(changeIsDepth()));
   else
      menu->addAction("Change y axis to depth", this, 
            SLOT(changeIsDepth()));
   if(_isLegend)
      menu->addAction("Hide legend", this, SLOT(showHideLegend()));
   else
      menu->addAction("Show legend", this, SLOT(showHideLegend()));
   menu->addAction("Set title", this, SLOT(setTitle()));

   menu->popup(mapToGlobal(pos));
}

void PercPicture::setRange()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set range");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * xminLabel = new QLabel("Xmin:");
   layout->addWidget(xminLabel,0,0,1,1);
   QLineEdit * xminLineEdit = new QLineEdit(dialog);
   xminLineEdit->setText(QString::number(this->xAxis->range().lower));
   layout->addWidget(xminLineEdit,0,1,1,1);
   
   QLabel * xmaxLabel = new QLabel("Xmax:");
   layout->addWidget(xmaxLabel,0,2,1,1);
   QLineEdit * xmaxLineEdit = new QLineEdit(dialog);
   xmaxLineEdit->setText(QString::number(this->xAxis->range().upper));
   layout->addWidget(xmaxLineEdit,0,3,1,1);

   QLabel * yminLabel = new QLabel("Ymin:");
   layout->addWidget(yminLabel,1,0,1,1);
   QLineEdit * yminLineEdit = new QLineEdit(dialog);
   yminLineEdit->setText(QString::number(this->yAxis->range().lower));
   layout->addWidget(yminLineEdit,1,1,1,1);

   QLabel * ymaxLabel = new QLabel("Ymax:");
   layout->addWidget(ymaxLabel,1,2,1,1);
   QLineEdit * ymaxLineEdit = new QLineEdit(dialog);
   ymaxLineEdit->setText(QString::number(this->yAxis->range().upper));
   layout->addWidget(ymaxLineEdit,1,3,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      
      this->xAxis->setRange(xminLineEdit->text().toDouble(), xmaxLineEdit->text().toDouble());
      this->yAxis->setRange(yminLineEdit->text().toDouble(), ymaxLineEdit->text().toDouble());
      this->replot();
   }
}

void PercPicture::saveImage()
{
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Save image"),".",tr("*.jpg"));
   if (fileName.isEmpty()) 
      return;
   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");
   saveJpg(fileName);
}

void PercPicture::changeIsDepth()
{
   _isDepthPlot = !_isDepthPlot;
   if(_isDepthPlot)
   {
      this->yAxis->setLabel("depth [m]");
      this->yAxis->setRangeReversed(true);
      this->yAxis2->setRangeReversed(true);
   }
   else
   {
      this->yAxis->setLabel("elevation [m.a.s.l.]");
      this->yAxis->setRangeReversed(false);
      this->yAxis2->setRangeReversed(false);
   }
   allPlotSlot();
}

void PercPicture::selectionChanged()
{
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
 /* for (int i=0; i<this->graphCount(); ++i)
  {
    QCPGraph *graph = this->graph(i);
    QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }*/

}

void PercPicture::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    this->axisRect()->setRangeZoom(this->xAxis->orientation());
  else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    this->axisRect()->setRangeZoom(this->yAxis->orientation());
  else
    this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void PercPicture::setDataForGraph(const int & idrill, const int & id)
{
   QVector<double> x;
   QVector<double> y;
   int nsizes = _sizeHandler->nsizes();
   if(_sizeHandler->isChecked(id) && _viewHandler->isCheckedDrillPerc(idrill))
   {
      int nSamples = _viewHandler->nSamples(idrill);
      for(int j=0; j<nSamples;j++)
      {
         double val1, val2;
         if(_isDepthPlot)
         {
            val1 = _viewHandler->sampleVal(idrill, j, 0);
            val2 = _viewHandler->sampleVal(idrill, j, 1);
         }else{
            val1 = _handler->elev(idrill)-_viewHandler->sampleVal(idrill, j, 0);
            val2 = _handler->elev(idrill)-_viewHandler->sampleVal(idrill, j, 1);
         }
         
         int nPoints = _handler->nPoints(idrill, j);
         if(nPoints>0)
         {
            double down=0.0;

            if(_handler->pointGrainSize(idrill, j, 0)>_sizeHandler->from(id))
               down = 0;//if there is no smaller grain size than from
            else if(_handler->pointGrainSize(idrill, j, nPoints-1) 
                  <_sizeHandler->from(id))
               down = 100;//if there is no bigger grain size than from
            else
            {
               for(int k=1; k<nPoints; k++)
               {
                  if(_handler->pointGrainSize(idrill, j, k) > 
                        _sizeHandler->from(id))
                  {//first bigger grain size
                     down = Calculation::interp(_sizeHandler->from(id), 
                           _handler->pointGrainSize(idrill, j, k-1),
                           _handler->pointGrainSize(idrill, j, k),
                           _handler->pointPercent(idrill, j, k-1),
                           _handler->pointPercent(idrill, j, k));
                     break;
                  }
               }
            }
            double up = 0.0;
            if(_handler->pointGrainSize(idrill, j, 0)>_sizeHandler->to(id))
               up = 0;//if there is no smaller grain size than to
            else if(_handler->pointGrainSize(idrill, j, nPoints-1) 
                  <=_sizeHandler->to(id))
               up = 100;//if there is no bigger grain size than to
            else
            {
               for(int k=1; k<nPoints; k++)
               {
                  if(_handler->pointGrainSize(idrill, j, k) >
                        _sizeHandler->to(id))
                  {//first bigger grain size
                     up = Calculation::interp(_sizeHandler->to(id), 
                           _handler->pointGrainSize(idrill, j, k-1),
                           _handler->pointGrainSize(idrill, j, k),
                           _handler->pointPercent(idrill, j, k-1),
                           _handler->pointPercent(idrill, j, k));
                     break;
                  }
               }
            }
            if(idrill==50)
               qDebug()<<" ovo bi bilo "<<up<<" "<<down<<" jer je "<<_handler->pointGrainSize(idrill, j, 0)<<" "<<_sizeHandler->to(id)<<"\n";
            double perc = up-down;
            int place = 
               std::lower_bound(x.begin(), x.end(), perc)-x.begin();
            x.insert(x.begin()+place, perc);
            y.insert(y.begin()+place, val1);
            x.insert(x.begin()+place, perc);
            y.insert(y.begin()+place, val2);
         }
      }
      this->graph(idrill*nsizes+id)->addToLegend();
   }
   else
      this->graph(idrill*nsizes+id)->removeFromLegend();
   
   this->graph(idrill*nsizes+id)->setData(x, y, true);
   this->graph(idrill*nsizes+id)->rescaleAxes(true);

}

void PercPicture::showCoord(QMouseEvent * event)
{
   double x = this->xAxis->pixelToCoord(event->pos().x());
   double y = this->yAxis->pixelToCoord(event->pos().y());
   _qGrains->statusBar()->showMessage(QString("%1 , %2").arg(x).arg(y));
}

void PercPicture::showHideLegend()
{
   _isLegend = !_isLegend;
   this->legend->setVisible(_isLegend);
   this->replot();
}

void PercPicture::setTitle()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Set title");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);

   QFrame * captFrame = new QFrame;
   QGridLayout * captLayout = new QGridLayout;

   QLabel * captLabel = new QLabel("Text");
   captLayout->addWidget(captLabel,1,0);
   QPlainTextEdit * captText = new QPlainTextEdit(&dialog);
   captText->document()->setPlainText(_caption);
   captLayout->addWidget(captText,1,1);
   
   captFrame->setLayout(captLayout);
   captFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(captFrame);
   
   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;
   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);

   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   if (dialog.exec() == QDialog::Accepted) {
      _caption = captText->document()->toPlainText();
      if(this->plotLayout()->elementCount()==2)
      {
         this->plotLayout()->removeAt(0);
         this->plotLayout()->simplify();
         this->plotLayout()->updateLayout();
      }
      if(!_caption.isEmpty())
      {
         this->plotLayout()->insertRow(0);
         this->plotLayout()->addElement(0, 0, new QCPTextElement(this, 
                  _caption, QFont("sans", 12, QFont::Bold)));
      }
      this->replot();
   }
}

void PercPicture::sizeSelChangeSlot(const int & id)
{
   int nsizes = _sizeHandler->nsizes();
   for(int i=0; i<_viewHandler->nDrills();i++)
   {
      styleChangeSlot(id);
      QPen lineStyle((Qt::PenStyle) _sizeHandler->sizeStyle(id));
      lineStyle.setColor(Qt::GlobalColor(_color[_sizeHandler->sizeColor(id)]));   
      lineStyle.setWidth(2);
      this->graph(i*nsizes+id)->setPen(lineStyle);
      //this->graph(i*6+id)->setLineStyle(QCPGraph::lsControlImpulse);
      setDataForGraph(i, id);

   }
   this->replot();

}

void PercPicture::drillSelChangeSlot(const int & idrill)
{
   int nsizes = _sizeHandler->nsizes();
   for(int id = 0; id<nsizes; id++)
   {
     // bool isChecked = _dHandler->isCheckedD(id);
     // if(isChecked)
     // {
         styleChangeSlot(id);
         QPen lineStyle((Qt::PenStyle) _sizeHandler->sizeStyle(id));
         lineStyle.setColor(Qt::GlobalColor(_color[_sizeHandler->sizeColor(id)]));   
         lineStyle.setWidth(2);
         this->graph(idrill*nsizes+id)->setPen(lineStyle);

         setDataForGraph(idrill, id);
     // }
      //else
      //{
        // qDebug()<<" odstiklirano \n";
     // }
   }
   this->replot();

}

void PercPicture::styleChangeSlot(const int & id)
{
   QPen lineStyle((Qt::PenStyle) _sizeHandler->sizeStyle(id));
   lineStyle.setColor(Qt::GlobalColor(_color[_sizeHandler->sizeColor(id)]));   
   lineStyle.setWidth(2);
   int nsizes = _sizeHandler->nsizes();
   for(int i=0;i<_viewHandler->nDrills();i++)
      this->graph(i*nsizes+id)->setPen(lineStyle);
   //this->graph(id)->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape) _sizeHandler->sizeTick(id),5));
   this->replot();
}

void PercPicture::changeGraphSize(const int &)
{
   int nsizes = _sizeHandler->nsizes();
   int nDrills = _viewHandler->nDrills();
   this->clearPlottables();
   this->legend->setVisible(false);
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   for(int idrill=0; idrill<nDrills; idrill++)
   {
      for(int isize=0; isize<nsizes; isize++)
      {
         this->addGraph();
         this->graph(idrill*nsizes+isize)->setLineStyle(QCPGraph::lsControlImpulse);
         this->graph(idrill*nsizes+isize)->setName(_handler->drillName(idrill)+" - "+QString::number(_sizeHandler->from(isize))+"-"+QString::number(_sizeHandler->to(isize)));
      }
   }
   allPlotSlot();
   QApplication::restoreOverrideCursor();
   this->legend->setVisible(true);
   this->replot();

}

void PercPicture::drillTickPercSlot(const int & idrill)
{
   int nsizes = _sizeHandler->nsizes();
   for(int id=0;id<nsizes;id++)
      this->graph(idrill*nsizes+id)->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape) _viewHandler->drillPercTick(idrill),5));
   this->replot();
}

void PercPicture::allPlotSlot()
{
   int nsizes = _sizeHandler->nsizes();
   int nDrills = _viewHandler->nDrills();
   for(int idrill=0;idrill<nDrills;idrill++)
   {
      drillTickPercSlot(idrill);
      for(int isize=0; isize<nsizes; isize++)
      {
         setDataForGraph(idrill, isize);
         styleChangeSlot(isize);
      }
   }

   replot();
}

