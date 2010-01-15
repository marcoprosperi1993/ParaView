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
// .NAME vtkCustomUnstructuredGridBuilder - Class for creating unstructured grids.
// .SECTION Description
// Class for creating vtkUnstructuredGrids for a test driver.  The 
// UnstructuredGrid is built directly from a vtkUniformGrid to demonstrate
// how to input a grid into the coprocessor.

#ifndef __vtkCustomUnstructuredGridBuilder_h
#define __vtkCustomUnstructuredGridBuilder_h

#include "vtkCPUnstructuredGridBuilder.h"

class vtkDataObject;
class vtkGenericCell;
class vtkIdList;
class vtkPoints;
class vtkUnstructuredGrid;

class VTK_EXPORT vtkCustomUnstructuredGridBuilder : public vtkCPUnstructuredGridBuilder
{
public:
  static vtkCustomUnstructuredGridBuilder* New();
  vtkTypeRevisionMacro(vtkCustomUnstructuredGridBuilder, vtkCPUnstructuredGridBuilder);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Return a grid.  BuiltNewGrid is set to 0 if the grids
  // that were returned were already built before.
  // vtkCustomUnstructuredGridBuilder will also delete the grid.
  virtual vtkDataObject* GetGrid(unsigned long timeStep, double time,
                                 int & builtNewGrid);

  // Description:
  // Customized function to build UnstructuredGrid.
  void BuildGrid();

protected:
  vtkCustomUnstructuredGridBuilder();
  ~vtkCustomUnstructuredGridBuilder();

  // Description:
  // Method to compute the centroid of Cell and return the values in xyz.
  void ComputeCellCentroid(vtkGenericCell* cell, double xyz[3]);

private:
  vtkCustomUnstructuredGridBuilder(const vtkCustomUnstructuredGridBuilder&); // Not implemented
  void operator=(const vtkCustomUnstructuredGridBuilder&); // Not implemented
};
#endif
