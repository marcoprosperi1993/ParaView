/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkProp.h"
#include "vtkObjectFactory.h"
#include "vtkAssemblyPaths.h"
#include "vtkOldStyleCallbackCommand.h"

vtkCxxRevisionMacro(vtkProp, "$Revision$");
vtkStandardNewMacro(vtkProp);

// Creates an Prop with the following defaults: visibility on.
vtkProp::vtkProp()
{
  this->Visibility = 1;  // ON

  this->Pickable   = 1;
  this->PickTag = 0;
  this->Dragable   = 1;
  
  this->AllocatedRenderTime = 10.0;
  this->EstimatedRenderTime = 0.0;
  this->RenderTimeMultiplier = 1.0;

  this->Paths = NULL;
}

vtkProp::~vtkProp()
{
  if ( this->Paths )
    {
    this->Paths->Delete();
    }
}

// This method is invoked when an instance of vtkProp (or subclass, 
// e.g., vtkActor) is picked by vtkPicker.
void vtkProp::SetPickMethod(void (*f)(void *), void *arg)
{
  if ( this->PickTag )
    {
    this->RemoveObserver(this->PickTag);
    }
  
  if ( f )
    {
    vtkOldStyleCallbackCommand *cbc = vtkOldStyleCallbackCommand::New();
    cbc->Callback = f;
    cbc->ClientData = arg;
    this->PickTag = this->AddObserver(vtkCommand::PickEvent,cbc);
    cbc->Delete();
    }
}

// Set a method to delete user arguments for PickMethod.
void vtkProp::SetPickMethodArgDelete(void (*f)(void *))
{
  vtkOldStyleCallbackCommand *cmd = 
    (vtkOldStyleCallbackCommand *)this->GetCommand(this->PickTag);
  if (cmd)
    {
    cmd->SetClientDataDeleteCallback(f);
    }
}

// This method is invoked if the prop is picked.
void vtkProp::Pick()
{
  this->InvokeEvent(vtkCommand::PickEvent,NULL);
}

// Shallow copy of vtkProp.
void vtkProp::ShallowCopy(vtkProp *prop)
{
  this->Visibility = prop->GetVisibility();
  this->Pickable   = prop->GetPickable();
  this->Dragable   = prop->GetDragable();
}

void vtkProp::InitPathTraversal()
{
  if ( this->Paths == NULL )
    {
    this->Paths = vtkAssemblyPaths::New();
    vtkAssemblyPath *path = vtkAssemblyPath::New();
    path->AddNode(this,NULL);
    this->BuildPaths(this->Paths,path);
    path->Delete();
    }
  this->Paths->InitTraversal();
}

vtkAssemblyPath *vtkProp::GetNextPath()
{
  if ( ! this->Paths)
    {
    return NULL;
    }
  return this->Paths->GetNextItem();
}

// This method is used in conjunction with the assembly object to build a copy
// of the assembly hierarchy. This hierarchy can then be traversed for 
// rendering, picking or other operations.
void vtkProp::BuildPaths(vtkAssemblyPaths *paths, vtkAssemblyPath *path)
{
  // This is a leaf node in the assembly hierarchy so we
  // copy the path in preparation to assingning it to paths.
  vtkAssemblyPath *childPath = vtkAssemblyPath::New();
  childPath->ShallowCopy(path);

  // We can add this path to the list of paths
  paths->AddItem(childPath);
  childPath->Delete(); //okay, reference counting
}

void vtkProp::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Dragable: " << (this->Dragable ? "On\n" : "Off\n");
  os << indent << "Pickable: " << (this->Pickable ? "On\n" : "Off\n");

  os << indent << "AllocatedRenderTime: " 
     << this->AllocatedRenderTime << endl;
  os << indent << "EstimatedRenderTime: " 
     << this->EstimatedRenderTime << endl;
  os << indent << "RenderTimeMultiplier: " 
     << this->RenderTimeMultiplier << endl;
  os << indent << "Visibility: " << (this->Visibility ? "On\n" : "Off\n");
}




