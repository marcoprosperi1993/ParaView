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
#include "vtkInformationUnsignedLongKey.h"

vtkCxxRevisionMacro(vtkInformationUnsignedLongKey, "$Revision$");

//----------------------------------------------------------------------------
vtkInformationUnsignedLongKey::vtkInformationUnsignedLongKey(const char* name, const char* location):
  vtkInformationKey(name, location)
{
}

//----------------------------------------------------------------------------
vtkInformationUnsignedLongKey::~vtkInformationUnsignedLongKey()
{
}

//----------------------------------------------------------------------------
void vtkInformationUnsignedLongKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
class vtkInformationUnsignedLongValue: public vtkObjectBase
{
public:
  vtkTypeMacro(vtkInformationUnsignedLongValue, vtkObjectBase);
  unsigned long Value;
};

//----------------------------------------------------------------------------
void vtkInformationUnsignedLongKey::Set(vtkInformation* info,
                                        unsigned long value)
{
  vtkInformationUnsignedLongValue* v = new vtkInformationUnsignedLongValue;
  this->ConstructClass("vtkInformationUnsignedLongValue");
  v->Value = value;
  this->SetAsObjectBase(info, v);
  v->Delete();
}

//----------------------------------------------------------------------------
unsigned long vtkInformationUnsignedLongKey::Get(vtkInformation* info)
{
  vtkInformationUnsignedLongValue* v =
    vtkInformationUnsignedLongValue::SafeDownCast(
      this->GetAsObjectBase(info));
  return v?v->Value:0;
}

//----------------------------------------------------------------------------
int vtkInformationUnsignedLongKey::Has(vtkInformation* info)
{
  vtkInformationUnsignedLongValue* v =
    vtkInformationUnsignedLongValue::SafeDownCast(
      this->GetAsObjectBase(info));
  return v?1:0;
}

//----------------------------------------------------------------------------
void vtkInformationUnsignedLongKey::Copy(vtkInformation* from,
                                         vtkInformation* to)
{
  this->Set(to, this->Get(from));
}
