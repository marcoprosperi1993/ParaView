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
/*
 * Copyright 2008 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */
// .SECTION Thanks
// Thanks to Janine Bennett, Philippe Pebay, and David Thompson from Sandia National Laboratories 
// for implementing this test.

#include <mpi.h>

#include "vtkPKMeansStatistics.h"

#include "vtkMath.h"
#include "vtkMPIController.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkStdString.h"
#include "vtkTable.h"
#include "vtkTimerLog.h"
#include "vtkVariantArray.h"
#include "vtkIdTypeArray.h"
#include "vtkDoubleArray.h"

// For debugging purposes, output results of serial engines ran on each slice of the distributed data set
#define PRINT_ALL_SERIAL_STATS 0 

struct RandomSampleStatisticsArgs
{
  int nVals;
  int* retVal;
  int ioRank;
  int argc;
  char** argv;
};

// This will be called by all processes
void RandomSampleStatistics( vtkMultiProcessController* controller, void* arg )
{
  // Get test parameters
  RandomSampleStatisticsArgs* args = reinterpret_cast<RandomSampleStatisticsArgs*>( arg );
  *(args->retVal) = 0;

  // Get MPI communicator
  vtkMPICommunicator* com = vtkMPICommunicator::SafeDownCast( controller->GetCommunicator() );

  // Get local rank
  int myRank = com->GetLocalProcessId();

  // Seed random number generator
  vtkMath::RandomSeed( static_cast<int>( vtkTimerLog::GetUniversalTime() ) * ( myRank + 1 ) );

  // Generate an input table that contains samples of mutually independent random variables over [0, 1]
  int nUniform = 2;
  int nNormal  = 2;
  int nVariables = nUniform + nNormal;

  vtkTable* inputData = vtkTable::New();
  vtkDoubleArray* doubleArray;
  vtkStdString columnNames[] = { "Standard Uniform 0", 
                                 "Standard Uniform 1",
                                 "Standard Normal 0",
                                 "Standard Normal 1" };
  
  // Generate samples
  for ( int c = 0; c < nVariables; ++ c )
    {
    doubleArray = vtkDoubleArray::New();
    doubleArray->SetNumberOfComponents( 1 );
    doubleArray->SetName( columnNames[c] );

    double x;
    for ( int r = 0; r < args->nVals; ++ r )
      {
      if( c < nUniform ) 
        {
        x = vtkMath::Random();
        }
      else 
        {
        x = vtkMath::Gaussian();
        }
      doubleArray->InsertNextValue( x );
      }
    
    inputData->AddColumn( doubleArray );
    doubleArray->Delete();
    }

  // set up a single set of param data - send out to all and make tables...
  vtkTable* paramData = vtkTable::New();
  vtkIdTypeArray* paramCluster;
  vtkDoubleArray* paramArray;
  const int numRuns = 5;
  const int numClustersInRun[] = { 5, 2, 3, 4, 5 };
  paramCluster = vtkIdTypeArray::New();
  paramCluster->SetName( "K" );

  for( int curRun = 0; curRun < numRuns; curRun++ )
    {
    for( int nInRun = 0; nInRun < numClustersInRun[curRun]; nInRun++ )
      {
      paramCluster->InsertNextValue( numClustersInRun[curRun] );
      }
    }
  paramData->AddColumn( paramCluster );
  paramCluster->Delete();

  int totalNumClusters = 0;
  for( int i = 0; i < numRuns; i++ )
    {
    totalNumClusters += numClustersInRun[i];
    }

  int nClusterCoords = totalNumClusters*nVariables;
  double* clusterCoords = new double[nClusterCoords];

  // generate data on one node only
  if( myRank == args->ioRank )
    {
    int cIndex = 0;
    for ( int c = 0; c < nVariables; ++ c )
      {
      double x;
      for( int curRun = 0; curRun < numRuns; curRun++ )
        {
        for( int nInRun = 0; nInRun < numClustersInRun[curRun]; nInRun++ )
          {
          if( c < nUniform )
            {
            x = vtkMath::Random();
            }
          else 
            {
            x = vtkMath::Gaussian();
            }
          clusterCoords[cIndex++] = x;
          }
        }
      }
    }

  // broadcast data to all nodes 
  if( !com->Broadcast( clusterCoords, nClusterCoords, args->ioRank) )
    {
    cout << "Process " <<  myRank << " could not broadcast Initial Cluster Coordinates." << endl;
    return;
    }

  for ( int c = 0; c < nVariables; ++ c )
    {
    paramArray = vtkDoubleArray::New();
    paramArray->SetName( columnNames[c] ); 
    paramArray->SetNumberOfTuples( totalNumClusters ); 
    memcpy( paramArray->GetPointer( 0 ), &(clusterCoords[c*totalNumClusters]), totalNumClusters*sizeof( double ) );
    paramData->AddColumn( paramArray );
    paramArray->Delete();
    }

  delete [] clusterCoords;

  // ************************** KMeans Statistics ************************** 

  // Synchronize and start clock
  com->Barrier();
  vtkTimerLog *timer=vtkTimerLog::New();
  timer->StartTimer();

  // Instantiate a parallel KMeans statistics engine and set its ports
  vtkPKMeansStatistics* pks = vtkPKMeansStatistics::New();
  pks->SetInput( vtkStatisticsAlgorithm::INPUT_DATA, inputData );
  pks->SetInput( vtkStatisticsAlgorithm::LEARN_PARAMETERS, paramData );

  // Select columns for testing
  pks->SetColumnStatus( inputData->GetColumnName( 0 ) , 1 );
  pks->SetColumnStatus( inputData->GetColumnName( 2 ) , 1 );
  pks->SetColumnStatus( inputData->GetColumnName( 1 ) , 1 );
  pks->RequestSelectedColumns();

  // Test (in parallel) with Learn, Derive, and Assess options turned on
  pks->SetLearnOption( true );
  pks->SetDeriveOption( true );
  pks->SetAssessOption( true );
  pks->Update();

    // Synchronize and stop clock
  com->Barrier();
  timer->StopTimer();

  if ( myRank == args->ioRank )
    {
    vtkMultiBlockDataSet* outputMetaDS = vtkMultiBlockDataSet::SafeDownCast( pks->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );

    cout << "\n## Completed parallel calculation of kmeans statistics (with assessment):\n"
         << "   Wall time: "
         << timer->GetElapsedTime()
         << " sec.\n";
    for ( unsigned int b = 0; b < outputMetaDS->GetNumberOfBlocks(); ++ b )
      {
      vtkTable* outputMeta = vtkTable::SafeDownCast( outputMetaDS->GetBlock( b ) );
      if ( b == 0 )
        {
        cout << "Computed clusters:" << "\n";
        }
      else
        {
        cout << "Ranked cluster: " << "\n";
        }
        outputMeta->Dump();
      }
    }
  // Clean up
  pks->Delete();
  inputData->Delete();
  paramData->Delete();
  timer->Delete();
}

