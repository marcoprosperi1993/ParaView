/*=========================================================================

Copyright (c) 1998-2003 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

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
// .NAME vtkHashMapIterator - an array map iterator

#ifndef __vtkHashMapIterator_h
#define __vtkHashMapIterator_h

#include "vtkAbstractIterator.h"

template <class DataType> class vtkVectorIterator;
template <class KeyType, class DataType> class vtkHashMap;
template <class KeyType, class DataType> class vtkAbstractMapItem;

template <class KeyType,class DataType>
class vtkHashMapIterator : public vtkAbstractIterator<KeyType,DataType>
{
  friend class vtkHashMap<KeyType,DataType>;

public:
  virtual const char* GetClassName() const { return "vtkHashMapIterator"; }

  // Description:
  // Retrieve the key of the current element.
  // This method returns VTK_OK if key was retrieved correctly.
  int GetKey(KeyType&);
  
  // Description:
  // Retrieve the data of the current element.
  // This method returns VTK_OK if key was retrieved correctly.
  int GetData(DataType&);
  
  // Description:
  // Retrieve the key and data of the current element.
  // This method returns VTK_OK if key and data were retrieved correctly.
  int GetKeyAndData(KeyType&, DataType&);
  
  // Description:
  // Set the data at the iterator's position.
  // This method returns VTK_OK if data were set correctly.
  int SetData(const DataType&);
  
  // Description:
  // Initialize the traversal of the container. 
  // Set the iterator to the "beginning" of the container.
  void InitTraversal();
  
  // Description:
  // Check if the iterator is at the end of the container. Returns 1
  // for yes and 0 for no.
  int IsDoneWithTraversal();
  
  // Description:
  // Increment the iterator to the next location.
  void GoToNextItem();
  
  // Description:
  // Decrement the iterator to the next location.
  void GoToPreviousItem();
  
  // Description:
  // Go to the "first" item of the map.
  void GoToFirstItem();
  
  // Description:
  // Go to the "last" item of the map.
  void GoToLastItem();
  
protected:
  static vtkHashMapIterator<KeyType,DataType> *New(); 
  
  vtkHashMapIterator();
  virtual ~vtkHashMapIterator();
  
  void ScanForward();
  void ScanBackward();
  
  typedef vtkAbstractMapItem<KeyType,DataType> ItemType;
  typedef vtkVectorIterator<ItemType> BucketIterator;
  
  vtkIdType Bucket;
  BucketIterator* Iterator;
  
private:
  vtkHashMapIterator(const vtkHashMapIterator<KeyType,DataType>&); // Not implemented
  void operator=(const vtkHashMapIterator<KeyType,DataType>&); // Not implemented
};

#ifdef VTK_NO_EXPLICIT_TEMPLATE_INSTANTIATION
#include "vtkHashMapIterator.txx"
#endif 

#endif



