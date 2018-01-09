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
#include "message.h"
#include "qGrains.h"

#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QString>
#include <QWidget>

Message::Message(QGrains * qGrains) : _qGrains(qGrains) {}

void Message::newError(const QString text)
{
   QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("ERROR"),text,
                                            QMessageBox::Ok,_qGrains);
   newError->exec();
}

void Message::newWarning(const QString text)
{
   QMessageBox * newWarning = new QMessageBox(QMessageBox::Icon::Information,
                                            QString("Information"),text,
                                            QMessageBox::Ok,_qGrains);
   newWarning->exec();
}

int Message::newModalWarning(QWidget * parent, const QString text)
{
   _modalParent = parent;
   QMessageBox * newWarning = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("Warning"),text,
                                              QMessageBox::Ok | QMessageBox::Cancel, _modalParent);
   newWarning->setDefaultButton(QMessageBox::Cancel);
   if (newWarning->exec() == QMessageBox::Cancel) return false;
   else return true;
   _modalParent = nullptr;
}
