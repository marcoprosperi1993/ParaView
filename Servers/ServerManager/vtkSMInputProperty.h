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
// .NAME vtkSMInputProperty - proxy representing inputs to a filter
// .SECTION Description
// vtkSMInputProperty is a concrete sub-class of vtkSMProperty representing
// inputs to a filter (through vtkSMProxy). It is a special property that
// always calls AddInput on a vtkSMSourceProxy.
// .SECTION See Also
// vtkSMInputProperty vtkSMSourceProxy

#ifndef __vtkSMInputProperty_h
#define __vtkSMInputProperty_h

#include "vtkSMProxyProperty.h"

class VTK_EXPORT vtkSMInputProperty : public vtkSMProxyProperty
{
public:
  static vtkSMInputProperty* New();
  vtkTypeRevisionMacro(vtkSMInputProperty, vtkSMProxyProperty);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Command that can be used to remove inputs. If set, this
  // command is called before the main Command is called with
  // all the arguments.
  vtkSetStringMacro(CleanCommand);
  vtkGetStringMacro(CleanCommand);

  // Description:
  // Should be set to true if the "input port" this property represents
  // can accept multiple inputs (for example, an append filter)
  vtkSetMacro(MultipleInput, int);
  vtkGetMacro(MultipleInput, int);

  // Description:
  // If InputsUpdateImmediately is true, all input properties push
  // their values as soon as they are modified. Otherwise, the values
  // are pushed UpdateVTKObjects
  static int GetInputsUpdateImmediately();
  static void SetInputsUpdateImmediately(int up);

protected:
  vtkSMInputProperty();
  ~vtkSMInputProperty();

  //BTX
  // Description:
  // Description:
  // Append a command to update the vtk object with the property values(s).
  // The proxy objects create a stream by calling this method on all the
  // modified properties.
  // Note that if the proxy has multiple IDs, they are all appended to the 
  // command stream.  
  virtual void AppendCommandToStream(
    vtkSMProxy*, vtkClientServerStream* stream, vtkClientServerID objectId );
  //ETX

  // Description:
  // Set the appropriate ivars from the xml element. Should
  // be overwritten by subclass if adding ivars.
  virtual int ReadXMLAttributes(vtkSMProxy* parent, 
                                vtkPVXMLElement* element);

  int MultipleInput;
  
  static int InputsUpdateImmediately;

  char* CleanCommand;

private:
  vtkSMInputProperty(const vtkSMInputProperty&); // Not implemented
  void operator=(const vtkSMInputProperty&); // Not implemented
};

#endif
