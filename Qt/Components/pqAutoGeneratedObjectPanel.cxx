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

// this include
#include "pqAutoGeneratedObjectPanel.h"

// Qt includes
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>

// VTK includes

// ParaView Server Manager includes
#include "vtkSmartPointer.h"
#include "vtkSMDomain.h"
#include "vtkSMDomainIterator.h"
#include "vtkSMProperty.h"
#include "vtkSMOrderedPropertyIterator.h"
#include "vtkSMStringVectorProperty.h"

// ParaView includes
#include "pqApplicationCore.h"
#include "pqFileDialog.h"
#include "pqPipelineSource.h"
#include "pqPropertyManager.h"
#include "pqProxy.h"
#include "pqReaderFactory.h"
#include "pqSMAdaptor.h"
#include "pqServerFileDialogModel.h"
#include "pqTreeWidgetItemObject.h"
#include "pqFileChooserWidget.h"


static QLabel* createPanelLabel(QWidget* parent, QString text)
{
  QLabel* label = new QLabel(parent);
  label->setText(text);
  label->setWordWrap(true);
  return label;
}

//-----------------------------------------------------------------------------
/// constructor
pqAutoGeneratedObjectPanel::pqAutoGeneratedObjectPanel(pqProxy* object_proxy, QWidget* p) :
  pqNamedObjectPanel(object_proxy, p)
{
  this->PanelLayout = new QGridLayout(this);
  this->createWidgets();
  this->linkServerManagerProperties();
}

//-----------------------------------------------------------------------------
/// destructor
pqAutoGeneratedObjectPanel::~pqAutoGeneratedObjectPanel()
{
}

//-----------------------------------------------------------------------------
void pqAutoGeneratedObjectPanel::setupValidator(QLineEdit* lineEdit, 
  QVariant::Type type)
{
  switch (type)
    {
  case QVariant::Double:
    lineEdit->setValidator(new QDoubleValidator(lineEdit));
    break;

  case QVariant::Int:
    lineEdit->setValidator(new QIntValidator(lineEdit));
    break;

  default:
    break;
    }
}

//-----------------------------------------------------------------------------
void pqAutoGeneratedObjectPanel::linkServerManagerProperties()
{
  pqNamedObjectPanel::linkServerManagerProperties();
  
  QPixmap cellPixmap(":/pqWidgets/Icons/pqCellData16.png");
  QPixmap pointPixmap(":/pqWidgets/Icons/pqPointData16.png");

  // link point/cell array status widget
  QTreeWidget* treeWidget;
  vtkSMProperty* cellArrayStatus;
  vtkSMProperty* pointArrayStatus;
  treeWidget = this->findChild<QTreeWidget*>("CellAndPointArrayStatus");
  cellArrayStatus = this->proxy()->getProxy()->GetProperty("CellArrayStatus");
  pointArrayStatus = this->proxy()->getProxy()->GetProperty("PointArrayStatus");

  if(treeWidget && cellArrayStatus && pointArrayStatus)
    {
    QList<QVariant> sel_domain;
    sel_domain = pqSMAdaptor::getSelectionPropertyDomain(cellArrayStatus);
    for(int j=0; j<sel_domain.size(); j++)
      {
      QList<QString> str;
      str.append(sel_domain[j].toString());
      pqTreeWidgetItemObject* item;
      item = new pqTreeWidgetItemObject(treeWidget, str);
      item->setData(0, Qt::DecorationRole, cellPixmap);
      this->propertyManager()->registerLink(item, 
                                        "checked", 
                                        SIGNAL(checkedStateChanged(bool)),
                                        this->proxy()->getProxy(), 
                                        cellArrayStatus, j);
      }
    sel_domain = pqSMAdaptor::getSelectionPropertyDomain(pointArrayStatus);
    for(int j=0; j<sel_domain.size(); j++)
      {
      QList<QString> str;
      str.append(sel_domain[j].toString());
      pqTreeWidgetItemObject* item;
      item = new pqTreeWidgetItemObject(treeWidget, str);
      item->setData(0, Qt::DecorationRole, pointPixmap);
      this->propertyManager()->registerLink(item,
                                        "checked", 
                                        SIGNAL(checkedStateChanged(bool)),
                                        this->proxy()->getProxy(), 
                                        pointArrayStatus, j);
      }
    }
}

