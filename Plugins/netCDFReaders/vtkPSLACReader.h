// -*- c++ -*-
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

/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/

// .NAME vtkPSLACReader
//
// .SECTION Description
//
// Extends the vtkSLACReader to read in partitioned pieces.  Due to the nature
// of the data layout, this reader only works in a data parallel mode where
// each process in a parallel job simultaneously attempts to read the piece
// corresponding to the local process id.
//

#ifndef __vtkPSLACReader_h
#define __vtkPSLACReader_h

#include "vtkSLACReader.h"

#include <vtkstd/vector>        // For interal lists
#include <vtksys/hash_map.hxx>  // For internal maps

class vtkMultiProcessController;

struct vtkSLACReaderIdTypeHash {
  size_t operator()(vtkIdType val) const { return val; }
};

class vtkPSLACReader : public vtkSLACReader
{
public:
  vtkTypeRevisionMacro(vtkPSLACReader, vtkSLACReader);
  static vtkPSLACReader *New();
  virtual void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // The controller used to communicate partition data.  The number of pieces
  // requested must agree with the number of processes, the piece requested must
  // agree with the local process id, and all process must invoke
  // ProcessRequests of this filter simultaneously.
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  virtual void SetController(vtkMultiProcessController *);

protected:
  vtkPSLACReader();
  ~vtkPSLACReader();

  vtkMultiProcessController *Controller;

  virtual int RequestInformation(vtkInformation *request,
                                 vtkInformationVector **inputVector,
                                 vtkInformationVector *outputVector);

  virtual int RequestData(vtkInformation *request,
                          vtkInformationVector **inputVector,
                          vtkInformationVector *outputVector);

  virtual int ReadConnectivity(int meshFD, vtkMultiBlockDataSet *output);
  virtual int ReadCoordinates(int meshFD, vtkMultiBlockDataSet *output);
  virtual int ReadFieldData(int modeFD, vtkMultiBlockDataSet *output);
  virtual int ReadMidpointCoordinates(int meshFD, vtkMultiBlockDataSet *output,
                                      vtkMidpointCoordinateMap &map);
  virtual int ReadMidpointData(int meshFD, vtkMultiBlockDataSet *output);

  virtual int ReadTetrahedronInteriorArray(int meshFD,
                                           vtkIdTypeArray *connectivity);
  virtual int ReadTetrahedronExteriorArray(int meshFD,
                                           vtkIdTypeArray *connectivity);

//BTX
  // Description:
  // Reads point data arrays.  Called by ReadCoordinates and ReadFieldData.
  virtual vtkSmartPointer<vtkDataArray> ReadPointDataArray(int ncFD, int varId);
//ETX

//BTX
  // Description:
  // A map from local point ids to global ids.  Can also be used as the
  // global point ids.
  typedef vtksys::hash_map<vtkIdType, vtkIdType, vtkSLACReaderIdTypeHash>
    GlobalToLocalIdType;
  GlobalToLocalIdType GlobalToLocalIds;
  vtkSmartPointer<vtkIdTypeArray> LocalToGlobalIds;

  // Description:
  // The point data we expect to receive from each process.
  vtkSmartPointer<vtkIdTypeArray> PointsExpectedFromProcessesLengths;
  vtkSmartPointer<vtkIdTypeArray> PointsExpectedFromProcessesOffsets;

  // Description:
  // The point data we have to send to each process.  Stored as global ids.
  vtkSmartPointer<vtkIdTypeArray> PointsToSendToProcesses;
  vtkSmartPointer<vtkIdTypeArray> PointsToSendToProcessesLengths;
  vtkSmartPointer<vtkIdTypeArray> PointsToSendToProcessesOffsets;
  
  // Description:
  // The edge data we expect to receive from each process.
  vtkSmartPointer<vtkIdTypeArray> EdgesExpectedFromProcessesLengths;
  vtkSmartPointer<vtkIdTypeArray> EdgesExpectedFromProcessesOffsets;

  // Description:
  // The edge data we have to send to each process.  Stored as global ids.
  vtkSmartPointer<vtkIdTypeArray> EdgesToSendToProcesses;
  vtkSmartPointer<vtkIdTypeArray> EdgesToSendToProcessesLengths;
  vtkSmartPointer<vtkIdTypeArray> EdgesToSendToProcessesOffsets;
  
//ETX

  // Description:
  // The number of pieces and the requested piece to load.  Synonymous with
  // the number of processes and the local process id.
  int NumberOfPieces;
  int RequestedPiece;

  // Description:
  // The number of points defined in the mesh file.
  vtkIdType NumberOfGlobalPoints;

  // Description:
  // The number of midpoints defined in the mesh file
  vtkIdType NumberOfGlobalMidpoints;

  // Description:
  // The start/end points read by the given process.
  vtkIdType StartPointRead(int process) {
    return process*(this->NumberOfGlobalPoints/this->NumberOfPieces + 1);
  }
  vtkIdType EndPointRead(int process) {
    vtkIdType result = this->StartPointRead(process+1);
    if (result > this->NumberOfGlobalPoints) result=this->NumberOfGlobalPoints;
    return result;
  }

private:
  vtkPSLACReader(const vtkPSLACReader &);       // Not implemented
  void operator=(const vtkPSLACReader &);       // Not implemented
};

#endif //__vtkPSLACReader_h
