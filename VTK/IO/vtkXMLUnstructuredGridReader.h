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
// .NAME vtkXMLUnstructuredGridReader - Read VTK XML UnstructuredGrid files.
// .SECTION Description
// vtkXMLUnstructuredGridReader reads the VTK XML UnstructuredGrid
// file format.  One unstructured grid file can be read to produce one
// output.  Streaming is supported.  The standard extension for this
// reader's file format is "vtu".  This reader is also used to read a
// single piece of the parallel file format.

// .SECTION See Also
// vtkXMLPUnstructuredGridReader

#ifndef __vtkXMLUnstructuredGridReader_h
#define __vtkXMLUnstructuredGridReader_h

#include "vtkXMLUnstructuredDataReader.h"

class vtkUnstructuredGrid;

class VTK_IO_EXPORT vtkXMLUnstructuredGridReader : public vtkXMLUnstructuredDataReader
{
public:
  vtkTypeRevisionMacro(vtkXMLUnstructuredGridReader,vtkXMLUnstructuredDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);  
  static vtkXMLUnstructuredGridReader *New();
  
  // Description:
  // Get/Set the reader's output.
  void SetOutput(vtkUnstructuredGrid *output);
  vtkUnstructuredGrid *GetOutput();
  
protected:
  vtkXMLUnstructuredGridReader();
  ~vtkXMLUnstructuredGridReader();
  
  const char* GetDataSetName();
  void GetOutputUpdateExtent(int& piece, int& numberOfPieces, int& ghostLevel);
  void SetupOutputTotals();
  void SetupPieces(int numPieces);
  void DestroyPieces();
  
  void SetupOutputData();
  int ReadPiece(vtkXMLDataElement* ePiece);
  void SetupNextPiece();
  int ReadPieceData();
  
  // Read a data array whose tuples coorrespond to cells.
  int ReadArrayForCells(vtkXMLDataElement* da, vtkDataArray* outArray);
  
  // The index of the cell in the output where the current piece
  // begins.
  vtkIdType StartCell;
  
  // The Cells element for each piece.
  vtkXMLDataElement** CellElements;
  vtkIdType* NumberOfCells;
  
private:
  vtkXMLUnstructuredGridReader(const vtkXMLUnstructuredGridReader&);  // Not implemented.
  void operator=(const vtkXMLUnstructuredGridReader&);  // Not implemented.
};

#endif