void pqAutoGeneratedObjectPanel::createWidgets()
{
  int rowCount = 0;
  int skippedFirstFileProperty = 0;

  // query for proxy properties, and create widgets
  vtkSMOrderedPropertyIterator *iter = vtkSMOrderedPropertyIterator::New();
  iter->SetProxy(this->proxy()->getProxy());
  
  int hasCellArrayStatus = 0;
  int hasPointArrayStatus = 0;

  // check for both CellArrayStatus & PointArrayStatus
  for(iter->Begin(); !iter->IsAtEnd(); iter->Next())
    {
    if(QString(iter->GetKey()) == QString("CellArrayStatus"))
      {
      hasCellArrayStatus = 1;
      }
    if(QString(iter->GetKey()) == QString("PointArrayStatus"))
      {
      hasPointArrayStatus = 1;
      }
    }

  int hasCellPointArrayStatus = 0;
  if(hasPointArrayStatus && hasCellArrayStatus)
    {
    hasCellPointArrayStatus = 1;
    }
  int cellPointArrayStatusWidgetAdded = 0;

  for(iter->Begin(); !iter->IsAtEnd(); iter->Next())
    {
    vtkSMProperty* SMProperty = iter->GetProperty();

    QString propertyName = iter->GetKey();
    propertyName.replace(':', '_');
    propertyName.replace(' ', '_');
    QString propertyLabel = iter->GetKey();
    // replace "MixedCase" names with "Mixed Case"
    for(int i=1; i < propertyLabel.size(); i++)
      {
      QChar c = propertyLabel.at(i);
      if(c.isUpper())
        {
        propertyLabel.replace(i, 1, QString(" %1").arg(c));
        i++;
        }
      }

    // skip information properties
    if(SMProperty->GetInformationOnly() || SMProperty->GetIsInternal())
      {
      continue;
      }

    if (SMProperty->IsA("vtkSMStringVectorProperty") && !skippedFirstFileProperty)
      {
      // Do not show the first file property. We do not allow changing of
      // the main filename from the gui. The user has to create another
      // instance of the reader and reconnect the pipeline.
      vtkSmartPointer<vtkSMDomainIterator> diter;
      diter.TakeReference(SMProperty->NewDomainIterator());
      diter->Begin();
      while(!diter->IsAtEnd())
        {
        if (diter->GetDomain()->IsA("vtkSMFileListDomain"))
          {
          break;
          }
        diter->Next();
        }
      if (!diter->IsAtEnd())
        {
        skippedFirstFileProperty = 1;
        continue;
        }
      }

    // update domains we might ask for
    SMProperty->UpdateDependentDomains();

    pqSMAdaptor::PropertyType pt = pqSMAdaptor::getPropertyType(SMProperty);
    QList<QString> domainsTypes = pqSMAdaptor::getDomainTypes(SMProperty);

    // skip input properties
    if(pt == pqSMAdaptor::PROXY || pt == pqSMAdaptor::PROXYLIST)
      {
      if(SMProperty == this->proxy()->getProxy()->GetProperty("Input"))
        {
        continue;
        }
      }

    if(pt == pqSMAdaptor::PROXY)
      {
      // create a combo box with list of proxies
      QComboBox* combo = new QComboBox(this->PanelLayout->parentWidget());
      combo->setObjectName(propertyName);
      QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                       propertyLabel);
      this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
      this->PanelLayout->addWidget(combo, rowCount, 1, 1, 1);
      rowCount++;
      }
    else if(pt == pqSMAdaptor::PROXYLIST)
      {
      // create a list of selections of proxies
      }
    else if (pt==pqSMAdaptor::PROXYSELECTION)
      {
      // create a combo box with list of proxies
      QComboBox* combo = new QComboBox(this->PanelLayout->parentWidget());
      combo->setObjectName(propertyName);
      QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                       propertyLabel);
      this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
      this->PanelLayout->addWidget(combo, rowCount, 1, 1, 1);
      rowCount++;

      QGroupBox* group = new QGroupBox(this->PanelLayout->parentWidget());
      group->setObjectName(QString("WidgetBox.") + propertyName);
      group->setLayout(new QVBoxLayout(group));
      this->PanelLayout->addWidget(group, rowCount, 0, 1, 2);
      group->hide();
      rowCount++;
      }
    else if(pt == pqSMAdaptor::ENUMERATION)
      {
      QVariant enum_property = pqSMAdaptor::getEnumerationProperty(SMProperty);
      if(enum_property.type() == QVariant::Bool)
        {
        // check box for true/false
        QCheckBox* check;
        check = new QCheckBox(propertyLabel, 
                              this->PanelLayout->parentWidget());
        check->setObjectName(propertyName);
        this->PanelLayout->addWidget(check, rowCount, 0, 1, 2);
        rowCount++;
        }
      else
        {
        // combo box with strings
        QComboBox* combo = new QComboBox(this->PanelLayout->parentWidget());
        combo->setObjectName(propertyName);
        QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                         propertyLabel);

        this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
        this->PanelLayout->addWidget(combo, rowCount, 1, 1, 1);
        rowCount++;
        }
      }
    else if(pt == pqSMAdaptor::SELECTION)
      {
      int doIt = 1;
      QString name = propertyName;
      QString header = propertyLabel;

      if((propertyName == QString("CellArrayStatus") ||
         propertyName == QString("PointArrayStatus")) &&
         hasCellPointArrayStatus == 1)
        {
        if(cellPointArrayStatusWidgetAdded == 1)
          {
          doIt = 0;
          }
        else
          {
          cellPointArrayStatusWidgetAdded = 1;
          name = "CellAndPointArrayStatus";
          header = "Cell/Point Array Status";
          }
        }
      if(doIt)
        {
        QList<QList<QVariant> > items;
        items = pqSMAdaptor::getSelectionProperty(SMProperty);
        QTreeWidget* tw = new QTreeWidget(this->PanelLayout->parentWidget());
        tw->setColumnCount(1);
        tw->setRootIsDecorated(false);
        QTreeWidgetItem* h = new QTreeWidgetItem();
        h->setData(0, Qt::DisplayRole, header);
        tw->setHeaderItem(h);
        tw->setObjectName(name);
        this->PanelLayout->addWidget(tw, rowCount, 0, 1, 2);
        rowCount++;
        }
      }
    else if(pt == pqSMAdaptor::SINGLE_ELEMENT)
      {
      QVariant elem_property = pqSMAdaptor::getElementProperty(SMProperty);
      QList<QVariant> propertyDomain;
      propertyDomain = pqSMAdaptor::getElementPropertyDomain(SMProperty);
      if(elem_property.type() == QVariant::String && propertyDomain.size())
        {
        // combo box with strings
        QComboBox* combo = new QComboBox(this->PanelLayout->parentWidget());
        combo->setObjectName(propertyName);
        QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                         propertyLabel);
        
        this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
        this->PanelLayout->addWidget(combo, rowCount, 1, 1, 1);
        rowCount++;
        }
      else if(elem_property.type() == QVariant::Int && 
              propertyDomain.size() == 2)
        {
        QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                         propertyLabel);
        QSlider* slider;
        slider = new QSlider(Qt::Horizontal, this->PanelLayout->parentWidget());
        slider->setObjectName(QString(propertyName) + "_Slider");
        slider->setRange(propertyDomain[0].toInt(), propertyDomain[1].toInt());

        QLineEdit* lineEdit = new QLineEdit(this->PanelLayout->parentWidget());
        lineEdit->setObjectName(propertyName);
        this->setupValidator(lineEdit, elem_property.type()); 
        this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
        QHBoxLayout* hlayout = new QHBoxLayout;
        hlayout->addWidget(slider);
        hlayout->addWidget(lineEdit);
        this->PanelLayout->addLayout(hlayout, rowCount, 1, 1, 1);
        slider->show();
        lineEdit->show();
        rowCount++;
        }
      else if(elem_property.type() == QVariant::Double && 
              propertyDomain.size() == 2 && 
              domainsTypes.contains("vtkSMDoubleRangeDomain"))
        {
        double range[2];
        range[0] = propertyDomain[0].toDouble();
        range[1] = propertyDomain[1].toDouble();
        QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                         propertyLabel);
        QDoubleSpinBox* spinBox;
        spinBox = new QDoubleSpinBox(this->PanelLayout->parentWidget());
        spinBox->setObjectName(propertyName);
        spinBox->setRange(range[0], range[1]);
        spinBox->setSingleStep((range[1] - range[0]) / 20.0);

        this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
        this->PanelLayout->addWidget(spinBox, rowCount, 1, 1, 1);
        rowCount++;
        }
      else
        {
        QLineEdit* lineEdit = new QLineEdit(this->PanelLayout->parentWidget());
        lineEdit->setObjectName(propertyName);
        this->setupValidator(lineEdit, elem_property.type()); 

        QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                         propertyLabel);
        
        this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
        this->PanelLayout->addWidget(lineEdit, rowCount, 1, 1, 1);
        rowCount++;
        }
      }
    else if(pt == pqSMAdaptor::MULTIPLE_ELEMENTS)
      {
      QList<QVariant> list_property;
      list_property = pqSMAdaptor::getMultipleElementProperty(SMProperty);
      QList<QList<QVariant> > domain;
      domain = pqSMAdaptor::getMultipleElementPropertyDomain(SMProperty);
      
      QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                       propertyLabel);
      this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
      QGridLayout* glayout = new QGridLayout;
      glayout->setObjectName(propertyName);
      this->PanelLayout->addLayout(glayout, rowCount, 1, 1, 1);

      // we have a few different layouts
      static const int LayoutThreeByTwo = 0;
      static const int LayoutRow = 1;
      static const int LayoutColumn = 2;

      // let's peek at what property types and domains we have to determine
      // which layout to use
      int layoutMethod = LayoutRow;
      if(list_property.size() == 6)
        {
        layoutMethod = LayoutThreeByTwo;
        }
      else if(list_property.size() > 3)  // that many won't fit in a row
        {
        layoutMethod = LayoutColumn;
        }

      int i=0;
      foreach(QVariant v, list_property)
        {
        QLayoutItem* item = NULL;

        // create the widget(s)
        if( 0 /*v.type() == QVariant::String && 
           domain.size() && domain[i].size() */)  // link not supported yet
          {
          QComboBox* combo = new QComboBox(this->PanelLayout->parentWidget());
          combo->setObjectName(QString("%1_%2").arg(propertyName).arg(i));
          item = new QWidgetItem(combo);
          }
        else if(v.type() == QVariant::Int && 
           domain.size() && domain[i].size() == 2)
          {
          int range[2];
          range[0] = domain[i][0].toInt();
          range[1] = domain[i][1].toInt();
          QSpinBox* spinBox;
          spinBox = new QSpinBox(this->PanelLayout->parentWidget());
          spinBox->setObjectName(QString("%1_%2").arg(propertyName).arg(i));
          spinBox->setRange(range[0], range[1]);
          item = new QWidgetItem(spinBox);
          }
        else if(v.type() == QVariant::Double && 
                domain.size() && domain[i].size()== 2)
          {
          double range[2];
          range[0] = domain[i][0].toDouble();
          range[1] = domain[i][1].toDouble();
          QDoubleSpinBox* spinBox;
          spinBox = new QDoubleSpinBox(this->PanelLayout->parentWidget());
          spinBox->setObjectName(QString("%1_%2").arg(propertyName).arg(i));
          spinBox->setRange(range[0], range[1]);
          spinBox->setSingleStep((range[1] - range[0]) / 20.0);
          item = new QWidgetItem(spinBox);
          }
        else
          {
          QLineEdit* lineEdit = new QLineEdit(this->PanelLayout->parentWidget());
          lineEdit->setObjectName(QString("%1_%2").arg(propertyName).arg(i));
          this->setupValidator(lineEdit, v.type()); 
          item = new QWidgetItem(lineEdit);
          }

        // insert the widget(s) in the layout
        if(layoutMethod == LayoutThreeByTwo)
          {
          glayout->addItem(item, i/2, i%2, 1, 1);
          }
        else if(layoutMethod == LayoutRow)
          {
          glayout->addItem(item, 0, i, 1, 1);
          }
        else if(layoutMethod == LayoutColumn)
          {
          glayout->addItem(item, i, 0, 1, 1);
          }


        // widgets in sub-layouts need help being shown
        QLayout* l = item->layout();
        if(l)
          {
          int count = l->count();
          for(int k=0; k<count; k++)
            {
            QLayoutItem* li = l->itemAt(k);
            if(li->widget())
              {
              li->widget()->show();
              }
            }
          }

        i++;
        }
        rowCount++;
      }
    else if(pt == pqSMAdaptor::FILE_LIST)
      {
      pqFileChooserWidget* chooser;
      chooser = new pqFileChooserWidget(this->PanelLayout->parentWidget());
      chooser->setServer(this->proxy()->getServer());
      chooser->setObjectName(propertyName);
      QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                       propertyLabel);
      
      this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
      this->PanelLayout->addWidget(chooser, rowCount, 1, 1, 1);
      rowCount++;
      }
    else if(pt == pqSMAdaptor::FIELD_SELECTION)
      {
      QLabel* label = createPanelLabel(this->PanelLayout->parentWidget(),
                                       "Scalars");
      QComboBox* combo = new QComboBox(this->PanelLayout->parentWidget());
      combo->setObjectName(QString(propertyName));
      this->PanelLayout->addWidget(label, rowCount, 0, 1, 1);
      this->PanelLayout->addWidget(combo, rowCount, 1, 1, 1);
      rowCount++;
      }
    }
  iter->Delete();
  this->PanelLayout->addItem(new QSpacerItem(0,0,
                                             QSizePolicy::Expanding,
                                             QSizePolicy::Expanding), 
                             rowCount, 0, 1, 2);
  this->PanelLayout->invalidate();
}

