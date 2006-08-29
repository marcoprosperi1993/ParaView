/*=========================================================================

   Program: ParaView
   Module:    $RCSfile$

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.1. 

   See License_v1.1.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "pqQVTKWidgetEventPlayer.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QtDebug>
#include <QRegExp>

#include <QtDebug>

#include "QVTKWidget.h"
pqQVTKWidgetEventPlayer::pqQVTKWidgetEventPlayer()
{
}

bool pqQVTKWidgetEventPlayer::playEvent(QObject* Object, 
         const QString& Command, const QString& Arguments, 
         bool& /*Error*/)
{
  QVTKWidget* widget = qobject_cast<QVTKWidget*>(Object);
  if(widget)
    {
    if (Command == "mousePress" || Command=="mouseRelease" || Command=="mouseMove")
      {
      QRegExp mouseRegExp("\\(([^,]*),([^,]*),([^,]),([^,]),([^,]*)\\)");
      if (mouseRegExp.indexIn(Arguments)!= -1)
        {
        QVariant v = mouseRegExp.cap(1);
        int x = static_cast<int>(v.toDouble() * widget->size().width());
        v = mouseRegExp.cap(2);
        int y = static_cast<int>(v.toDouble() * widget->size().height());
        v = mouseRegExp.cap(3);
        Qt::MouseButton button = static_cast<Qt::MouseButton>(v.toInt());
        v = mouseRegExp.cap(4);
        Qt::MouseButtons buttons = static_cast<Qt::MouseButton>(v.toInt());
        v = mouseRegExp.cap(5);
        Qt::KeyboardModifiers keym = static_cast<Qt::KeyboardModifier>(v.toInt());
        QEvent::Type type = (Command == "mousePress")? QEvent::MouseButtonPress :
          ((Command=="mouseMove")?  QEvent::MouseMove : QEvent::MouseButtonRelease);
        QMouseEvent e(type, QPoint(x,y), button, buttons, keym);
        QCoreApplication::sendEvent(widget, &e);
        QCoreApplication::processEvents();
        }
      return true;
      }
    }
  return false;
}

