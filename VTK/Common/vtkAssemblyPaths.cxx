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
#include "vtkAssemblyPaths.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkAssemblyPaths, "$Revision$");
vtkStandardNewMacro(vtkAssemblyPaths);

unsigned long vtkAssemblyPaths::GetMTime()
{
  unsigned long mtime=this->vtkCollection::GetMTime();
  unsigned long pathMTime;
  vtkAssemblyPath *path;
  
  for ( this->InitTraversal(); (path = this->GetNextItem()); )
    {
    pathMTime = path->GetMTime();
    if ( pathMTime > mtime )
      {
      mtime = pathMTime;
      }
    }
  return mtime;
}





