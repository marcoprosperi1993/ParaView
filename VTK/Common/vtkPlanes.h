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
// .NAME vtkPlanes - implicit function for convex set of planes
// .SECTION Description
// vtkPlanes computes the implicit function and function gradient for a set
// of planes. The planes must define a convex space.
//
// The function value is the closest first order distance of a point to the
// convex region defined by the planes. The function gradient is the plane
// normal at the function value.  Note that the normals must point outside of
// the convex region. Thus, a negative function value means that a point is
// inside the convex region.
//
// There are several methods to define the set of planes. The most general is
// to supply an instance of vtkPoints and an instance of vtkDataArray. (The 
// points define a point on the plane, and the normals corresponding plane 
// normals.) Two other specialized ways are to 1) supply six planes defining 
// the view frustrum of a camera, and 2) provide a bounding box.

// .SECTION See Also
// vtkCamera

#ifndef __vtkPlanes_h
#define __vtkPlanes_h

#include "vtkImplicitFunction.h"

class vtkPlane;

class VTK_COMMON_EXPORT vtkPlanes : public vtkImplicitFunction
{
public:
  static vtkPlanes *New();
  vtkTypeRevisionMacro(vtkPlanes,vtkImplicitFunction);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // Evaluate plane equations. Return smallest absolute value.
  float EvaluateFunction(float x[3]);
  float EvaluateFunction(float x, float y, float z)
    {return this->vtkImplicitFunction::EvaluateFunction(x, y, z); } ;

  // Description
  // Evaluate planes gradient.
  void EvaluateGradient(float x[3], float n[3]);

  // Description:
  // Specify a list of points defining points through which the planes pass.
  vtkSetObjectMacro(Points,vtkPoints);
  vtkGetObjectMacro(Points,vtkPoints);
  
  // Description:
  // Specify a list of normal vectors for the planes. There is a one-to-one
  // correspondence between plane points and plane normals.
  void SetNormals(vtkDataArray* normals);
  vtkGetObjectMacro(Normals,vtkDataArray);

  // Description:
  // An alternative method to specify six planes defined by the camera view 
  // frustrum. See vtkCamera::GetFrustumPlanes() documentation.
  void SetFrustumPlanes(float planes[24]);

  // Description:
  // An alternative method to specify six planes defined by a bounding box.
  // The bounding box is a six-vector defined as (xmin,xmax,ymin,ymax,zmin,zmax).
  // It defines six planes orthogonal to the x-y-z coordinate axes.
  void SetBounds(float bounds[6]);
  void SetBounds(float xmin, float xmax, float ymin, float ymax,
                 float zmin, float zmax);

  // Description:
  // Return the number of planes in the set of planes.
  int GetNumberOfPlanes();
  
  // Description:
  // Create and return a pointer to a vtkPlane object at the ith
  // position. It is your responsibility to delete the vtkPlane
  // when done with it. Asking for a plane outside the allowable
  // range returns NULL.
  vtkPlane *GetPlane(int i);

protected:
  vtkPlanes();
  ~vtkPlanes();

  vtkPoints *Points;
  vtkDataArray *Normals;
  vtkPlane *Plane;

private:
  float Planes[24];
  float Bounds[6];

private:
  vtkPlanes(const vtkPlanes&);  // Not implemented.
  void operator=(const vtkPlanes&);  // Not implemented.
};

#endif


