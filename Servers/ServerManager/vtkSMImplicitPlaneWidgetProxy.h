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
// .NAME vtkSMImplicitPlaneWidgetProxy 
// .SECTION Description

#ifndef __vtkSMImplicitPlaneWidgetProxy_h
#define __vtkSMImplicitPlaneWidgetProxy_h

#include "vtkSM3DWidgetProxy.h"

class VTK_EXPORT vtkSMImplicitPlaneWidgetProxy : public vtkSM3DWidgetProxy
{
public:
  static vtkSMImplicitPlaneWidgetProxy* New();
  vtkTypeRevisionMacro(vtkSMImplicitPlaneWidgetProxy, vtkSM3DWidgetProxy);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  vtkSetVector3Macro(Center,double);  
  vtkGetVector3Macro(Center,double);
  
  vtkSetVector3Macro(Normal,double);
  vtkGetVector3Macro(Normal,double);
  
  // Description:
  // Send a SetDrawPlane event to the server.
  vtkSetMacro(DrawPlane,int);
  
  virtual void SaveInBatchScript(ofstream *file);

  // Description:
  // In vtkImplicitPlaneWidget, PlaceWidget uses the bounds on the input
  // to position the center of the widget, rather than the argument bounds.
  // Hence, we have to explicitly position the center.
  virtual void PlaceWidget(double bds[6]);

  virtual void UpdateVTKObjects();
protected:
  vtkSMImplicitPlaneWidgetProxy();
  ~vtkSMImplicitPlaneWidgetProxy();
 
  // Description:
  // Overloaded to update the property values before saving state
  virtual void SaveState(const char* name, ostream* file, vtkIndent indent);

  // Description:
  // Execute event of the 3D Widget.
  virtual void ExecuteEvent(vtkObject*, unsigned long, void*);

  virtual void CreateVTKObjects(int numObjects);

  double  Center[3];
  double  Normal[3];
  int     DrawPlane;

  // Description:
  // These are the center and normal that were last set on the VTK object on the Server.
  // This is used to determine if the Center/Normal changed has changed when 
  // UpdateVTKObjects() is called. The WidgetModifiedEvent is raised only if they are 
  // changed. This intricacy is needed since, we don't wnat the WidgetModifiedEvent 
  // invoked when only the DrawPlane status is modified. 
  // Otherwise we get problems like the Accept button
  // remaining green even after Accept is called etc.
  double LastCenter[3];
  double LastNormal[3];
  vtkSetVector3Macro(LastCenter,double);
  vtkSetVector3Macro(LastNormal,double); 

private:
  vtkSMImplicitPlaneWidgetProxy(const vtkSMImplicitPlaneWidgetProxy&); // Not implemented
  void operator=(const vtkSMImplicitPlaneWidgetProxy&); // Not implemented
};

#endif
