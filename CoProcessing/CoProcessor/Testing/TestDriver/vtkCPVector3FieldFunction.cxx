/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkCPVector3FieldFunction.h"

vtkCxxRevisionMacro(vtkCPVector3FieldFunction, "$Revision$");

//----------------------------------------------------------------------------
vtkCPVector3FieldFunction::vtkCPVector3FieldFunction()
{
}

//----------------------------------------------------------------------------
vtkCPVector3FieldFunction::~vtkCPVector3FieldFunction()
{
}

//----------------------------------------------------------------------------
void vtkCPVector3FieldFunction::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
