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
#include "vtkCPNodalFieldBuilder.h"

#include "vtkCPTensorFieldFunction.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"

#include <vtkstd/vector>

vtkStandardNewMacro(vtkCPNodalFieldBuilder);
vtkCxxRevisionMacro(vtkCPNodalFieldBuilder, "$Revision$");

//----------------------------------------------------------------------------
vtkCPNodalFieldBuilder::vtkCPNodalFieldBuilder()
{
}

//----------------------------------------------------------------------------
void vtkCPNodalFieldBuilder::BuildField(unsigned long timeStep, double time,
                                        vtkDataSet* grid)
{
  vtkCPTensorFieldFunction* tensorFieldFunction = 
    this->GetTensorFieldFunction();
  if(tensorFieldFunction == 0)
    {
    vtkErrorMacro("Must set TensorFieldFunction.");
    return;
    }
  if(this->GetArrayName() == 0)
    {
    vtkErrorMacro("Must set ArrayName.");
    return;
    }
  vtkIdType numberOfPoints = grid->GetNumberOfPoints();
  unsigned int numberOfComponents = 
    tensorFieldFunction->GetNumberOfComponents();
  vtkDoubleArray* array = vtkDoubleArray::New();
  array->SetNumberOfComponents(numberOfComponents);
  array->SetNumberOfTuples(numberOfPoints);
  vtkstd::vector<double> tupleValues(numberOfComponents);
  double point[3];
  for(vtkIdType i=0;i<numberOfPoints;i++)
    {
    grid->GetPoint(i, point);
    for(unsigned int uj=0;uj<numberOfComponents;uj++)
      {
      tupleValues[uj] = tensorFieldFunction->ComputeComponenentAtPoint(
        uj, point, timeStep, time);
      }
    array->SetTupleValue(i, &tupleValues[0]);
    }
  array->SetName(this->GetArrayName());
  grid->GetPointData()->AddArray(array);
  array->Delete();
}

//----------------------------------------------------------------------------
vtkCPNodalFieldBuilder::~vtkCPNodalFieldBuilder()
{
}

//----------------------------------------------------------------------------
void vtkCPNodalFieldBuilder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
