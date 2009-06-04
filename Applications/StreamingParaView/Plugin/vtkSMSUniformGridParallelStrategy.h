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
// .NAME vtkSMSUniformGridParallelStrategy
// .SECTION Description

#ifndef __vtkSMSUniformGridParallelStrategy_h
#define __vtkSMSUniformGridParallelStrategy_h

#include "vtkSMUniformGridParallelStrategy.h"

class VTK_EXPORT vtkSMSUniformGridParallelStrategy 
: public vtkSMUniformGridParallelStrategy
{
public:
  static vtkSMSUniformGridParallelStrategy* New();
  vtkTypeRevisionMacro(vtkSMSUniformGridParallelStrategy,
                       vtkSMUniformGridParallelStrategy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Tells server side to work with a particular piece until further notice.
  virtual void SetPassNumber(int Pass, int force);
  // Description:
  // Orders the pieces from most to least important.
  virtual int ComputePriorities();
  // Description:
  // Copies the piece ordering to dest via serialization.
  virtual void SharePieceList(vtkSMRepresentationStrategy *dest);
  // Description:
  // Clears the data object cache in the streaming display pipeline.
  virtual void ClearStreamCache();
  // Description:
  // Tells the strategy where the camera is so that pieces can be sorted and rejected
  virtual void SetViewState(double *camera, double *frustum);

//BTX
protected:
  vtkSMSUniformGridParallelStrategy();
  ~vtkSMSUniformGridParallelStrategy();

  // Description:
  // Overridden to swap in StreamingUpdateSupressors and add the PieceCache.
  virtual void BeginCreateVTKObjects();

  // Description:
  // Overridden to insert piececache in front of render pipeline.
  virtual void CreatePipeline(vtkSMSourceProxy* input, int outputport);

  // Description:
  // Overridden to insert piececache in front of LOD render pipeline.
  virtual void CreateLODPipeline(vtkSMSourceProxy* input, int outputport);


  // Description:
  // Copies ordered piece list from one UpdateSupressor to the other.
  virtual void CopyPieceList(vtkClientServerStream *stream,
                             vtkSMSourceProxy *src, 
                             vtkSMSourceProxy *dest);

  // Description:
  // Overridden to gather information incrementally.
  virtual void GatherInformation(vtkPVInformation*);

  // Description:
  // Overridden to gather information incrementally.
  virtual void GatherLODInformation(vtkPVInformation*);

  // Description:
  // Overridden to clean piececache too.
  virtual void InvalidatePipeline();

  vtkSMSourceProxy* PieceCache;
  vtkSMSourceProxy* ViewSorter;

private:
  vtkSMSUniformGridParallelStrategy(const vtkSMSUniformGridParallelStrategy&); // Not implemented
  void operator=(const vtkSMSUniformGridParallelStrategy&); // Not implemented

//ETX
};

#endif
