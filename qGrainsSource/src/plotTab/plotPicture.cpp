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
#include "plotPicture.h"
#include "model/handlers/plotHandler.h"
#include "model/handlers/inputHandler.h"
#include "model/modelNodeBase.h"
#include "qGrains.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QStatusBar>
#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QPlainTextEdit>
#include <QPainterPathStroker>
#include <math.h>

PlotPicture::PlotPicture(QGrains * qGrains, PlotHandler * plotHandler, 
      InputHandler * inputHandler): _qGrains(qGrains), _plotHandler(plotHandler)
{
   QSize newSize(width(),height());
   _image=new QImage(newSize, QImage::Format_RGB32);

   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuPlotTab(QPoint)));

   connect(_plotHandler, SIGNAL(drawSpline(const int, const int)), this,
         SLOT(drawSplineSlot(const int, const int)));
   connect(_plotHandler, SIGNAL(changeStyle()), this,
         SLOT(changeStyleSlot()));

   connect(_plotHandler, SIGNAL(sampleSelected(const int &, const int &)), this,
         SLOT(sampleSelectedSlot(const int &, const int &)));

   connect(_plotHandler->model(), SIGNAL(loadPlotTrue(const int &, const int &)), this, SLOT(drawSplineSlot(const int &, const int &)));

   connect(inputHandler, SIGNAL(deleteSpline(const int &, const int &)), this,
         SLOT(deleteSplineSlot(const int &, const int &)));

   setMouseTracking(true);
}

void PlotPicture::mousePressEvent(QMouseEvent * event)
{
   if(event->buttons() == Qt::LeftButton)
   {
      _lastDragPos = event->pos();
      _drag=true;
   }
}

void PlotPicture::mouseMoveEvent(QMouseEvent * event)
{
   if(_drag)
   {
      double moveX = event->pos().x()-_lastDragPos.x();
      double moveY = event->pos().y()-_lastDragPos.y();

      _transform_b += moveX;
      _transform_d += moveY;
      _lastDragPos=event->pos();
      repaint();
   }
   {
      double x = inverseTransformX(event->pos().x());
      double y = inverseTransformY(event->pos().y());
     // double xval = 0.0;
     // double yval = 0.0;
      if((x <= _end.x() && x >= _origin.x()) && 
         (y <= _end.y() && y >= _origin.y()) )
      {//then it is in rectangle
         //qDebug()<<" ovo bi bilo "<<100.0-100*(y-_origin.y())/(_end.y()-_origin.y())<<" donja "<<_bVal[_leftBIndex].toDouble()*pow(10, exp)<<"\n";
         QString str(QString::number(logTransfX(event->pos().x())));
         str.append("; ");
         str.append(QString::number(logTransfY(event->pos().y())));//y axis
/*
         QString str(QString::number(event->pos().x()));
         str.append("; ");
         str.append(QString::number(event->pos().y()));
*/
         _qGrains->statusBar()->showMessage(str);
         repaint();
      }

      //qDebug()<<point.x()<<"; "<<point.y()<<" evo me na poziciji ("<<event->pos().x()<<"; "<<event->pos().y()<<") dok je ("<<_origin.x()<<";"<<_origin.y()<<") -- ("<<_end.x()<<";"<<_end.y()<<"\n";
   }
}

void PlotPicture::wheelEvent(QWheelEvent * event)
{
   double zoomInFactor=0.95;
   int numDegrees = -event->delta() / 8;
   double numSteps = numDegrees / 15.0f;
   double zoom=pow(zoomInFactor, numSteps);
   _transform_a*=zoom;
   _transform_b=zoom*_transform_b + event->pos().x()*(1-zoom);
   _transform_c*=zoom;
   _transform_d=zoom*_transform_d + event->pos().y()*(1-zoom);

   repaint();
}

void PlotPicture::mouseReleaseEvent(QMouseEvent * event)
{
   _drag=false;
   for(auto& it:_splines)
   {
      if(it.second.contains(event->pos()))      
      {
         int idrill=std::get<0> (it.first);
         int jsample=std::get<1> (it.first);
         _selected = it.first;
         emit _plotHandler->splineSelected(idrill, jsample);
         repaint();
         return;
      }
   }
   _selected = std::make_pair(-1,-1);
   emit _plotHandler->deselectedSpline();
   repaint();
}

void PlotPicture::contextMenuPlotTab(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Set bounds", this, SLOT(setBounds()));
   menu->addAction("Properties", this, SLOT(properties()));
   menu->addAction("Save image", this, SLOT(savePic()));
   if(_showLegend)
      menu->addAction("Hide legend", this, SLOT(showHideLegend()));
   else
      menu->addAction("Show legend", this, SLOT(showHideLegend()));
   menu->popup(mapToGlobal(pos));
}

void PlotPicture::setBounds()
{
   QDialog * dialog = new QDialog(this);
   QGridLayout * setBoundsLayout = new QGridLayout;
   QLabel * left = new QLabel("Left end");
   setBoundsLayout->addWidget(left, 0,0, 1,1);
   QComboBox * leftCombo = new QComboBox;
   QComboBox * rCombo = new QComboBox;
   for(size_t i=0;i<_bVal.size();i++)
   {
      leftCombo->addItem(_bVal[i]);
      rCombo->addItem(_bVal[i]);
   }

   leftCombo->setCurrentIndex(_leftBIndex);//left combo
   setBoundsLayout->addWidget(leftCombo, 0, 1, 1, 1);
   QLabel * right = new QLabel("Right end");
   setBoundsLayout->addWidget(right, 1,0, 1,1);
   
   rCombo->setCurrentIndex(_rightBIndex);//left combo
   setBoundsLayout->addWidget(rCombo, 1,1,1,1);
   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, dialog);
   setBoundsLayout->addWidget(&buttonBox, 2,0, 1,1);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
   dialog->setLayout(setBoundsLayout);
   if (dialog->exec() == QDialog::Accepted)
   {
      _leftBIndex = leftCombo->currentIndex();
      _rightBIndex = rCombo->currentIndex();
      repaint();
   }
}

