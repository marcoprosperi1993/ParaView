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
// .NAME vtkRenderWindowCollection - a list of RenderWindows
// .SECTION Description
// vtkRenderWindowCollection represents and provides methods to manipulate a 
// list of RenderWindows. The list is unsorted and duplicate entries are 
// not prevented.

// .SECTION see also
// vtkRenderWindow vtkCollection

#ifndef __vtkRenderWindowCollection_h
#define __vtkRenderWindowCollection_h

#include "vtkCollection.h"
#include "vtkRenderWindow.h"

class VTK_RENDERING_EXPORT vtkRenderWindowCollection : public vtkCollection
{
 public:
  static vtkRenderWindowCollection *New();
  vtkTypeRevisionMacro(vtkRenderWindowCollection,vtkCollection);

  // Description:
  // Add a RenderWindow to the list.
  void AddItem(vtkRenderWindow *a) {
    this->vtkCollection::AddItem((vtkObject *)a);};
  
  // Description:
  // Get the next RenderWindow in the list. Return NULL when at the end of the 
  // list.
  vtkRenderWindow *GetNextItem() {
    return static_cast<vtkRenderWindow *>(this->GetNextItemAsObject());};
  
protected:
  vtkRenderWindowCollection() {};
  ~vtkRenderWindowCollection() {};

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

private:
  vtkRenderWindowCollection(const vtkRenderWindowCollection&);  // Not implemented.
  void operator=(const vtkRenderWindowCollection&);  // Not implemented.
};


#endif
