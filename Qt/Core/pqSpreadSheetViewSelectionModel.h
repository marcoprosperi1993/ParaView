/*=========================================================================

   Program: ParaView
   Module:    $RCSfile$

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.1. 

   See License_v1.1.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#ifndef __pqSpreadSheetViewSelectionModel_h 
#define __pqSpreadSheetViewSelectionModel_h

#include <QItemSelectionModel>
#include "pqCoreExport.h"

class pqSpreadSheetViewModel;
class vtkSMSourceProxy;


/// This is the selection model used by spread sheet view. It manages two
/// operations:
/// \li When user changes the selection by clicking on the view, it updates the
///     selection source proxy on the visible source proxy to reflect the 
///     current selection.
/// \li When vtkSelection changes, is updated the selection in the view
///     accordingly.
class PQCORE_EXPORT pqSpreadSheetViewSelectionModel : public QItemSelectionModel
{
  Q_OBJECT
  typedef QItemSelectionModel Superclass;
public:
  pqSpreadSheetViewSelectionModel(pqSpreadSheetViewModel* model, QObject* parent=0);
  ~pqSpreadSheetViewSelectionModel();

public slots:
  virtual void select(const QModelIndex& index, 
    QItemSelectionModel::SelectionFlags command)
    {
    this->Superclass::select(index, command);
    }

  virtual void select(const QItemSelection& selection, 
    QItemSelectionModel::SelectionFlags command);

signals:
  void selection(vtkSMSourceProxy*);

protected slots:
  void serverSelectionChanged();

protected:
  /// Locate the selection source currently set on the representation being shown.
  /// If no selection exists, or selection present is not "updatable" by this
  /// model, we create a new selection.
  vtkSMSourceProxy* getSelectionSource();

  bool UpdatingSelection;

private:
  pqSpreadSheetViewSelectionModel(const pqSpreadSheetViewSelectionModel&); // Not implemented.
  void operator=(const pqSpreadSheetViewSelectionModel&); // Not implemented.

  class pqInternal;
  pqInternal* Internal;
};

#endif


