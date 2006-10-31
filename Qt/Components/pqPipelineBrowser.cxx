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

=========================================================================*/

/// \file pqPipelineBrowser.cxx
/// \date 4/20/2006

#include "pqPipelineBrowser.h"

#include "pqApplicationCore.h"
#include "pqFlatTreeView.h"
#include "pqPipelineBrowserContextMenu.h"
#include "pqPipelineBuilder.h"
#include "pqPipelineDisplay.h"
#include "pqPipelineModel.h"
#include "pqPipelineModelSelectionAdaptor.h"
#include "pqPipelineSource.h"
#include "pqServer.h"
#include "pqGenericViewModule.h"
#include "pqServerManagerModel.h"
#include "pqServerManagerObserver.h"

#include <QEvent>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QVBoxLayout>

#include "vtkSMProxy.h"


pqPipelineBrowser::pqPipelineBrowser(QWidget *widgetParent)
  : QWidget(widgetParent)
{
  this->ListModel = 0;
  this->TreeView = 0;

  // Get the pipeline model from the pipeline data.
  this->ListModel = new pqPipelineModel(this);
  QObject::connect(this, SIGNAL(viewModuleChanged(pqGenericViewModule*)),
                   this->ListModel, SLOT(setViewModule(pqGenericViewModule*)));

  // Connect the model to the ServerManager model.
  pqServerManagerModel* smModel = 
    pqApplicationCore::instance()->getServerManagerModel();
  
  QObject::connect(smModel, SIGNAL(serverAdded(pqServer*)),
    this->ListModel, SLOT(addServer(pqServer*)));
  QObject::connect(smModel, SIGNAL(aboutToRemoveServer(pqServer *)),
    this->ListModel, SLOT(startRemovingServer(pqServer *)));
  QObject::connect(smModel, SIGNAL(serverRemoved(pqServer*)),
    this->ListModel, SLOT(removeServer(pqServer*)));
  QObject::connect(smModel, SIGNAL(sourceAdded(pqPipelineSource*)),
    this->ListModel, SLOT(addSource(pqPipelineSource*)));
  QObject::connect(smModel, SIGNAL(sourceRemoved(pqPipelineSource*)),
    this->ListModel, SLOT(removeSource(pqPipelineSource*)));
  QObject::connect(smModel, 
    SIGNAL(connectionAdded(pqPipelineSource*, pqPipelineSource*)),
    this->ListModel, 
    SLOT(addConnection(pqPipelineSource*, pqPipelineSource*)));
  QObject::connect(smModel, 
    SIGNAL(connectionRemoved(pqPipelineSource*, pqPipelineSource*)),
    this->ListModel, 
    SLOT(removeConnection(pqPipelineSource*, pqPipelineSource*)));

  QObject::connect(smModel, SIGNAL(nameChanged(pqServerManagerModelItem *)),
    this->ListModel, SLOT(updateItemName(pqServerManagerModelItem *)));
  QObject::connect(smModel,
    SIGNAL(sourceDisplayChanged(pqPipelineSource *, pqConsumerDisplay*)),
    this->ListModel,
    SLOT(updateDisplays(pqPipelineSource *, pqConsumerDisplay*)));

  // Create a flat tree view to display the pipeline.
  this->TreeView = new pqFlatTreeView(this);
  if(this->TreeView)
    {
    this->TreeView->setObjectName("PipelineView");
    this->TreeView->getHeader()->hide();
    this->TreeView->setModel(this->ListModel);
    this->TreeView->installEventFilter(this);
    this->TreeView->getHeader()->moveSection(1, 0);
    //this->TreeView->setSelectionBehavior(pqFlatTreeView::SelectRows);
    this->TreeView->setSelectionMode(pqFlatTreeView::ExtendedSelection);

    // Listen to the selection change signals.
    QItemSelectionModel *selection = this->TreeView->getSelectionModel();
    if(selection)
      {
      connect(selection,
          SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(changeCurrent(const QModelIndex &, const QModelIndex &)));
      }

    // Listen for index clicked signals to change visibility.
    QObject::connect(this->TreeView, SIGNAL(clicked(const QModelIndex &)),
        this, SLOT(handleIndexClicked(const QModelIndex &)));

    // Make sure the tree items get expanded when new descendents
    // are added.
    if(this->ListModel)
      {
      connect(this->ListModel, SIGNAL(firstChildAdded(const QModelIndex &)),
          this->TreeView, SLOT(expand(const QModelIndex &)));
      }

    // The tree view should have a context menu based on the selected
    // items. The context menu policy should be set to custom for this
    // behavior.
    this->TreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    new pqPipelineBrowserContextMenu(this);
    }

  // Add the tree view to the layout.
  QVBoxLayout *boxLayout = new QVBoxLayout(this);
  if(boxLayout)
    {
    boxLayout->setMargin(0);
    boxLayout->addWidget(this->TreeView);
    }

  // Create the adaptor.
  new pqPipelineModelSelectionAdaptor(this->TreeView->getSelectionModel(),
    pqApplicationCore::instance()->getSelectionModel(), this);
}

pqPipelineBrowser::~pqPipelineBrowser()
{
}

//-----------------------------------------------------------------------------
bool pqPipelineBrowser::eventFilter(QObject *object, QEvent *e)
{
  if(object == this->TreeView && e->type() == QEvent::KeyPress)
    {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
    if(keyEvent->key() == Qt::Key_Delete ||
        keyEvent->key() == Qt::Key_Backspace)
      {
      this->deleteSelected();
      }
    }

  return QWidget::eventFilter(object, e);
}