QComboBox * addComBox(int defColor)
{
   QComboBox * comboBox = new QComboBox;
   comboBox->addItem(QIcon(":resources/images/colors/black.png"), QString("black"));
   comboBox->addItem(QIcon(":resources/images/colors/red.png"), QString("red"));
   comboBox->addItem(QIcon(":resources/images/colors/blue.png"), QString("blue"));
   comboBox->addItem(QIcon(":resources/images/colors/green.png"), QString("green"));
   comboBox->addItem(QIcon(":resources/images/colors/yellow.png"), QString("yellow"));
   comboBox->addItem(QIcon(":resources/images/colors/cyan.png"), QString("cyan"));
   comboBox->addItem(QIcon(":resources/images/colors/magenta.png"), QString("magenta"));
   comboBox->addItem(QIcon(":resources/images/colors/gray.png"), QString("gray"));
   comboBox->setCurrentIndex(defColor);//because default is blue
   for(int i = 0; i < comboBox->count(); i++)
    comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
   return comboBox;
}

void PlotPicture::properties()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Properties");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);

   QFrame * captFrame = new QFrame;
   QGridLayout * captLayout = new QGridLayout;
   QLabel * labelCapt = new QLabel("Caption");
   captLayout->addWidget(labelCapt,1,0);

   QLabel * captLabel = new QLabel("Text");
   captLayout->addWidget(captLabel,2,0);
   QPlainTextEdit * captText = new QPlainTextEdit(&dialog);
   captText->document()->setPlainText(_caption);
   captLayout->addWidget(captText,2,1);
   
   captFrame->setLayout(captLayout);
   captFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(captFrame);

   QFrame * rectFrame = new QFrame;
   QGridLayout * rectLayout = new QGridLayout;
   QLabel * labelRect = new QLabel("Rectangle");
   rectLayout->addWidget(labelRect,1,0);
   
   QLabel * labelRectColor = new QLabel("Rectangle color");
   rectLayout->addWidget(labelRectColor,2,0);
   QComboBox * rectColorCombo=addComBox(_rectColor);
   rectLayout->addWidget(rectColorCombo,2,1);
   
/*   QLabel * labelPointColor = new QLabel("Point color");
   rectTempLayout->addWidget(labelPointColor,3,0);
   QComboBox * pointColorCombo=addComboBox(_pointColor);
   rectTempLayout->addWidget(pointColorCombo,3,1);*/
   
   QLabel * lineRectWidth = new QLabel("Line width");
   rectLayout->addWidget(lineRectWidth,3,0);
   QLineEdit * lineRectWidthEdit = new QLineEdit(&dialog);
   lineRectWidthEdit->setText(QString::number(_lineRectWidth));
   rectLayout->addWidget(lineRectWidthEdit,3,1);
   
   QLabel * lineThinWidth = new QLabel("Thin line width");
   rectLayout->addWidget(lineThinWidth,4,0);
   QLineEdit * lineThinWidthEdit = new QLineEdit(&dialog);
   lineThinWidthEdit->setText(QString::number(_lineThinWidth));
   rectLayout->addWidget(lineThinWidthEdit,4,1);

   QLabel * ratio = new QLabel("Height : width");
   rectLayout->addWidget(ratio,5,0);
   QLineEdit * ratioLineEdit = new QLineEdit(&dialog);
   ratioLineEdit->setText(QString::number(_ratio));
   rectLayout->addWidget(ratioLineEdit,5,1);
   
   QLabel * fontSize = new QLabel("FontSize");
   rectLayout->addWidget(fontSize,6,0);
   QLineEdit * fontSizeLineEdit = new QLineEdit(&dialog);
   fontSizeLineEdit->setText(QString::number(_fontSize));
   rectLayout->addWidget(fontSizeLineEdit,6,1);


