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
#include "vtkSMSimpleStringInformationHelper.h"

#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMStringVectorProperty.h"

vtkStandardNewMacro(vtkSMSimpleStringInformationHelper);
vtkCxxRevisionMacro(vtkSMSimpleStringInformationHelper, "$Revision$");

//---------------------------------------------------------------------------
vtkSMSimpleStringInformationHelper::vtkSMSimpleStringInformationHelper()
{
}

//---------------------------------------------------------------------------
vtkSMSimpleStringInformationHelper::~vtkSMSimpleStringInformationHelper()
{
}

//---------------------------------------------------------------------------
void vtkSMSimpleStringInformationHelper::UpdateProperty(
    int serverIds, vtkClientServerID objectId, vtkSMProperty* prop)
{
  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(prop);
  if (!svp)
    {
    vtkErrorMacro("A null property or a property of a different type was "
                  "passed when vtkSMStringVectorProperty was needed.");
    return;
    }
  vtkClientServerStream str;
  str << vtkClientServerStream::Invoke 
      << objectId << prop->GetCommand()
      << vtkClientServerStream::End;

  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  pm->SendStream(vtkProcessModule::GetRootId(serverIds), str, 0);

  const vtkClientServerStream& res =     
    pm->GetLastResult(vtkProcessModule::GetRootId(serverIds));


  int numMsgs = res.GetNumberOfMessages();
  if (numMsgs < 1)
    {
    return;
    }

  int numArgs = res.GetNumberOfArguments(0);
  if (numArgs < 1)
    {
    return;
    }

  int argType = res.GetArgumentType(0, 0);

  if (argType == vtkClientServerStream::string_value)
    {
    const char* sres;
    int retVal = res.GetArgument(0, 0, &sres);
    if (!retVal)
      {
      vtkErrorMacro("Error getting argument.");
      return;
      }
    svp->SetNumberOfElements(1);
    svp->SetElement(0, sres);
    }
}

//---------------------------------------------------------------------------
void vtkSMSimpleStringInformationHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
