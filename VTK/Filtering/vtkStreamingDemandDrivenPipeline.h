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
// .NAME vtkStreamingDemandDrivenPipeline - Executive supporting partial updates.
// .SECTION Description
// vtkStreamingDemandDrivenPipeline is an executive that supports
// updating only a portion of the data set in the pipeline.  This is
// the style of pipeline update that is provided by the old-style VTK
// 4.x pipeline.  Instead of always updating an entire data set, this
// executive supports asking for pieces or sub-extents.

#ifndef __vtkStreamingDemandDrivenPipeline_h
#define __vtkStreamingDemandDrivenPipeline_h

#include "vtkDemandDrivenPipeline.h"

class vtkExtentTranslator;
class vtkInformationDoubleVectorKey;
class vtkInformationIntegerKey;
class vtkInformationIntegerVectorKey;
class vtkInformationObjectBaseKey;

class VTK_FILTERING_EXPORT vtkStreamingDemandDrivenPipeline : public vtkDemandDrivenPipeline
{
public:
  static vtkStreamingDemandDrivenPipeline* New();
  vtkTypeRevisionMacro(vtkStreamingDemandDrivenPipeline,vtkDemandDrivenPipeline);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Generalized interface for asking the executive to fullfill update
  // requests.
  virtual int ProcessRequest(vtkInformation* request);

  // Description:
  // Bring the outputs up-to-date.
  virtual int Update();
  virtual int Update(int port);
  virtual int UpdateWholeExtent();

  // Description:
  // Propagate the update request from the given output port back
  // through the pipeline.  Should be called only when information is
  // up to date.
  int PropagateUpdateExtent(int outputPort);

  // Description:
  // Set/Get the maximum number of pieces that can be requested from
  // the given port.  The maximum number of pieces is meta data for
  // unstructured data sets.  It gets set by the source during the
  // update information call.  A value of -1 indicates that there is
  // no maximum.
  int SetMaximumNumberOfPieces(int port, int n);
  int GetMaximumNumberOfPieces(int port);

  // Description:
  // Set/Get the whole extent of an output port.  The whole extent is
  // meta data for structured data sets.  It gets set by the algorithm
  // during the update information pass.
  int SetWholeExtent(int port, int extent[6]);
  void GetWholeExtent(int port, int extent[6]);
  int* GetWholeExtent(int port);

  // Description:
  // If the whole input extent is required to generate the requested output
  // extent, this method can be called to set the input update extent to the
  // whole input extent. This method assumes that the whole extent is known
  // (that UpdateInformation has been called)
  int SetUpdateExtentToWholeExtent(int port);

  // Description:
  // Get/Set the update extent for output ports that use 3D extents.
  int SetUpdateExtent(int port, int extent[6]);
  void GetUpdateExtent(int port, int extent[6]);
  int* GetUpdateExtent(int port);

  // Description:
  // Set/Get the update piece, update number of pieces, and update
  // number of ghost levels for an output port.  Similar to update
  // extent in 3D.
  int SetUpdateExtent(int port, int piece, int numPieces, int ghostLevel);
  int SetUpdatePiece(int port, int piece);
  int GetUpdatePiece(int port);
  int SetUpdateNumberOfPieces(int port, int n);
  int GetUpdateNumberOfPieces(int port);
  int SetUpdateGhostLevel(int port, int n);
  int GetUpdateGhostLevel(int port);

  // Description:
  // This request flag indicates whether the requester can handle more
  // data than requested for the given port.  Right now it is used in
  // vtkImageData.  Image filters can return more data than requested.
  // The the consumer cannot handle this (i.e. DataSetToDataSetFitler)
  // the image will crop itself.  This functionality used to be in
  // ImageToStructuredPoints.
  int SetRequestExactExtent(int port, int flag);
  int GetRequestExactExtent(int port);

  // Description:
  // Get/Set the object that will translate pieces into structured
  // extents for an output port.
  int SetExtentTranslator(int port, vtkExtentTranslator* translator);
  vtkExtentTranslator* GetExtentTranslator(int port);

  // Description:
  // Set/Get the whole bounding box of an output port data object.
  // The whole whole bounding box is meta data for data sets.  It gets
  // set by the algorithm during the update information pass.
  int SetWholeBoundingBox(int port, double bb[6]);
  void GetWholeBoundingBox(int port, double bb[6]);
  double* GetWholeBoundingBox(int port);

  // Description:
  // Key defining a request to propagate the update extent upstream.
  static vtkInformationIntegerKey* REQUEST_UPDATE_EXTENT();

  // Description:
  // Key for an algorithm to store in its own information to tell
  // this executive to keep executing it.
  static vtkInformationIntegerKey* CONTINUE_EXECUTING();

  // Description:
  // Key to store an extent translator in pipeline information.
  static vtkInformationObjectBaseKey* EXTENT_TRANSLATOR();

  // Description:
  // Keys to store an update request in pipeline information.
  static vtkInformationIntegerKey* UPDATE_EXTENT_INITIALIZED();
  static vtkInformationIntegerVectorKey* UPDATE_EXTENT();
  static vtkInformationIntegerKey* UPDATE_PIECE_NUMBER();
  static vtkInformationIntegerKey* UPDATE_NUMBER_OF_PIECES();
  static vtkInformationIntegerKey* UPDATE_NUMBER_OF_GHOST_LEVELS();

  // Description:
  // Key to store the whole extent provided in pipeline information.
  static vtkInformationIntegerVectorKey* WHOLE_EXTENT();

  // Description:
  // Key to store the maximum number of pieces provided in pipeline
  // information.
  static vtkInformationIntegerKey* MAXIMUM_NUMBER_OF_PIECES();

  // Description:
  // Key to store the bounding box of the entire data set in pipeline
  // information.
  static vtkInformationDoubleVectorKey* WHOLE_BOUNDING_BOX();

  // Description:
  // Key to specify the request for exact extent in pipeline information.
  static vtkInformationIntegerKey* EXACT_EXTENT();

protected:
  vtkStreamingDemandDrivenPipeline();
  ~vtkStreamingDemandDrivenPipeline();

  // Setup default information on the output after the algorithm
  // executes information.
  virtual int ExecuteInformation();

  // Copy information for the given request.
  virtual void CopyDefaultInformation(vtkInformation* request, int direction);

  // Helper to check output information before propagating it to inputs.
  virtual int VerifyOutputInformation(int outputPort);

  // Override this check to account for update extent.
  virtual int NeedToExecuteData(int outputPort);

  // Remove update/whole extent when resetting pipeline information.
  virtual void ResetPipelineInformation(int port, vtkInformation*);

private:
  vtkStreamingDemandDrivenPipeline(const vtkStreamingDemandDrivenPipeline&);  // Not implemented.
  void operator=(const vtkStreamingDemandDrivenPipeline&);  // Not implemented.
};

#endif