/*   QLabel * pointRad = new QLabel("Point radius");
   rectTempLayout->addWidget(pointRad,5,0);
   QLineEdit * pointRadEdit = new QLineEdit(&dialog);
   pointRadEdit->setText(QString::number(_pointRad));
   rectTempLayout->addWidget(pointRadEdit,5,1);*/

   rectFrame->setLayout(rectLayout);
   rectFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(rectFrame);

   QFrame * lineFrame = new QFrame;
   QGridLayout * lineLayout = new QGridLayout;
   QLabel * labelLine = new QLabel("Line");
   lineLayout->addWidget(labelLine,1,0);

   QLabel * lineLineWidth = new QLabel("Line width");
   lineLayout->addWidget(lineLineWidth,2,0);
   QLineEdit * lineLineWidthEdit = new QLineEdit(&dialog);
   lineLineWidthEdit->setText(QString::number(_lineLineWidth));
   lineLayout->addWidget(lineLineWidthEdit,2,1);
   
   QLabel * snap = new QLabel("Snap");
   lineLayout->addWidget(snap,3,0);
   QLineEdit * snapLineEdit = new QLineEdit(&dialog);
   snapLineEdit->setText(QString::number(_snap));
   lineLayout->addWidget(snapLineEdit,3,1);
   
   lineFrame->setLayout(lineLayout);
   lineFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(lineFrame);
   
   QFrame * langFrame = new QFrame;
   QGridLayout * langLayout = new QGridLayout;
   QLabel * langLabel = new QLabel("Language");
   langLayout->addWidget(langLabel,1,0);

   QComboBox * langNames = new QComboBox;
   langNames->addItem("Choose");
   qobject_cast<QStandardItemModel *>(langNames->model())->item(0)->setSelectable( false );
   langNames->addItem("Serbian latin");
   langNames->addItem("Serbian cyrillic");
   langNames->addItem("English");
   langLayout->addWidget(langNames,1,1);

   langFrame->setLayout(langLayout);
   langFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(langFrame);



   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);

   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   if (dialog.exec() == QDialog::Accepted) {
      _rectColor = rectColorCombo->currentIndex();
      _lineRectWidth = lineRectWidthEdit->text().toInt();
      _lineThinWidth = lineThinWidthEdit->text().toInt();
      _lineLineWidth = lineLineWidthEdit->text().toInt();
      _snap = snapLineEdit->text().toInt();
      _ratio = ratioLineEdit->text().toDouble();
      _fontSize = fontSizeLineEdit->text().toInt();
      _end = QPoint (550, _ratio*500+150);
      if(langNames->currentIndex()==1)
         _names = {"zrna u % težine manjih od d", "zrna u % težine većih od d", "prečnik zrna d (mm)", "J.U.S.\nuB1.018", "M.I.T.\nBoston", "GLINA", "fina", "sred", "krup", "PRAŠINA", "prašinast", "sitan", "srednji", "PESAK", "ŠLJUNAK", "OBLUCI"};
      else if(langNames->currentIndex()==2)
         _names = {"зрна у % тежине мањих од d", "зрна у % тежине већих од d", "пречник зрна d (mm)", "Ј.У.С.\nуБ1.018", "М.И.Т.\nБостон", "ГЛИНА", "фина", "сред", "круп", "ПРАШИНА", "прашинаст", "ситан", "средњи", "ПЕСАК", "ШЉУНАК", "ОБЛУЦИ"};
      else if(langNames->currentIndex()==3)
         _names = {"% of grain mass less than d (mm)", "% of grain mass greater than d (mm)", "grain diameter (mm)", "J.U.S.\nuB1.018", "M.I.T.\nBoston", "CLAY", "fine", "med", "coarse", "SILT", "silty", "fine", "medium", "SAND", "GRAVEL", "PEBBLESTONE"};
      _caption = captText->document()->toPlainText();
      repaint();
   }
}

void PlotPicture::drawSplineSlot(const int & idrill, const int & jsample)
{
   if(_plotHandler->isSampleChecked(idrill, jsample))
   {
      _drillSampleSpline.insert(std::make_pair(idrill,jsample)); 
      /*for (std::set<std::pair<int,int> >::iterator it = _drillSampleSpline.begin(); 
            it != _drillSampleSpline.end(); it++) 
      {
         qDebug() << " evo ga "<<std::get<0> (*it)<<" "<<std::get<1> (*it)<<"\n";
      }*/
     /* int npoints = _plotHandler->npoints(idrill, jsample);
      if(npoints<2)
         return;
      std::vector<QPointF> points(npoints);
      for(int i=0;i<npoints;i++)
      {
         points[i]=QPointF(_plotHandler->pointValue(idrill, jsample, i, 0), 
                           _plotHandler->pointValue(idrill, jsample, i, 1));
         qDebug()<<i<<" imas tacke "<<points[i]<<" -- "<<logInvPoint(points[i])<<"\n";
      }
      qDebug()<<" crtaj "<<idrill<<" "<<jsample<<" ima tacaka "<<npoints<<"\n";
      _splinePoints[std::make_pair(idrill, jsample)]=calculateP1(points);
      for(size_t ii=0;ii<_splinePoints[std::make_pair(idrill, jsample)].size();
            ii++)
      {
         qDebug()<<ii<<" tacka "<<_splinePoints[std::make_pair(idrill, jsample)][ii]<<"\n";
      }
      _splinePoints2[std::make_pair(idrill, jsample)]=calculateP2(points,
            _splinePoints[std::make_pair(idrill, jsample)]);
            */
   }else
   {
      _drillSampleSpline.erase(std::make_pair(idrill, jsample));
      /*
      _splinePoints.erase(std::make_pair(idrill, jsample));
      _splinePoints2.erase(std::make_pair(idrill, jsample));

      qDebug()<<" vise ne crtaj "<<idrill<<" "<<jsample<<"\n";*/
   }
   repaint();
}

void PlotPicture::changeStyleSlot()
{
   repaint();
}

void PlotPicture::savePic()
{
   QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save image"),".",tr("JPG Files (*.jpg)"));

   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");

   _image->save(fileName);
}

void PlotPicture::sampleSelectedSlot(const int & idrill, const int & jsample)
{
   _selected = std::make_pair(idrill, jsample);
   repaint();
}

void PlotPicture::showHideLegend()
{
   _showLegend = !_showLegend;
   repaint();
}

void PlotPicture::deleteSplineSlot(const int & idrill, const int & jsample)
{
   _drillSampleSpline.erase(std::make_pair(idrill, jsample));
   for (std::set<std::pair<int,int> >::iterator it = _drillSampleSpline.begin(); it != _drillSampleSpline.end(); it++)
   { 
      if(std::get<0> (*it) == idrill && std::get<1> (*it)>jsample)
      {
         int tmp = std::get<1> (*it);
         _drillSampleSpline.erase(std::make_pair(idrill, tmp));
         _drillSampleSpline.insert(std::make_pair(idrill, tmp-1));
      }
   }
}

