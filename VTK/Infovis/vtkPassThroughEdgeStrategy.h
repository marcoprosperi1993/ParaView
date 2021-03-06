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
/*----------------------------------------------------------------------------
 Copyright (c) Sandia Corporation
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/
// .NAME vtkPassThroughEdgeStrategy - passes edge routing information through
//
// .SECTION Description
// Simply passes existing edge layout information from the input to the
// output without making changes.

#ifndef __vtkPassThroughEdgeStrategy_h
#define __vtkPassThroughEdgeStrategy_h

#include "vtkEdgeLayoutStrategy.h"

class VTK_INFOVIS_EXPORT vtkPassThroughEdgeStrategy : public vtkEdgeLayoutStrategy 
{
public:
  static vtkPassThroughEdgeStrategy* New();
  vtkTypeRevisionMacro(vtkPassThroughEdgeStrategy,vtkEdgeLayoutStrategy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This is the layout method where the graph that was
  // set in SetGraph() is laid out.
  virtual void Layout();

protected:
  vtkPassThroughEdgeStrategy();
  ~vtkPassThroughEdgeStrategy();

private:
  vtkPassThroughEdgeStrategy(const vtkPassThroughEdgeStrategy&);  // Not implemented.
  void operator=(const vtkPassThroughEdgeStrategy&);  // Not implemented.
};

#endif

