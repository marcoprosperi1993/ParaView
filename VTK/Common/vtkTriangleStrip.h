/*=========================================================================

  Program:   Visualization Library
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Description:
---------------------------------------------------------------------------
This file is part of the Visualization Library. No part of this file
or its contents may be copied, reproduced or altered in any way
without the express written consent of the authors.

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen 1993, 1994 

=========================================================================*/
//
// Computational class for triangle strips.
//
#ifndef __vlTriangleStrip_h
#define __vlTriangleStrip_h

#include "Cell.hh"

class vlTriangleStrip : public vlCell
{
public:
  vlTriangleStrip() {};
  char *GetClassName() {return "vlTriangleStrip";};

  float DistanceToPoint(float *x);

};

#endif