//----------------------------------------------------------------------------
int main( int argc, char** argv )
{
  // **************************** MPI Initialization *************************** 
  vtkMPIController* controller = vtkMPIController::New();
  controller->Initialize( &argc, &argv );

  // If an MPI controller was not created, terminate in error.
  if ( ! controller->IsA( "vtkMPIController" ) )
    {
    vtkGenericWarningMacro("Failed to initialize a MPI controller.");
    controller->Delete();
    return 1;
    } 

  vtkMPICommunicator* com = vtkMPICommunicator::SafeDownCast( controller->GetCommunicator() );

  // ************************** Find an I/O node ******************************** 
  int* ioPtr;
  int ioRank;
  int flag;

  MPI_Attr_get( MPI_COMM_WORLD, 
                MPI_IO,
                &ioPtr,
                &flag );

  if ( ( ! flag ) || ( *ioPtr == MPI_PROC_NULL ) )
    {
    // Getting MPI attributes did not return any I/O node found.
    ioRank = MPI_PROC_NULL;
    vtkGenericWarningMacro("No MPI I/O nodes found.");

    // As no I/O node was found, we need an unambiguous way to report the problem.
    // This is the only case when a testValue of -1 will be returned
    controller->Finalize();
    controller->Delete();
    
    return -1;
    }
  else 
    {
    if ( *ioPtr == MPI_ANY_SOURCE )
      {
      // Anyone can do the I/O trick--just pick node 0.
      ioRank = 0;
      }
    else
      {
      // Only some nodes can do I/O. Make sure everyone agrees on the choice (min).
      com->AllReduce( ioPtr,
                      &ioRank,
                      1,
                      vtkCommunicator::MIN_OP );
      }
    }
  
  // ************************** Initialize test ********************************* 
  if ( com->GetLocalProcessId() == ioRank )
    {
    cout << "\n# Process "
         << ioRank
         << " will be the I/O node.\n";
    }
      
  // Check how many processes have been made available
  int numProcs = controller->GetNumberOfProcesses();
  if ( controller->GetLocalProcessId() == ioRank )
    {
    cout << "\n# Running test with "
         << numProcs
         << " processes...\n";
    }

  // Parameters for regression test.
  int testValue = 0;
  RandomSampleStatisticsArgs args;
  args.nVals = 100;
  args.retVal = &testValue;
  args.ioRank = ioRank;
  args.argc = argc;
  args.argv = argv;

  // Execute the function named "process" on both processes
  controller->SetSingleMethod( RandomSampleStatistics, &args );
  controller->SingleMethodExecute();

  // Clean up and exit
  if ( com->GetLocalProcessId() == ioRank )
    {
    cout << "\n# Test completed.\n\n";
    }

  controller->Finalize();
  controller->Delete();
  
  return testValue;
}
