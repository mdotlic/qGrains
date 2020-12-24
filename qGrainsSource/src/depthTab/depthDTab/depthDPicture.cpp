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
#include "depthDPicture.h"
#include "model/handlers/viewHandler.h"
#include "model/handlers/dHandler.h"
#include "model/handlers/handler.h"
#include "model/modelNodeBase.h"
#include "qGrains.h"
#include "centralWindow.h"
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


DepthDPicture::DepthDPicture(Handler * handler, ViewHandler * viewHandler, DHandler * dHandler, QGrains * qGrains):_viewHandler(viewHandler), _dHandler(dHandler), _qGrains(qGrains), _handler(handler)
{
   this->xAxis->setScaleType(QCPAxis::stLogarithmic);//log scale x axis
   this->xAxis2->setScaleType(QCPAxis::stLogarithmic);//log scale x axis
   QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
   this->xAxis->setTicker(logTicker);
   this->xAxis2->setTicker(logTicker);
   this->xAxis->grid()->setSubGridVisible(true);
   //color of grid changed to black
   QPen pen(this->xAxis->grid()->pen());
   pen.setColor(Qt::GlobalColor(Qt::black));
   this->xAxis->grid()->setPen(pen);
   this->yAxis->grid()->setPen(pen);
   this->xAxis->grid()->setSubGridPen(pen);
   this->yAxis->grid()->setSubGridPen(pen);

   this->xAxis2->setVisible(true);
   this->xAxis2->setTickLabels(false);
   this->yAxis2->setVisible(true);
   this->yAxis2->setTickLabels(true);
   this->yAxis->setRangeReversed(true);
   this->yAxis2->setRangeReversed(true);
   this->xAxis->setLabel("d [mm]");
   this->yAxis->setLabel("depth [m]");

   this->legend->setVisible(_isLegend);

   connect(dHandler, SIGNAL(dSelectionChange(const int &)), this, 
         SLOT(dSelChangeSlot(const int &)));
   connect(viewHandler, SIGNAL(drillSelectionChange(const int &)), this, 
         SLOT(drillSelChangeSlot(const int &)));
   connect(dHandler, SIGNAL(styleChange(const int &)), this, 
         SLOT(styleChangeSlot(const int &)));

   connect(dHandler->model(), SIGNAL(loadFinished()), this, 
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

   connect(dHandler->model(), SIGNAL(addedDrill(const int &)), this, 
         SLOT(addGraphForDrill(const int &)));
   connect(dHandler->model(), SIGNAL(deletedDrill(const int &)), this,
         SLOT(deleteGraphsForDrill(const int &)));
   connect(viewHandler, SIGNAL(drillTickChange(const int &)), this,
         SLOT(drillTickSlot(const int &)));
}

void DepthDPicture::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Set range", this, SLOT(setRange()));
   menu->addAction("Save image", this, SLOT(saveImage()));
   if(!_handler->useDElev())
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

void DepthDPicture::setRange()
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

void DepthDPicture::saveImage()
{
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Save image"),".",tr("*.jpg"));
   if (fileName.isEmpty()) 
      return;
   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");
   saveJpg(fileName);
}

void DepthDPicture::changeIsDepth()
{
   _handler->setUseDElev(!_handler->useDElev());
   allPlotSlot();
}

void DepthDPicture::selectionChanged()
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
/*  for (int i=0; i<this->graphCount(); ++i)
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

void DepthDPicture::mouseWheel()
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

/*
void DrillPicture::setGraphStyle(const int & id, const int & iGraph)
{
   QPen lineStyle((Qt::PenStyle) _dHandler->dStyle(id));
   lineStyle.setColor(Qt::GlobalColor(_color[_dHandler->dColor(id)]));   
   lineStyle.setWidth(2);
   this->graph(iGraph)->setPen(lineStyle);
   this->graph(iGraph)->setLineStyle(QCPGraph::lsControlImpulse);
   this->graph(iGraph)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
}*/

void DepthDPicture::setDataForGraph(const int & idrill, const int & id)
{
   QVector<double> x;
   QVector<double> y;
   if(_dHandler->isCheckedD(id) && _viewHandler->isCheckedDrill(idrill))
   {
      int nSamples = _viewHandler->nSamples(idrill);
      for(int j=0; j<nSamples;j++)
      {
         double val1, val2;
         if(_handler->useDElev())
         {
            val1 = _viewHandler->sampleVal(idrill, j, 0);
            val2 = _viewHandler->sampleVal(idrill, j, 1);
         }else{
            val1=_handler->elev(idrill)-_viewHandler->sampleVal(idrill, j, 0);
            val2=_handler->elev(idrill)-_viewHandler->sampleVal(idrill, j, 1);
         }

         int nPoints = _handler->nPoints(idrill, j);
         for(int k=0; k<nPoints; k++)
         {
            double xval = _handler->pointGrainSize(idrill, j, k);
            double perc = _handler->pointPercent(idrill, j, k);
            if(fabs(perc-_d[id]) < _handler->tolerance())
            {
               int place = 
                  std::lower_bound(x.begin(), x.end(), xval)-x.begin();
               x.insert(x.begin()+place, xval);
               y.insert(y.begin()+place, val1);
               x.insert(x.begin()+place, xval);
               y.insert(y.begin()+place, val2);
            }
         }
      }
      this->graph(idrill*6+id)->addToLegend();
   }
   else
   {
      this->graph(idrill*6+id)->removeFromLegend();
   }
   this->graph(idrill*6+id)->setData(x, y, true);
   this->graph(idrill*6+id)->rescaleAxes(true);

}

void DepthDPicture::showCoord(QMouseEvent * event)
{
   double x = this->xAxis->pixelToCoord(event->pos().x());
   double y = this->yAxis->pixelToCoord(event->pos().y());
   _qGrains->statusBar()->showMessage(QString("%1 , %2").arg(x).arg(y));

}

void DepthDPicture::showHideLegend()
{
   _isLegend = !_isLegend;
   this->legend->setVisible(_isLegend);
   this->replot();
}

void DepthDPicture::setTitle()
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

void DepthDPicture::dSelChangeSlot(const int & id)
{
  // bool isChecked = _dHandler->isCheckedD(id);
  // if(isChecked)
  // {
   for(int i=0; i<_viewHandler->nDrills();i++)
   {
      styleChangeSlot(id);
      QPen lineStyle((Qt::PenStyle) _dHandler->dStyle(id));
      lineStyle.setColor(Qt::GlobalColor(_color[_dHandler->dColor(id)]));   
      lineStyle.setWidth(2);
      this->graph(i*6+id)->setPen(lineStyle);
      //this->graph(i*6+id)->setLineStyle(QCPGraph::lsControlImpulse);
      setDataForGraph(i, id);

   }
   this->replot();
}

void DepthDPicture::drillSelChangeSlot(const int & idrill)
{
   for(int id = 0; id<6; id++)
   {
      styleChangeSlot(id);
      QPen lineStyle((Qt::PenStyle) _dHandler->dStyle(id));
      lineStyle.setColor(Qt::GlobalColor(_color[_dHandler->dColor(id)]));   
      lineStyle.setWidth(2);
      this->graph(idrill*6+id)->setPen(lineStyle);
      //this->graph(idrill*6+id)->setLineStyle(QCPGraph::lsControlImpulse);
      setDataForGraph(idrill, id);
      //else
      //{
      // qDebug()<<" odstiklirano \n";
      // }
   }
   this->replot();
}

void DepthDPicture::styleChangeSlot(const int & id)
{
   QPen lineStyle((Qt::PenStyle) _dHandler->dStyle(id));
   lineStyle.setColor(Qt::GlobalColor(_color[_dHandler->dColor(id)]));   
   lineStyle.setWidth(2);
   for(int i=0;i<_viewHandler->nDrills();i++)
      this->graph(i*6+id)->setPen(lineStyle);
   //this->graph(id)->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape) _dHandler->dTick(id),5));
   this->replot();
}

void DepthDPicture::addGraphForDrill(const int & idrill)
{//for every drill we have 6 graphs
   for(size_t i=0;i<6;i++)
   {
      this->addGraph();
      this->graph(idrill*6+i)->setLineStyle(QCPGraph::lsControlImpulse);
      this->graph(idrill*6+i)->setName(_handler->drillName(idrill)+" - "+_dHandler->dName(i));
      this->graph(idrill*6+i)->removeFromLegend();
   }
}

void DepthDPicture::deleteGraphsForDrill(const int & idrill)
{
   for(size_t i=0;i<6;i++)
      this->removeGraph(idrill*6+i);
}

void DepthDPicture::drillTickSlot(const int & idrill)
{
   for(size_t id=0;id<6;id++)
      this->graph(idrill*6+id)->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape) _viewHandler->drillTick(idrill),5));
   this->replot();
}

void DepthDPicture::allPlotSlot()
{
   if(!_handler->useDElev())
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
   for(int idrill=0; idrill<_viewHandler->nDrills(); idrill++)
   {
      if(_viewHandler->isCheckedDrill(idrill))
      {
         drillTickSlot(idrill);
         for(size_t id=0; id<6; id++)
         {
            if(_dHandler->isCheckedD(id))
            {
               setDataForGraph(idrill,id);
               styleChangeSlot(id);
            }
         }
      }
   }
   replot();
}