void PlotPicture::paintEvent(QPaintEvent * event)
{
   QSize newSize(width(), height());
   delete _image;
   _image=new QImage(newSize, QImage::Format_RGB32);
   _image->fill(qRgb(255, 255, 255));

   QPainter painter(_image);
   painter.setRenderHints( QPainter::Antialiasing, true);
   
   QFont font;
   font.setPixelSize(_fontSize);
   painter.setFont(font);

   drawFrame(painter);
   drawSplines(painter);

   painter.setPen(palette().dark().color());
   painter.setBrush(Qt::NoBrush);
   painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

   QPainter painterW(this);
   QRect dirtyRect = event->rect();
   painterW.drawImage(dirtyRect, *_image, dirtyRect);
}


double PlotPicture::logTransfX(const double x) const
{
   double transfX = inverseTransformX(x);
   double exp = (_rightBIndex-_leftBIndex)*(transfX-_origin.x())/(_end.x()-_origin.x());
   return _bVal[_leftBIndex].toDouble()*pow(10, exp);
}

double PlotPicture::logTransfY(const double y) const
{
   double transfY = inverseTransformY(y);
   return 100.0-100*(transfY-_origin.y())/(_end.y()-_origin.y());
}

int PlotPicture::logInvX(const double x) const
{
   double z = log10(x/_bVal[_leftBIndex].toDouble())*(_end.x()-_origin.x())/
      (_rightBIndex-_leftBIndex)+_origin.x();
   return transformX(z);
}

int PlotPicture::logInvY(const double y) const
{
   double z = (100.0-y)*(_end.y()-_origin.y())/100 + _origin.y();
   return transformY(z);
   //return 100.0-100*(transfY-_origin.y())/(_end.y()-_origin.y());
}

QPointF PlotPicture::logInvPoint(const QPointF & p)
{
   return QPointF(logInvX(p.x()), logInvY(p.y()));
}

int PlotPicture::transformX(const double x) const
{
   return _transform_a*x+_transform_b;
}

int PlotPicture::transformY(const double y) const
{
   return _transform_c*y+_transform_d;
}

double PlotPicture::inverseTransformX(const double x) const
{
   return (1/_transform_a)*x-_transform_b/_transform_a;
}

double PlotPicture::inverseTransformY(const double y) const
{
   return (1/_transform_c)*y-_transform_d/_transform_c;
}

QPoint PlotPicture::transformPoint(const QPoint & point) const
{
   return QPoint(transformX(point.x()), transformY(point.y()));
}

int PlotPicture::checkLogInvX(const double x, const QPoint & plf, 
      const QPoint & pbr) const 
{
   int x1 = logInvX(x);
   if(x1<plf.x())
      return plf.x();
   else if(x1>pbr.x())
      return pbr.x();
   return x1;
}

