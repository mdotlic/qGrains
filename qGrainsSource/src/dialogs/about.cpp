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

#include "about.h"

#include "qGrains.h"

#include <QDialog>
#include <QFrame>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>

About::About(QGrains * qGrains) : QDialog(qGrains)
{
   setWindowTitle("About");
   
   QLabel * iconJCI = new QLabel;
   iconJCI->setPixmap(QIcon(":/resources/images/logoJCI.png").pixmap(65,93));
   
   QLabel * labelJCI = new QLabel("JCI");
   labelJCI->setStyleSheet("font: 18px bold;");
   
   QString textCerni = QString("Jaroslav ").append(QChar(0x010C)).append("erni Institute\nfor the Development of Water Resources");
   QLabel * labelCerni = new QLabel(textCerni);
   labelCerni->setWordWrap(true);
   //labelCerni->setAlignment(Qt::AlignCenter);
   labelCerni->setFixedWidth(235);
   
   // TEXT LAYOUT
   
   QFrame * aboutFrame = new QFrame;
   aboutFrame->setFrameShape(QFrame::StyledPanel);
   aboutFrame->setFrameShadow(QFrame::Sunken);
   QVBoxLayout * frameLayout = new QVBoxLayout;
   
   constructText();
   
   QLabel * aboutQGrains = new QLabel("About qGrains_1.31");
   aboutQGrains->setStyleSheet("font:12px bold;");
   
   QLabel * textQGrainsLabel = new QLabel(_textQGrains);
   textQGrainsLabel->setWordWrap(true);
   textQGrainsLabel->setFixedWidth(300);

   QLabel * credits = new QLabel("Credits:");
   credits->setStyleSheet("font:12px bold;");
   
   QLabel * textCreditsLabel = new QLabel(_textCredits);
   textCreditsLabel->setWordWrap(true);
   textCreditsLabel->setFixedWidth(300);

   QLabel * copyrightLabel = new QLabel(_copyright);
   copyrightLabel->setWordWrap(true);
   copyrightLabel->setFixedWidth(300);
   
   //copyr->setStyleSheet("font:12px bold;");
   
   
   
   frameLayout->addWidget(aboutQGrains);
   frameLayout->addWidget(textQGrainsLabel);
   frameLayout->addWidget(credits);
   frameLayout->addWidget(textCreditsLabel);
   frameLayout->addStretch();
   frameLayout->addWidget(copyrightLabel);
   
   aboutFrame->setLayout(frameLayout);
   
   // MAIN LAYOUT
   
   QGridLayout * aboutLayout = new QGridLayout;
   aboutLayout->addWidget(iconJCI,0,0,3,1);
   //aboutLayout->addWidget(labelJCI,0,2);
   aboutLayout->addWidget(labelCerni,1,2);
   aboutLayout->setColumnStretch(1,1);
   
   aboutLayout->setRowStretch(2,2);
   aboutLayout->addWidget(aboutFrame,3,0,1,3);
   aboutLayout->setRowStretch(4,1);
   
   QPushButton * okButton = new QPushButton(QString("OK"));
   okButton->setMaximumWidth(80);
   aboutLayout->addWidget(okButton,5,0,1,3,Qt::AlignCenter);
   connect(okButton,SIGNAL(clicked()),this,SLOT(accept()));
   
   setLayout(aboutLayout);
   
   exec();
}

void About::constructText()
{
   _textQGrains = QString("qGrains is a sofware package for analyzing and visualization of subsurface granulometry. \nAll bugs and problems should be reported on the e-mail milandotlic@gmail.com."      
   );
   _textCredits=QString("Milan Dotlić, \nMilenko Pušić, \nGoran Jevtić \nMilan Dimkić");
   _copyright = QString("Copyright "+QString(QChar(0x00A9))+" 2018 Jaroslav Černi Institute for the Development of Water Resources, Serbia.");
}
