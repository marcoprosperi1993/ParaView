/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Thanks:    Thanks to David G. Gobbi who developed this class.


Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include <ctype.h>
#include <string.h>
#include "vtkImageExport.h"
#include "vtkObjectFactory.h"



//------------------------------------------------------------------------------
vtkImageExport* vtkImageExport::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageExport");
  if(ret)
    {
    return (vtkImageExport*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageExport;
}






//----------------------------------------------------------------------------
vtkImageExport::vtkImageExport()
{
  this->ImageFlip = NULL;
  this->ImageLowerLeft = 1;
}



//----------------------------------------------------------------------------
vtkImageExport::~vtkImageExport()
{
  if (this->ImageFlip)
    {
    this->ImageFlip->UnRegister(this);
    this->ImageFlip = NULL;
    }
}


//----------------------------------------------------------------------------
void vtkImageExport::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkProcessObject::PrintSelf(os,indent);

  os << indent << "ImageLowerLeft: " 
     << (this->ImageLowerLeft ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
void vtkImageExport::SetInput(vtkImageData *input)
{
  this->vtkProcessObject::SetNthInput(0, input);
}

//----------------------------------------------------------------------------
vtkImageData *vtkImageExport::GetInput()
{
  if (this->NumberOfInputs < 1)
    {
    return NULL;
    }
  
  return (vtkImageData *)(this->Inputs[0]);
}

//----------------------------------------------------------------------------
int vtkImageExportGetDataTypeSize(int type)
{
  switch (type)
    {
    case VTK_VOID:
      return 0;
    case VTK_DOUBLE:
      return sizeof(double);
    case VTK_FLOAT:
      return sizeof(float);
    case VTK_LONG:
      return sizeof(long);
    case VTK_UNSIGNED_LONG:
      return sizeof(unsigned long);
    case VTK_INT:
      return sizeof(int);
    case VTK_UNSIGNED_INT:
      return sizeof(unsigned int);
    case VTK_SHORT:
      return sizeof(short);
    case VTK_UNSIGNED_SHORT:
      return sizeof(unsigned short); 
    case VTK_UNSIGNED_CHAR:
      return sizeof(unsigned char); 
    default:
      return 0; 
    }
}

//----------------------------------------------------------------------------
int vtkImageExport::GetDataMemorySize()
{
  this->GetInput()->UpdateInformation();
  int *extent = this->GetInput()->GetWholeExtent();

  int size = vtkImageExportGetDataTypeSize(this->GetInput()->GetScalarType());
  if (size == 0)
    {
    vtkErrorMacro(<< "GetDataMemorySize: Illegal ScalarType.");
    return 0; 
    }
  size *= this->GetInput()->GetNumberOfScalarComponents();
  size *= (extent[1] - extent[0] + 1);
  size *= (extent[3] - extent[2] + 1);
  size *= (extent[5] - extent[4] + 1);

  return size;
}


//----------------------------------------------------------------------------
void vtkImageExport::GetDataDimensions(int *dims)
{
  this->GetInput()->UpdateInformation();
  int *extent = this->GetInput()->GetWholeExtent();
  dims[0] = extent[1]-extent[0]+1;
  dims[1] = extent[3]-extent[2]+1;
  dims[2] = extent[5]-extent[4]+1;
}

//----------------------------------------------------------------------------
// Exports all the data from the input.
void vtkImageExport::Export(void *output)
{
  memcpy(output,this->GetPointerToData(),this->GetDataMemorySize());
}

//----------------------------------------------------------------------------
// Provides a valid pointer to the data (only valid until the next
// update, though)

void *vtkImageExport::GetPointerToData()
{
  // Error checking
  if ( this->GetInput() == NULL )
    {
    vtkErrorMacro(<<"Export: Please specify an input!");
    return 0;
    }

  vtkImageData *input = this->GetInput();

  // flip data if necessary
  if (this->ImageLowerLeft == 0)
    {
    if (this->ImageFlip == NULL)
      {
      this->ImageFlip = vtkImageFlip::New();
      this->ImageFlip->SetInput(input);
      this->ImageFlip->SetFilteredAxis(1);
      input = this->ImageFlip->GetOutput();
      }
    }
  else
    {
    if (this->ImageFlip)
      {
      this->ImageFlip->UnRegister(this);
      this->ImageFlip = NULL;
      }
    }

  //  if (this->GetDataMemorySize() > input->GetMemoryLimit())
  //    {
  //    input->SetMemoryLimit(this->GetDataMemorySize());
  //    }

  input->SetUpdateExtent(input->GetWholeExtent());
  input->ReleaseDataFlagOff();

  this->UpdateProgress(0.0);
  input->Update();
  this->UpdateProgress(1.0);

  return input->GetScalarPointer();
}
  
  