void PlotPicture::drawFrame(QPainter & painter)
{
   QPoint plf = transformPoint(_origin);//topleft point
   QPoint pbr = transformPoint(_end);//bottom right point
   
   QPen pen;
   pen.setColor(Qt::GlobalColor(_color[_rectColor]));
   pen.setStyle(Qt::SolidLine);
   pen.setWidth(_lineRectWidth);
   painter.setPen(pen);

   painter.drawRect(QRectF(plf, pbr));
   int dy = (pbr.y()-plf.y())/10;//note that y is negative oriented
   double ddy = (pbr.y()-plf.y())/10.0;//note that y is negative oriented
   //the biggest rectangle
   painter.drawRect(QRectF(plf-QPoint(2*dy,2*dy), pbr+QPoint(2*dy, 2*dy)));
//left text rectangle
   QPoint otherLeft = plf-QPoint(2*dy,0);
   QRectF leftRect = QRectF(QPoint(-otherLeft.y(), otherLeft.x()), QPoint(-pbr.y()-2*dy, plf.x()-dy));
   painter.rotate(-90);
   painter.drawRect(leftRect);
   painter.drawText(leftRect, Qt::AlignCenter, _names[0]);
   painter.rotate(90);
//right text rectangle
   QPoint otherRight = pbr+QPoint(2*dy,2*dy);
//   QRectF rightRect = QRectF(QPoint(pbr.x()+dy, plf.y()-2*dy-1), QPoint(pbr+QPoint(2*dy, 2*dy)));
   QRectF rightRect = QRectF(QPoint(plf.y()-2*dy, -pbr.x()-dy), QPoint(otherRight.y(), -otherRight.x()));
   painter.rotate(90);
   painter.drawRect(rightRect);
   painter.drawText(rightRect, Qt::AlignCenter, _names[1]);
   painter.rotate(-90);

   //caption rectangle
   if(!_caption.isEmpty())
   {
      QRectF captRect = QRectF(plf-QPoint(2*dy,4*dy-1), QPoint(pbr.x(), plf.y()-2*dy));
      painter.drawRect(captRect);
      painter.drawText(captRect, Qt::AlignCenter, _caption);
   }

   //lower rectangle
   QRectF lowerRect = QRectF(QPoint(plf.x()-dy, pbr.y()+dy), 
            QPoint(pbr.x()+dy, pbr.y()+2*dy));
   painter.drawRect(lowerRect);
   painter.drawText(lowerRect, Qt::AlignCenter, _names[2]);

   //j.u.s. rectangle
   QRectF jusRect = QRectF(plf-QPoint(2*dy,2*dy-1), plf);
   painter.drawText(jusRect, Qt::AlignCenter, _names[3]);

   //M.I.T. rectangle
   QRectF mitRect = QRectF(QPoint(plf.x()-2*dy,pbr.y()+2*dy), QPoint(plf.x(), pbr.y()+4*dy+1));
   painter.drawText(mitRect, Qt::AlignCenter, _names[4]);
   painter.drawRect(mitRect);

   for(size_t i=0;i<=10;i++)
   {//horizontal lines
      painter.drawLine(plf.x()-5, plf.y()+i*ddy, plf.x(), plf.y()+i*ddy);
      painter.drawLine(pbr.x(), plf.y()+i*ddy, pbr.x()+5, plf.y()+i*ddy);

      ////odavde///podesavanje debljine
      pen.setWidth(_lineThinWidth);
     // pen.setColor(Qt::GlobalColor(Qt::gray));
      painter.setPen(pen);
      painter.drawLine(plf.x(), plf.y()+i*ddy, pbr.x(), plf.y()+i*ddy);
      pen.setWidth(_lineRectWidth);
     // pen.setColor(Qt::GlobalColor(_color[_rectColor]));
      painter.setPen(pen);
      ////dovde//

      if(i==0)
      {
         painter.drawText(plf.x()-32, plf.y()+i*ddy+5, QString::number(10*(10-i)));
         painter.drawText(pbr.x()+10, plf.y()+i*ddy+5, QString::number(10*i));
      }
      else if(i==10)
      {
         painter.drawText(plf.x()-15, plf.y()+i*ddy+5, QString::number(10*(10-i)));
         painter.drawText(pbr.x()+10, plf.y()+i*ddy+5, QString::number(10*i));
      }
      else
      {
         painter.drawText(plf.x()-23, plf.y()+i*ddy+5, QString::number(10*(10-i)));
         painter.drawText(pbr.x()+10, plf.y()+(10-i)*ddy+5, QString::number(10*(10-i)));
      }

   }
   int downInterv = fabs(_rightBIndex - _leftBIndex);
   double dx = ((double)(pbr.x()-plf.x()))/downInterv;
   
   double add = _leftBIndex>_rightBIndex ? 1 : 0;
   double sgn = _leftBIndex>_rightBIndex ? -1 : 1;
   for(int i=0;i<=downInterv;i++)
   {//vertical lines
      double xtick = logInvX(_bVal[_leftBIndex + sgn*i].toDouble());
      painter.drawLine(xtick, pbr.y(), xtick, pbr.y()+5);
      //ticks
      if(i<downInterv+add && i>=add)
      {
         painter.drawLine(xtick+sgn*log10(2)*dx, pbr.y(), 
               xtick+sgn*log10(2)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(3)*dx, pbr.y(), 
               xtick+sgn*log10(3)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(4)*dx, pbr.y(), 
               xtick+sgn*log10(4)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(5)*dx, pbr.y(), 
               xtick+sgn*log10(5)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(6)*dx, pbr.y(), 
               xtick+sgn*log10(6)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(7)*dx, pbr.y(), 
               xtick+sgn*log10(7)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(8)*dx, pbr.y(), 
               xtick+sgn*log10(8)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(9)*dx, pbr.y(), 
               xtick+sgn*log10(9)*dx, pbr.y()+5);
      
         ////odavde///podesavanje debljine
         pen.setWidth(_lineThinWidth);//_lineRectWidth/2);
         //pen.setColor(Qt::GlobalColor(Qt::gray));
         painter.setPen(pen);
         painter.drawLine(xtick, plf.y(), xtick, pbr.y());
         if(_leftBIndex+sgn*i==5 || _leftBIndex+sgn*i==6 
               || _leftBIndex+sgn*i==8)
         {//thick line for 0.002 0.02 2
            pen.setWidth(_lineRectWidth);
            painter.setPen(pen);
         }
         painter.drawLine((int)(xtick+sgn*log10(2)*dx), plf.y(), 
               xtick+sgn*log10(2)*dx, pbr.y()+5);
         if(_leftBIndex+sgn*i==5 || _leftBIndex+sgn*i==6 
               || _leftBIndex+sgn*i==8)
         {//back on thin line for 0.002 0.02 2
            pen.setWidth(_lineThinWidth);
            painter.setPen(pen);
         }

         painter.drawLine(xtick+sgn*log10(3)*dx, plf.y(), 
               xtick+sgn*log10(3)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(4)*dx, plf.y(), 
               xtick+sgn*log10(4)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(5)*dx, plf.y(), 
               xtick+sgn*log10(5)*dx, pbr.y()+5);
         
         if(_leftBIndex+sgn*i==6 || _leftBIndex+sgn*i==9)
         {//thick line for 0.06 60
            pen.setWidth(_lineRectWidth);
            painter.setPen(pen);
         }
         painter.drawLine(xtick+sgn*log10(6)*dx, plf.y(), 
               xtick+sgn*log10(6)*dx, pbr.y()+5);
         if(_leftBIndex+sgn*i==6 || _leftBIndex+sgn*i==9)
         {//thick line for 0.06 60
            pen.setWidth(_lineThinWidth);
            painter.setPen(pen);
         }

         painter.drawLine(xtick+sgn*log10(7)*dx, plf.y(), 
               xtick+sgn*log10(7)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(8)*dx, plf.y(), 
               xtick+sgn*log10(8)*dx, pbr.y()+5);
         painter.drawLine(xtick+sgn*log10(9)*dx, plf.y(), 
               xtick+sgn*log10(9)*dx, pbr.y()+5);

         pen.setWidth(_lineRectWidth);
         //pen.setColor(Qt::GlobalColor(_color[_rectColor]));
         painter.setPen(pen);
         ////dovde//
      }

//      int j = _leftBIndex < _rightBIndex ? _leftBIndex+i : _leftBIndex - i;
      int j = _leftBIndex + sgn * i;
      painter.rotate(90);
      painter.drawText(pbr.y()+8, -xtick+6, _bVal[j]);
      if(i<downInterv+add && i>=add)
      {
         painter.drawText(pbr.y()+8, -xtick-sgn*log10(2)*dx+6, 
               "2"+_bVal[j].mid(1));
         painter.drawText(pbr.y()+8, -xtick-sgn*log10(6)*dx+6, 
               "6"+_bVal[j].mid(1));
      }
//      painter.drawText(plf.x()+i*dx-6, pbr.y()+17, _bVal[j]);
      painter.rotate(-90);
   }
   //Glina 
   int x1Glina = checkLogInvX(1e-8, plf, pbr);
   int x2Glina = checkLogInvX(2e-3, plf, pbr);
   QRect glinaRect(QPoint(x1Glina, plf.y()-2*dy), 
         QPoint(x2Glina-1, plf.y()-1));
   painter.drawRect(glinaRect);
   painter.drawText(glinaRect, Qt::AlignCenter, _names[5]);
   QRect glinaRectD(QPoint(x1Glina, pbr.y()+2*dy), 
         QPoint(x2Glina-1, pbr.y()+4*dy));
   painter.drawRect(glinaRectD);
   painter.drawText(glinaRectD, Qt::AlignCenter, _names[5]);
   //Prasina
   int x2Pras = checkLogInvX(6e-3, plf, pbr);
   int x3Pras = checkLogInvX(1e-2, plf, pbr);
   int x4Pras = checkLogInvX(2e-2, plf, pbr);
   int x5Pras = checkLogInvX(6e-2, plf, pbr);

   QRect finaRectG(QPoint(x2Glina, plf.y()-dy), QPoint(x3Pras-1, plf.y()-1));
   painter.drawRect(finaRectG);
   painter.drawText(finaRectG, Qt::AlignCenter, _names[6]);
   QRect sredRectG(QPoint(x3Pras, plf.y()-dy), QPoint(x4Pras-1, plf.y()-1));
   painter.drawRect(sredRectG);
   painter.drawText(sredRectG, Qt::AlignCenter, _names[7]);
   QRect prasRectG(QPoint(x2Glina, plf.y()-2*dy), 
         QPoint(x4Pras-1, plf.y()-dy-1));
   painter.drawRect(prasRectG);
   painter.drawText(prasRectG, Qt::AlignCenter, _names[9]);
   
   QRect finaRectD(QPoint(x2Glina, pbr.y()+3*dy), 
         QPoint(x2Pras-1, pbr.y()+4*dy));
   painter.drawRect(finaRectD);
   painter.drawText(finaRectD, Qt::AlignCenter, _names[6]);   
   QRect sredRectD(QPoint(x2Pras, pbr.y()+3*dy), 
         QPoint(x4Pras-1, pbr.y()+4*dy));
   painter.drawRect(sredRectD);
   painter.drawText(sredRectD, Qt::AlignCenter, _names[7]);   
   QRect krupRectD(QPoint(x4Pras, pbr.y()+3*dy), 
         QPoint(x5Pras-1, pbr.y()+4*dy));
   painter.drawRect(krupRectD);
   painter.drawText(krupRectD, Qt::AlignCenter, _names[8]);   
   QRect prasRectD(QPoint(x2Glina, pbr.y()+2*dy), 
         QPoint(x5Pras-1, pbr.y()+3*dy-1));
   painter.drawRect(prasRectD);
   painter.drawText(prasRectD, Qt::AlignCenter, _names[9]);

   //Pesak 
   int x1Pes = checkLogInvX(0.1, plf, pbr);
   int x2Pes = checkLogInvX(0.2, plf, pbr);
   int x3Pes = checkLogInvX(0.6, plf, pbr);
   int x4Pes = checkLogInvX(1.0, plf, pbr);
   int x5Pes = checkLogInvX(2.0, plf, pbr);
   QRect pesPraRectG(QPoint(x4Pras, plf.y()-dy), QPoint(x1Pes-1, plf.y()-1));
   painter.drawRect(pesPraRectG);
   painter.drawText(pesPraRectG, Qt::AlignCenter, _names[10]);
   QRect pesSitRectG(QPoint(x1Pes, plf.y()-dy), QPoint(x2Pes-1, plf.y()-1));
   painter.drawRect(pesSitRectG);
   painter.drawText(pesSitRectG, Qt::AlignCenter, _names[11]);
   QRect pesSreRectG(QPoint(x2Pes, plf.y()-dy), QPoint(x4Pes-1, plf.y()-1));
   painter.drawRect(pesSreRectG);
   painter.drawText(pesSreRectG, Qt::AlignCenter, _names[12]);
   QRect pesKruRectG(QPoint(x4Pes, plf.y()-dy), QPoint(x5Pes-1, plf.y()-1));
   painter.drawRect(pesKruRectG);
   painter.drawText(pesKruRectG, Qt::AlignCenter, _names[8]);
   QRect pesRectG(QPoint(x4Pras, plf.y()-2*dy), QPoint(x5Pes-1, plf.y()-dy-1));
   painter.drawRect(pesRectG);
   painter.drawText(pesRectG, Qt::AlignCenter, _names[13]);

   QRect pesSitRectD(QPoint(x5Pras, pbr.y()+3*dy), 
         QPoint(x2Pes-1, pbr.y()+4*dy));
   painter.drawRect(pesSitRectD);
   painter.drawText(pesSitRectD, Qt::AlignCenter, _names[11]);
   QRect pesSreRectD(QPoint(x2Pes, pbr.y()+3*dy), 
         QPoint(x3Pes-1, pbr.y()+4*dy));
   painter.drawRect(pesSreRectD);
   painter.drawText(pesSreRectD, Qt::AlignCenter, _names[12]);
   QRect pesKruRectD(QPoint(x3Pes, pbr.y()+3*dy), 
         QPoint(x5Pes-1, pbr.y()+4*dy));
   painter.drawRect(pesKruRectD);
   painter.drawText(pesKruRectD, Qt::AlignCenter, _names[8]);
   QRect pesRectD(QPoint(x5Pras, pbr.y()+2*dy), 
         QPoint(x5Pes-1, pbr.y()+3*dy-1));
   painter.drawRect(pesRectD);
   painter.drawText(pesRectD, Qt::AlignCenter, _names[13]);

   //Sljunak
   int x1Slj = checkLogInvX(6, plf, pbr);
   int x2Slj = checkLogInvX(10, plf, pbr);
   int x3Slj = checkLogInvX(20, plf, pbr);
   int x4Slj = checkLogInvX(30, plf, pbr);
   int x5Slj = checkLogInvX(60, plf, pbr);
   QRect sljSitRectG(QPoint(x5Pes, plf.y()-dy), QPoint(x2Slj-1, plf.y()-1));
   painter.drawRect(sljSitRectG);
   painter.drawText(sljSitRectG, Qt::AlignCenter, _names[11]);
   QRect sljSreRectG(QPoint(x2Slj, plf.y()-dy), QPoint(x4Slj-1, plf.y()-1));
   painter.drawRect(sljSreRectG);
   painter.drawText(sljSreRectG, Qt::AlignCenter, _names[12]);
   QRect sljKruRectG(QPoint(x4Slj, plf.y()-dy), QPoint(x5Slj-1, plf.y()-1));
   painter.drawRect(sljKruRectG);
   painter.drawText(sljKruRectG, Qt::AlignCenter, _names[8]);
   QRect sljRectG(QPoint(x5Pes, plf.y()-2*dy), QPoint(x5Slj-1, plf.y()-dy-1));
   painter.drawRect(sljRectG);
   painter.drawText(sljRectG, Qt::AlignCenter, _names[14]);
   
   QRect sljSitRectD(QPoint(x5Pes, pbr.y()+3*dy), 
         QPoint(x1Slj-1, pbr.y()+4*dy));
   painter.drawRect(sljSitRectD);
   painter.drawText(sljSitRectD, Qt::AlignCenter, _names[11]);
   QRect sljSreRectD(QPoint(x1Slj, pbr.y()+3*dy), 
         QPoint(x3Slj-1, pbr.y()+4*dy));
   painter.drawRect(sljSreRectD);
   painter.drawText(sljSreRectD, Qt::AlignCenter, _names[12]);
   QRect sljKruRectD(QPoint(x3Slj, pbr.y()+3*dy), 
         QPoint(x5Slj-1, pbr.y()+4*dy));
   painter.drawRect(sljKruRectD);
   painter.drawText(sljKruRectD, Qt::AlignCenter, _names[8]);
   QRect sljRectD(QPoint(x5Pes, pbr.y()+2*dy), 
         QPoint(x5Slj-1, pbr.y()+3*dy-1));
   painter.drawRect(sljRectD);
   painter.drawText(sljRectD, Qt::AlignCenter, _names[14]);

   //Obluci 
   int xObl = checkLogInvX(100, plf, pbr);

   QRect oblRectG(QPoint(plf.y()-2*dy, -x5Slj), QPoint(plf.y()-1, -xObl-1));   
   QRect oblRectD(QPoint(pbr.y()+2*dy, -x5Slj), QPoint(pbr.y()+4*dy, -xObl-1));
   painter.rotate(90);
   painter.drawRect(oblRectG);
   painter.drawRect(oblRectD);
   painter.drawText(oblRectG, Qt::AlignCenter, _names[15]);
   painter.drawText(oblRectD, Qt::AlignCenter, _names[15]);
   painter.rotate(-90);

   int count = 0;

   if(_showLegend)
   {
      for(int i=0; i<_plotHandler->nDrills(); i++)
      {
         for(int j=0; j<_plotHandler->nSample(i); j++)
         {
            if(_plotHandler->isSampleChecked(i, j))
            {
               QPen pen((Qt::PenStyle) (_plotHandler->line(i, j)+1));
               pen.setColor(Qt::GlobalColor(_color[_plotHandler->color(i, j)]));
               //      pen.setStyle(Qt::SolidLine);
               
               pen.setWidth(2*_lineLineWidth);
               painter.setPen(pen);
               painter.drawLine(plf.x()-2*dy+(count%3)*10*dy, 
                     pbr.y()+(5+count/3)*dy-4, plf.x()+(count%3)*10*dy, 
                     pbr.y()+(5+count/3)*dy-4);
               
               QPen pen1;
               pen1.setColor(Qt::GlobalColor(Qt::black));
               //      pen.setStyle(Qt::SolidLine);
               painter.setPen(pen1);
               
               painter.drawText(plf.x()+10+(count%3)*10*dy, 
                  pbr.y()+(5+count/3)*dy, 
                 _plotHandler->drillName(i)+" "+_plotHandler->sampleName(i, j));
               count++;
            }
         }
      }
   }
}

