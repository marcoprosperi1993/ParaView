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
// .NAME vtkPythonProgrammableFilter - Executes a user supplied python script 
// on its input dataset to produce an output dataset.
// .SECTION Description
// This filter will execute a python script to produce an output dataset.
// An new interpretor is created at the beginning of RequestInformation().
// The state of the python interpretor is preserved until the
// next execution of RequestInformation().
// After the interpretor is creates the vtk module is imported with 
// "from paraview import vtk".
//
// Then the interpretor runs the InformationScript during RequestInformation().
// This script is run in a python function called RequestInformation().
// An argument named self that refers to the programmable filter is passed
// to the function.
// The interpretor also runs the Script during RequestData().
// This script is run in a python function called RequestData().
// An argument named self that refers to the programmable filter is passed
// to the function.
// Furthermore, a set of parameters passed with the SetParameter()
// call are defined as Python variables inside both scripts. This allows
// the developer to keep the scripts the same but change their behaviour
// using parameters.
// .SECTION Caveat
// Note that this algorithm sets the output extent translator to be
// vtkOnePieceExtentTranslator. This means that all processes will ask
// for the whole extent. This behaviour can be overridden in InformationScript.

#ifndef __vtkPythonProgrammableFilter_h
#define __vtkPythonProgrammableFilter_h

#include "vtkProgrammableFilter.h"

class vtkPythonProgrammableFilterImplementation;

class VTK_EXPORT vtkPythonProgrammableFilter : public vtkProgrammableFilter
{
public:
  vtkTypeRevisionMacro(vtkPythonProgrammableFilter,vtkProgrammableFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkPythonProgrammableFilter *New();

  // Description: 
  // Set the text of the python script to execute.
  vtkSetStringMacro(Script)
  vtkGetStringMacro(Script)

  // Description: 
  // Set the text of the python script to execute in RequestInformation().
  vtkSetStringMacro(InformationScript)
  vtkGetStringMacro(InformationScript)

  // Description:
  // Set a name-value parameter that will be available to the script
  // when it is run
  void SetParameter(const char *name, const char *value);

  // Description:
  // Clear all name-value parameters
  void ClearParameters();

  // Description: 
  // For internal use only.
  static void ExecuteScript(void *);

  // Description:
  // Changes the output data set type.
  // Allowable values are defined in vtkType.h
  vtkSetMacro(OutputDataSetType, int);
  vtkGetMacro(OutputDataSetType, int);

protected:
  vtkPythonProgrammableFilter();
  ~vtkPythonProgrammableFilter();

  // Description:
  // For internal use only.
  void Exec(const char*, const char*);

  virtual int FillOutputPortInformation(int port, vtkInformation* info);

  //overridden to allow multiple inputs to port 0
  virtual int FillInputPortInformation(int port, vtkInformation *info);

  // Description:
  // Creates whatever output data set type is selected.
  virtual int RequestDataObject(vtkInformation* request, 
                                vtkInformationVector** inputVector, 
                                vtkInformationVector* outputVector);

  virtual int RequestInformation(vtkInformation* request, 
                                 vtkInformationVector** inputVector, 
                                 vtkInformationVector* outputVector);

  char *Script;
  char *InformationScript;
  int OutputDataSetType;

private:
  vtkPythonProgrammableFilter(const vtkPythonProgrammableFilter&);  // Not implemented.
  void operator=(const vtkPythonProgrammableFilter&);  // Not implemented.

//BTX
  vtkPythonProgrammableFilterImplementation* const Implementation;
//ETX
};

#endif
