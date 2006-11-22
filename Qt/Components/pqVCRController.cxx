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
#include "pqVCRController.h"

// ParaView Server Manager includes.
#include "vtkSMProxy.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMIntRangeDomain.h"

// Qt includes.
#include <QtDebug>

// ParaView includes.
#include "pqApplicationCore.h"
#include "pqPipelineSource.h"
#include "pqRenderViewModule.h"
#include "pqUndoStack.h"

//-----------------------------------------------------------------------------
pqVCRController::pqVCRController(QObject* _parent/*=null*/) : QObject(_parent)
{
  this->Source = NULL;
  QObject::connect(&this->Timer, SIGNAL(timeout()),
                   this, SLOT(onTimerNextFrame()));
  this->Timer.setInterval(100);
}

//-----------------------------------------------------------------------------
pqVCRController::~pqVCRController()
{
}

void pqVCRController::setSource(pqPipelineSource* source)
{
  this->Source = source;
}

void pqVCRController::onPlay()
{
  if(!this->Timer.isActive())
    {
    this->Timer.start();
    }
}

void pqVCRController::onPause()
{
  if(this->Timer.isActive())
    {
    this->Timer.stop();
    }
}
  
void pqVCRController::onTimerNextFrame()
{
  bool didStep = this->updateSource(false, false, 1);
  if(!didStep)
    {
    this->Timer.stop();
    emit this->playCompleted();
    }
}

//-----------------------------------------------------------------------------
void pqVCRController::onFirstFrame()
{
  this->updateSource(true, false, 0);
}

//-----------------------------------------------------------------------------
void pqVCRController::onPreviousFrame()
{
  this->updateSource(false, false, -1);
}

//-----------------------------------------------------------------------------
void pqVCRController::onNextFrame()
{
  this->updateSource(false, false, 1);
}

//-----------------------------------------------------------------------------
void pqVCRController::onLastFrame()
{
  this->updateSource(false, true, 0);
}

//-----------------------------------------------------------------------------
bool pqVCRController::updateSource(bool first, bool last, int offset)
{
  if (!this->Source)
    {
    return false;
    }
  vtkSMProxy* activeProxy = this->Source->getProxy();
  vtkSMIntVectorProperty* const timestep = vtkSMIntVectorProperty::SafeDownCast(
    activeProxy->GetProperty("TimeStep"));
    
  if(!timestep)
    {
    return false;
    }

  vtkSMIntRangeDomain* const timestep_range =
    vtkSMIntRangeDomain::SafeDownCast(timestep->GetDomain("range"));
    
  if(!timestep_range)
    {
    qDebug() << "Failed to locate 'range' domain.";
    return false;
    }

  pqApplicationCore::instance()->getUndoStack()->BeginUndoSet("VCR");

  int min_exists = 0;
  int max_exists = 0;
  int min_step = timestep_range->GetMinimum(0, min_exists);
  int max_step = timestep_range->GetMaximum(0, max_exists);
  
  int previousValue = timestep->GetElement(0);

  if (first)
    {
    timestep->SetElement(0, min_step);
    }
  else if (last)
    {
    timestep->SetElement(0, max_step);
    }
  else
    {
    int new_time = timestep->GetElement(0) + offset;
    new_time = (new_time >= min_step)? new_time : min_step;
    new_time = (new_time <= max_step)? new_time : max_step;
    timestep->SetElement(0, new_time);
    }
  activeProxy->UpdateVTKObjects();
  pqApplicationCore::instance()->getUndoStack()->EndUndoSet();

  emit this->timestepChanged();
  
  this->Source->renderAllViews();

  return timestep->GetElement(0) != previousValue ? true : false;

}


