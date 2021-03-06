/*=========================================================================

   Program: ParaView
   Module:    $RCSfile$

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
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
#include "pqCPWritersMenuManager.h"

#include "pqApplicationCore.h"
#include "pqCoreUtilities.h"
#include "pqObjectBuilder.h"
#include "pqOutputPort.h"
#include "pqPipelineFilter.h"
#include "pqProcessModuleGUIHelper.h"
#include "pqServer.h"
#include "pqServerManagerSelectionModel.h"
#include "pqUndoStack.h"
#include "vtkProcessModule.h"
#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"
#include "vtkSmartPointer.h"
#include "vtkSMInputProperty.h"
#include "vtkSMPropertyIterator.h"
#include "vtkSMProxyManager.h"
#include "vtkSMSourceProxy.h"

#include <QAction>
#include <QDebug>
#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

static vtkSMInputProperty* getInputProperty(vtkSMProxy* proxy)
{
  // if "Input" is present, we return that, otherwise the "first"
  // vtkSMInputProperty encountered is returned.

  vtkSMInputProperty *prop = vtkSMInputProperty::SafeDownCast(
    proxy->GetProperty("Input"));
  vtkSMPropertyIterator* propIter = proxy->NewPropertyIterator();
  for (propIter->Begin(); !prop && !propIter->IsAtEnd(); propIter->Next())
    {
    prop = vtkSMInputProperty::SafeDownCast(propIter->GetProperty());
    }

  propIter->Delete();
  return prop;
}

//-----------------------------------------------------------------------------
pqCPWritersMenuManager::pqCPWritersMenuManager(QObject* parentObject):
  Superclass(parentObject)
{
  this->Menu = 0;
  QObject::connect(
    pqApplicationCore::instance()->getSelectionModel(),
    SIGNAL(selectionChanged(const pqServerManagerSelection&,
    const pqServerManagerSelection&)),
    this, SLOT(updateEnableState()));
}

//-----------------------------------------------------------------------------
pqCPWritersMenuManager::~pqCPWritersMenuManager()
{
}

//-----------------------------------------------------------------------------
void pqCPWritersMenuManager::createMenu()
{
  // Load the resource file and parse the XML to add items to the menu.
  QFile xmlFile(":/CoProcessingPlugin/ParaViewResources/Writers.xml");
  if (!xmlFile.open(QIODevice::ReadOnly))
    {
    qCritical() << "Failed to load the Writes.xml resource file."
      << " Plugin has not been built correctly.";
    return;
    }

  QByteArray dat = xmlFile.readAll();
  xmlFile.close();
  vtkSmartPointer<vtkPVXMLParser> parser = 
    vtkSmartPointer<vtkPVXMLParser>::New();
  if (!parser->Parse(dat.data()))
    {
    qCritical() << "Failed to parse resource xml."
      << " Plugin has not been built correctly.";
    return;
    }

  QMainWindow *mainWindow = qobject_cast<QMainWindow*>(
    pqCoreUtilities::mainWidget());

  this->Menu = new QMenu("&Writers", mainWindow);
  this->Menu->setObjectName("CoProcessingWritersMenu");
  mainWindow->menuBar()->addMenu(this->Menu);

  QObject::connect(this->Menu, SIGNAL(triggered(QAction*)),
    this, SLOT(onActionTriggered(QAction*)), Qt::QueuedConnection);

  vtkSMProxyManager* pxm = vtkSMProxyManager::GetProxyManager();

  vtkPVXMLElement* elem = parser->GetRootElement();
  unsigned int num_elems = elem->GetNumberOfNestedElements();
  for (unsigned int cc=0; cc < num_elems; cc++)
    {
    vtkPVXMLElement* curElem = elem->GetNestedElement(cc);
    if (curElem->GetName() && strcmp(curElem->GetName(),"Writer") == 0)
      {
      const char* proxyname = curElem->GetAttribute("proxyname");
      const char* proxygroup = curElem->GetAttribute("proxygroup");
      if (proxygroup && proxyname)
        {
        vtkSMProxy* prototype = pxm->GetPrototypeProxy(proxygroup, proxyname);
        if (!prototype)
          {
          qWarning() << "Failed to locate proxy for writer: " <<
            proxygroup <<" , " << proxyname;
          continue;
          }
        QAction* action = this->Menu->addAction(prototype->GetXMLLabel()?
          prototype->GetXMLLabel() : prototype->GetXMLName());
        QStringList list;
        list << proxygroup << proxyname;
        action->setData(list);
        }
      }
    }

  this->updateEnableState();
}

//-----------------------------------------------------------------------------
void pqCPWritersMenuManager::updateEnableState()
{
  if (!this->Menu)
    {
    return;
    }

  // Get the list of selected sources. Make sure the list contains
  // only valid sources.
  const pqServerManagerSelection *selItems =
    pqApplicationCore::instance()->getSelectionModel()->selectedItems();

  QList<pqOutputPort*> outputPorts;
  pqServerManagerModelItem* item = NULL;
  pqServerManagerSelection::ConstIterator iter = selItems->begin();
  for( ; iter != selItems->end(); ++iter)
    {
    item = *iter;
    pqPipelineSource* source = qobject_cast<pqPipelineSource *>(item);
    pqOutputPort* port = source? source->getOutputPort(0) : 
      qobject_cast<pqOutputPort*>(item);
    if (port)
      {
      outputPorts.append(port);
      }
    }

  // Iterate over all filters in the menu and see if they can be
  // applied to the current source(s).
  bool some_enabled = false;
  vtkSMProxyManager *pxm = vtkSMProxyManager::GetProxyManager();
  QList<QAction *> menu_actions = this->Menu->findChildren<QAction *>();
  foreach( QAction* action, menu_actions)
    {
    QStringList filterType = action->data().toStringList();
    if (filterType.size() != 2)
      {
      continue;
      }
    if (outputPorts.size() == 0)
      {
      action->setEnabled(false);
      continue;
      }

    vtkSMProxy* output = pxm->GetPrototypeProxy(
      filterType[0].toAscii().data(),
      filterType[1].toAscii().data());
    if (!output)
      {
      action->setEnabled(false);
      continue;
      }

    int numProcs = outputPorts[0]->getServer()->getNumberOfPartitions();
    vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(output);
    if (sp && (
        (sp->GetProcessSupport() == vtkSMSourceProxy::SINGLE_PROCESS && numProcs > 1) ||
        (sp->GetProcessSupport() == vtkSMSourceProxy::MULTIPLE_PROCESSES && numProcs == 1)))
      {
      // Skip single process filters when running in multiprocesses and vice
      // versa.
      action->setEnabled(false);
      continue;
      }

    vtkSMInputProperty *input = ::getInputProperty(output); 
    if (input)
      {
      if (!input->GetMultipleInput() && selItems->size() > 1)
        {
        action->setEnabled(false);
        continue;
        }

      input->RemoveAllUncheckedProxies();
      for (int cc=0; cc < outputPorts.size(); cc++)
        {
        pqOutputPort* port = outputPorts[cc];
        input->AddUncheckedInputConnection(
          port->getSource()->getProxy(), port->getPortNumber());
        }

      if (input->IsInDomains())
        {
        action->setEnabled(true);
        some_enabled = true;
        }
      else
        {
        action->setEnabled(false);
        }
      input->RemoveAllUncheckedProxies();
      }
    }

  this->Menu->setEnabled(some_enabled);
}

//-----------------------------------------------------------------------------
void pqCPWritersMenuManager::onActionTriggered(QAction* action)
{
  QStringList filterType = action->data().toStringList();
  if (filterType.size() == 2)
    {
    this->createWriter(filterType[0], filterType[1]);
    }
}

//-----------------------------------------------------------------------------
void pqCPWritersMenuManager::createWriter(const QString& xmlgroup,
  const QString& xmlname)
{
  pqApplicationCore* core = pqApplicationCore::instance();
  pqObjectBuilder* builder = core->getObjectBuilder();  

  vtkSMProxyManager* pxm = vtkSMProxyManager::GetProxyManager();
  vtkSMProxy* prototype = 
    pxm->GetPrototypeProxy(xmlgroup.toAscii().data(), xmlname.toAscii().data());
  if (!prototype)
    {
    qCritical() << "Unknown proxy type: " << xmlname;
    return;
    }

  // Get the list of selected sources.
  pqServerManagerSelection selected =
    *core->getSelectionModel()->selectedItems();
  QList<pqOutputPort*> selectedOutputPorts;
  QMap<QString, QList<pqOutputPort*> > namedInputs;

  // Determine the list of selected output ports.
  foreach (pqServerManagerModelItem* item, selected)
    {
    pqOutputPort* opPort = qobject_cast<pqOutputPort*>(item);
    pqPipelineSource* source = qobject_cast<pqPipelineSource*>(item);
    if (opPort)
      {
      selectedOutputPorts.push_back(opPort);
      }
    else if (source)
      {
      selectedOutputPorts.push_back(source->getOutputPort(0));
      }
    }
  
  QList<const char*> inputPortNames = pqPipelineFilter::getInputPorts(prototype);
  namedInputs[inputPortNames[0]] = selectedOutputPorts;

  pqApplicationCore::instance()->getUndoStack()->beginUndoSet(
    QString("Create '%1'").arg(xmlname));
  pqPipelineSource* filter = builder->createFilter(xmlgroup, xmlname, 
    namedInputs, selectedOutputPorts[0]->getServer());
  (void)filter;
  pqApplicationCore::instance()->getUndoStack()->endUndoSet();
}
