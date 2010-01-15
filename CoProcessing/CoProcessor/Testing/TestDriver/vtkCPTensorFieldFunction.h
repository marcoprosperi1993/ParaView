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
// .NAME vtkCPTensorFieldFunction - Abstract class for specifying tensor fields at points.
// .SECTION Description
// Abstract class for specifying tensor fields at specified points.  

#ifndef __vtkCPTensorFieldFunction_h
#define __vtkCPTensorFieldFunction_h

#include "vtkObject.h"

class VTK_EXPORT vtkCPTensorFieldFunction : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkCPTensorFieldFunction, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the NumberOfComponents.  This is abstract to make sure 
  // that the value for the NumberOfComponents cannot be changed.
  virtual unsigned int GetNumberOfComponents() = 0;

  // Description:
  // Compute the field value at Point.
  virtual double ComputeComponenentAtPoint(unsigned int component, double point[3],
                                           unsigned long timeStep, double time) = 0;

protected:
  vtkCPTensorFieldFunction();
  ~vtkCPTensorFieldFunction();

private:
  vtkCPTensorFieldFunction(const vtkCPTensorFieldFunction&); // Not implemented
  void operator=(const vtkCPTensorFieldFunction&); // Not implemented
};

#endif