void PlotPicture::drawSplines(QPainter & painter)
{
   for(auto& it:_drillSampleSpline)
   {
      int idrill=std::get<0> (it);
      int jsample=std::get<1> (it);
      int npoints = _plotHandler->npoints(idrill, jsample);
      if(npoints<2)
         return;
      std::vector<QPointF> points(npoints);
      for(int i=0;i<npoints;i++)
      {
         points[i]=logInvPoint(QPointF(
                  _plotHandler->pointValue(idrill, jsample, i, 0), 
                  _plotHandler->pointValue(idrill, jsample, i, 1)));
      }
      QPen pen((Qt::PenStyle) (_plotHandler->line(idrill, jsample)+1));
      
      pen.setColor(Qt::GlobalColor(_color[_plotHandler->color(idrill, jsample)]));
//      pen.setStyle(Qt::SolidLine);
      pen.setWidth(_lineLineWidth);
      painter.setPen(pen);
      
      QPainterPathStroker stroker;
      stroker.setWidth(_snap);
      stroker.setJoinStyle(Qt::MiterJoin);
      //QPainterPath nline=stroker.createStroke(line);

      if(_selected == it)
      {
         painter.setBrush(Qt::GlobalColor(_color[_plotHandler->color(idrill, jsample)]));
         stroker.setWidth(3);
      }else
         painter.setBrush(QBrush());

      if(npoints==2)
      {
         QPainterPath line;
         line.moveTo(points[0]);
         line.lineTo(points[1]);
        /* QPainterPathStroker stroker;
         stroker.setWidth(_snap);
         stroker.setJoinStyle(Qt::MiterJoin);*/
         QPainterPath nline=stroker.createStroke(line);
         _splines[it]=nline;
         if(_selected == it)
         {
            painter.drawPath(line);
         }else
            painter.drawPath(line);
         return;
      }

      std::vector<QPointF> P1=calculateP1(points);

      QPainterPath spline;
      spline.moveTo(points[0]);
      for(int i=0;i<npoints-2;i++)
      {
         QPointF P2 = 2*points[i+1]-P1[i+1];
         
         QPointF P1m = changeToLimit(P1[i], points[i], points[i+1]);
         QPointF P2m = changeToLimit(P2, points[i], points[i+1]);
         spline.cubicTo(P1m, P2m, points[i+1]);
         
         //qDebug()<<i<<" drugo tacka "<<logTransfX(P1[i].x())<<"; "<<logTransfY(P1[i].y())<<"\n";
      }
      //qDebug()<<n-2<<" drugo tavcka "<<logTransfX(P1[n-2].x())<<"; "<<logTransfY(P1[n-2].y())<<"\n";
      QPointF P2 = 0.5*(points[npoints-1]+P1[npoints-2]);
      
      QPointF P1m = changeToLimit(P1[npoints-2], points[npoints-2], 
            points[npoints-1]);
      QPointF P2m = changeToLimit(P2, points[npoints-2], points[npoints-1]);
      spline.cubicTo(P1m, P2m, points[npoints-1]);
      
     /* QPen pen((Qt::PenStyle) (_plotHandler->line(idrill, jsample)+1));
      
      pen.setColor(Qt::GlobalColor(_color[_plotHandler->color(idrill, jsample)]));
//      pen.setStyle(Qt::SolidLine);
      pen.setWidth(_lineLineWidth);
      painter.setPen(pen);*/

      
      
     /* QPainterPathStroker stroker;
      stroker.setWidth(_snap);
      stroker.setJoinStyle(Qt::MiterJoin);*/
      QPainterPath nspline=stroker.createStroke(spline);

      if(_selected == it)
      {
         painter.drawPath(nspline);
      }else
         painter.drawPath(spline);
      _splines[it]=nspline;

   }
}