//-----------------------------------------------------------------------------
QItemSelectionModel *pqPipelineBrowser::getSelectionModel() const
{
  if(this->TreeView)
    {
    return this->TreeView->getSelectionModel();
    }

  return 0;
}

//-----------------------------------------------------------------------------
pqServer *pqPipelineBrowser::getCurrentServer() const
{
  pqServerManagerModelItem* item = this->getCurrentSelection();
  pqServer* server = dynamic_cast<pqServer*>(item);
  if (server)
    {
    return server;
    }
  pqPipelineSource* src = dynamic_cast<pqPipelineSource*>(item);
  if (src)
    {
    return src->getServer();
    }
  return 0;
}

//-----------------------------------------------------------------------------
pqServerManagerModelItem* pqPipelineBrowser::getCurrentSelection() const
{
  QItemSelectionModel *selectionModel = this->getSelectionModel();
  if(selectionModel && this->ListModel)
    {
    QModelIndex index = selectionModel->currentIndex();
    return this->ListModel->getItemFor(index);
    }
  return 0;
}
/*
//-----------------------------------------------------------------------------
vtkSMProxy *pqPipelineBrowser::getSelectedProxy() const
{
  QItemSelectionModel *selectionModel = this->getSelectionModel();
  if(selectionModel && this->ListModel)
    {
    return this->ListModel->getProxyFor(selectionModel->currentIndex());
    }

  return 0;
}

vtkSMProxy *pqPipelineBrowser::getNextProxy() const
{
  QItemSelectionModel *selectionModel = this->getSelectionModel();
  if(selectionModel && this->ListModel)
    {
    pqPipelineSource *source = this->ListModel->getSourceFor(
        selectionModel->currentIndex());
    if(source && source->getOutputCount() == 1)
      {
      source = dynamic_cast<pqPipelineSource *>(source->getOutput(0));
      if(source)
        {
        return source->getProxy();
        }
      }
    }

  return 0;
}
*/
//-----------------------------------------------------------------------------
void pqPipelineBrowser::select(pqServerManagerModelItem* item)
{
  QModelIndex index = this->ListModel->getIndexFor(item);
  // This not only changes the current selection, but also clears
  // any previous selection.
  this->TreeView->getSelectionModel()->setCurrentIndex(index,
    QItemSelectionModel::SelectCurrent | QItemSelectionModel::Clear);
//  emit this->selectionChanged(item); 
}

//-----------------------------------------------------------------------------
void pqPipelineBrowser::select(pqPipelineSource* src)
{
  this->select((pqServerManagerModelItem*)src);
}

//-----------------------------------------------------------------------------
void pqPipelineBrowser::select(pqServer* server)
{
  this->select((pqServerManagerModelItem*)server);
}

//-----------------------------------------------------------------------------
void pqPipelineBrowser::deleteSelected()
{
  // Get the selected item(s) from the selection model.
  QModelIndex current = this->TreeView->getSelectionModel()->currentIndex();
  pqServerManagerModelItem *item = this->ListModel->getItemFor(current);
  pqPipelineSource *source = qobject_cast<pqPipelineSource *>(item);
  pqServer *server = qobject_cast<pqServer *>(item);
  if(source)
    {
    pqApplicationCore::instance()->removeSource(source);
    }
  else if(server)
    {
    pqApplicationCore::instance()->removeServer(server);
    }
}

//-----------------------------------------------------------------------------
void pqPipelineBrowser::changeCurrent(const QModelIndex &current,
    const QModelIndex &)
{
  if(this->ListModel)
    {
    // Get the current item from the model.
    pqServerManagerModelItem* item = this->ListModel->getItemFor(current);

    emit this->selectionChanged(item); 
    }
}

//-----------------------------------------------------------------------------
void pqPipelineBrowser::handleIndexClicked(const QModelIndex &index)
{
  // See if the index is associated with a source.
  pqPipelineSource *source = dynamic_cast<pqPipelineSource *>(
      this->ListModel->getItemFor(index));
  if(source)
    {
    if(index.column() == 1)
      {
      // If the column clicked is 1, the user clicked the visible icon.
      // Get the display object for the current window.
      pqConsumerDisplay* display = source->getDisplay(this->ViewModule);

      // If the display exists, toggle the display. Otherwise, create a
      // display for the source in the current window.
      if(!display)
        {
        display = this->createDisplay(source, true);
        }
      else
        {
        display->setVisible(!display->isVisible());
        }
      if (display)
        {
        display->renderAllViews(false);
        }
      }
    // TODO
    //else if(index.column() == 2)
    //  {
    //  // If the column clicked is 2, the user clicked the input menu.
    //  }
    }
}

//-----------------------------------------------------------------------------
pqConsumerDisplay* pqPipelineBrowser::createDisplay(pqPipelineSource* source, 
  bool visible)
{
  if (!this->ViewModule || !this->ViewModule->canDisplaySource(source))
    {
    return 0;
    }
  pqApplicationCore* core = pqApplicationCore::instance();
  pqConsumerDisplay* display = 
    core->getPipelineBuilder()->createDisplay(source, this->ViewModule);
  display->setVisible(visible);
  return display;
}

//-----------------------------------------------------------------------------
void pqPipelineBrowser::setViewModule(pqGenericViewModule* rm)
{
  this->ViewModule = rm;
  emit this->viewModuleChanged(rm);
}

//-----------------------------------------------------------------------------
pqGenericViewModule* pqPipelineBrowser::getViewModule()
{
  return this->ViewModule;
}


