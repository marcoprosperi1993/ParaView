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
// .NAME vtkSMCompositeTreeDomain - domain used to restrict an
// vtkSMIntVectorProperty values to valid \c flat-index for a
// vtkCompositeDataSet.
// .SECTION Description
// vtkSMCompositeTreeDomain can be added to a vtkSMIntVectorProperty. This
// domain requires a vtkSMInputProperty which is used to provide the input to
// the filter. This domain obtains data information from the input selected on 
// the required input property and then decides the range for the flat-index. A
// flat index for a tree is obtained by performing a pre-order traversal of the
// tree eg. A ( B ( D, E), C (F, G)) becomes: [A,B,D,E,C,F,G], so flat-index of A is
// 0, while flat-index of C is 4.

#ifndef __vtkSMCompositeTreeDomain_h
#define __vtkSMCompositeTreeDomain_h

#include "vtkSMDomain.h"

class vtkSMInputProperty;
class vtkPVDataInformation;

// TODO: CHANGE NAME OF THIS CLASS
class VTK_EXPORT vtkSMCompositeTreeDomain : public vtkSMDomain
{
public:
  static vtkSMCompositeTreeDomain* New();
  vtkTypeRevisionMacro(vtkSMCompositeTreeDomain, vtkSMDomain);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Called when the 'required-property' is modified. The property must be a
  // vtkSMInputProperty. This will obtain the composite data information for the
  // input source and then determine the valid range for the flat-index.
  virtual void Update(vtkSMProperty* input);

  // Description:
  // Get the vtkPVDataInformation which provides the tree structure for the
  // composite dataset.
  vtkGetObjectMacro(Information, vtkPVDataInformation);

  // Description:
  // Is the (unchecked) value of the property in the domain? Overwritten by
  // sub-classes.
  virtual int IsInDomain(vtkSMProperty* vtkNotUsed(property)) {return 1; }

  // Description:
  // Mode indicates if the property is interested in all nodes, leaves only or
  // non-leaves only. Can be configured in XML using the "mode" attribute.
  // Values can be "all", "leaves", "non-leaves". Default is all nodes.
  vtkGetMacro(Mode, int);
  vtkSetMacro(Mode, int);

  //BTX
  enum
    {
    ALL=0,
    LEAVES=1,
    NON_LEAVES=2
    };
  //ETX

//BTX
protected:
  vtkSMCompositeTreeDomain();
  ~vtkSMCompositeTreeDomain();

  virtual int ReadXMLAttributes(vtkSMProperty* prop, vtkPVXMLElement* element);

  // Description:
  // Internal implementation called by Update(vtkSMProperty*);
  void Update(vtkSMInputProperty* iproperty);

  void InvokeModifiedIfChanged();

  void SetInformation(vtkPVDataInformation*);
  vtkPVDataInformation* Information;

  vtkTimeStamp UpdateTime;
  vtkPVDataInformation* LastInformation; // not reference counted.

  int Mode;
private:
  vtkSMCompositeTreeDomain(const vtkSMCompositeTreeDomain&); // Not implemented
  void operator=(const vtkSMCompositeTreeDomain&); // Not implemented
//ETX
};

#endif