std::vector<QPointF> PlotPicture::calculateP1(const std::vector<QPointF> & points)
{//matrix is [2 1 ... ; 1 4 1 ...; ...; ... 2 7]
   
   size_t n=points.size()-1;//because there is n-1 P1 control points
   std::vector<double> cprim(n-1);
   std::vector<QPointF> dprim(n);
   cprim[0]=0.5;
   dprim[0]=(points[0]+2*points[1])/2;
   for(size_t i=1; i<n-1; i++)
   {
      double down = 4-cprim[i-1];
      cprim[i]=1/down;
      QPointF di = 4*points[i]+2*points[i+1];
      dprim[i] = (di - dprim[i-1])/down;
   }
   dprim[n-1]=(8*points[n-1]+points[n]-2*dprim[n-2])/(7-2*cprim[n-2]);
   std::vector<QPointF> x(n);
   x[n-1]=dprim[n-1];
   for(size_t i=2; i<n+1; i++)
   {
      x[n-i]=dprim[n-i]-cprim[n-i]*x[n-i+1];
   }
   return x;
}

QPointF PlotPicture::changeToLimit(const QPointF & p, const QPointF & bl, 
      const QPointF & tr)
{
   QPointF newp=p;
   if(_leftBIndex<=_rightBIndex)
   {
      if(newp.x()<bl.x())
         newp.setX(bl.x());
      else if(newp.x()>tr.x())
         newp.setX(tr.x());
      if(newp.y()>bl.y())
         newp.setY(bl.y());
      else if(newp.y()<tr.y())
         newp.setY(tr.y());
   }else
   {//in this case bl is top right and tr is bottom left
      if(newp.x()<tr.x())
         newp.setX(tr.x());
      else if(newp.x()>bl.x())
         newp.setX(bl.x());
      if(newp.y()<tr.y())
         newp.setY(tr.y());
      else if(newp.y()>bl.y())
         newp.setY(bl.y());
   }

   return newp;
}
