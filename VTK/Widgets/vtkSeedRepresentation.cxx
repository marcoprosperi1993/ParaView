/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSeedRepresentation.h"

#include "vtkActor2D.h"
#include "vtkCoordinate.h"
#include "vtkHandleRepresentation.h"
#include "vtkInteractorObserver.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include <vtkstd/iterator>
#include <vtkstd/list>

vtkCxxRevisionMacro(vtkSeedRepresentation, "$Revision$");
vtkStandardNewMacro(vtkSeedRepresentation);

vtkCxxSetObjectMacro(vtkSeedRepresentation,HandleRepresentation,vtkHandleRepresentation);

// The vtkHandleList is a PIMPLed list<T>.
class vtkHandleList : public vtkstd::list<vtkHandleRepresentation*> {};
typedef vtkstd::list<vtkHandleRepresentation*>::iterator vtkHandleListIterator;


//----------------------------------------------------------------------
vtkSeedRepresentation::vtkSeedRepresentation()
{
  this->HandleRepresentation  = NULL;

  // The representation for the seed handles
  this->Handles = new vtkHandleList;
  this->ActiveHandle = -1;
  
  this->Tolerance = 5;
}

//----------------------------------------------------------------------
vtkSeedRepresentation::~vtkSeedRepresentation()
{
  if ( this->HandleRepresentation )
    {
    this->HandleRepresentation->Delete();
    }

  // Loop over all handles releasing their observes and deleting them
  vtkHandleListIterator iter;
  for ( iter = this->Handles->begin(); iter != this->Handles->end(); ++iter )
    {
    (*iter)->Delete();
    }
  delete this->Handles;
}

//----------------------------------------------------------------------
vtkHandleRepresentation *vtkSeedRepresentation
::GetHandleRepresentation(unsigned int num)
{
  if ( num < this->Handles->size() )
    {
    vtkHandleListIterator iter = this->Handles->begin();
    vtkstd::advance(iter,num);
    return (*iter);
    }
  else //create one
    {
    vtkHandleRepresentation *rep = this->HandleRepresentation->NewInstance();
    rep->DeepCopy(this->HandleRepresentation);
    this->Handles->push_back( rep );
    return rep;
    }
}

//----------------------------------------------------------------------
void vtkSeedRepresentation::GetSeedWorldPosition(unsigned int seedNum, double pos[3])
{
  if ( seedNum >= this->Handles->size() )
    {
    vtkErrorMacro("Trying to access non-existent handle");
    return;
    }
  vtkHandleListIterator iter = this->Handles->begin();
  vtkstd::advance(iter,seedNum);
  (*iter)->GetWorldPosition(pos);
}

//----------------------------------------------------------------------
void vtkSeedRepresentation::SetSeedDisplayPosition(unsigned int seedNum, double pos[3])
{
  if ( seedNum >= this->Handles->size() )
    {
    vtkErrorMacro("Trying to access non-existent handle");
    return;
    }
  vtkHandleListIterator iter = this->Handles->begin();
  vtkstd::advance(iter,seedNum);
  (*iter)->SetDisplayPosition(pos);
}

//----------------------------------------------------------------------
void vtkSeedRepresentation::GetSeedDisplayPosition(unsigned int seedNum, double pos[3])
{
  if ( seedNum >= this->Handles->size() )
    {
    vtkErrorMacro("Trying to access non-existent handle");
    return;
    }
  vtkHandleListIterator iter = this->Handles->begin();
  vtkstd::advance(iter,seedNum);
  (*iter)->GetDisplayPosition(pos);
}

//----------------------------------------------------------------------
int vtkSeedRepresentation::GetNumberOfSeeds()
{
  return static_cast<int>(this->Handles->size());
}

//----------------------------------------------------------------------
int vtkSeedRepresentation::ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
  // Loop over all the seeds to see if the point is close to any of them.
  double xyz[3], pos[3];
  double tol2 = this->Tolerance*this->Tolerance;
  xyz[0] = static_cast<double>(X);
  xyz[1] = static_cast<double>(Y);
  xyz[2] = 0.0;

  int i;
  vtkHandleListIterator iter;
  for ( i = 0, iter = this->Handles->begin(); iter != this->Handles->end(); ++iter, ++i )
    {
    if ( *iter != NULL )
      {
      (*iter)->GetDisplayPosition(pos);
      if ( vtkMath::Distance2BetweenPoints(xyz,pos) <= tol2 )
        {
        this->InteractionState = vtkSeedRepresentation::NearSeed;
        this->ActiveHandle = i;
        return this->InteractionState;
        }
      }
    }

  // Nothing found, so it's outside
  this->InteractionState = vtkSeedRepresentation::Outside;
  return this->InteractionState;
}

//----------------------------------------------------------------------
int vtkSeedRepresentation::GetActiveHandle()
{
  return this->ActiveHandle;
}

//----------------------------------------------------------------------
int vtkSeedRepresentation::CreateHandle(double e[2])
{
  double pos[3];
  pos[0] = e[0];
  pos[1] = e[1];
  pos[2] = 0.0;

  vtkHandleRepresentation *rep = this->GetHandleRepresentation(
    static_cast<int>(this->Handles->size()));
  rep->SetDisplayPosition(pos);
  this->ActiveHandle = static_cast<int>(this->Handles->size()) - 1;
  return this->ActiveHandle;
}

//----------------------------------------------------------------------
void vtkSeedRepresentation::RemoveLastHandle()
{
  if ( this->Handles->size() < 1 )
    {
    return;
    }

  // Delete last handle
  this->Handles->back()->Delete();
  this->Handles->pop_back();
}

//----------------------------------------------------------------------
void vtkSeedRepresentation::RemoveActiveHandle()
{
  if ( this->Handles->size() < 1 )
    {
    return;
    }
  if ( this->ActiveHandle >= 0 && this->ActiveHandle < static_cast<int>(this->Handles->size()) )
    {
    vtkHandleListIterator iter = this->Handles->begin();
    vtkstd::advance( iter, this->ActiveHandle );
    this->Handles->erase( iter );
    ( *iter )->Delete();
    this->ActiveHandle = -1;
    }
}

//----------------------------------------------------------------------
void vtkSeedRepresentation::BuildRepresentation()
{
  if ( this->ActiveHandle >=0 && this->ActiveHandle < static_cast<int>(this->Handles->size()) )
    {
    vtkHandleRepresentation *rep = this->GetHandleRepresentation(this->ActiveHandle);
    if ( rep )
      {
      rep->BuildRepresentation();
      }
    }
}

//----------------------------------------------------------------------
void vtkSeedRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Tolerance: " << this->Tolerance <<"\n";
  os << indent << "Number of Seeds: " << this->GetNumberOfSeeds() <<"\n";
}
