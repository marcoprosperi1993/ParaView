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

/// \file pqFlatTreeView.h
/// \date 3/27/2006

#ifndef _pqFlatTreeView_h
#define _pqFlatTreeView_h


#include "QtWidgetsExport.h"
#include <QAbstractScrollArea>
#include <QModelIndex>          // Needed for return type
#include <QStyleOptionViewItem> // Needed for return type

class pqFlatTreeViewItem;
class pqFlatTreeViewItemRows;
class pqFlatTreeViewInternal;

class QAbstractItemModel;
class QColor;
class QFontMetrics;
class QHeaderView;
class QItemSelection;
class QItemSelectionModel;
class QPoint;


/// \class pqFlatTreeView
/// \brief
///   The pqFlatTreeView class is used to display a flattened tree
///   view of a hierarchical model.
///
/// The tree view is flattened by taking long indented chains of
/// single items and lining them up vertically. If an item is the
/// only descendent of its parent item, it is drawn directly below
/// its parent. A vertical branch is drawn between the items to
/// indicate relationship. If an item has more than one descendent,
/// those items are indented from the parent. Normal tree view
/// branches are drawn between the parent and child items to show
/// the relationship.
class QTWIDGETS_EXPORT pqFlatTreeView : public QAbstractScrollArea
{
  Q_OBJECT

public:
  enum SelectionBehavior
    {
    SelectItems,
    SelectRows,
    SelectColumns
    };

  enum SelectionMode
    {
    NoSelection,
    SingleSelection,
    ExtendedSelection
    };

public:
  /// \brief
  ///   Creates a flat tree view.
  /// \param parent The parent widget for this instance.
  pqFlatTreeView(QWidget *parent=0);
  virtual ~pqFlatTreeView();

  /// \brief
  ///   Used to monitor the header view.
  ///
  /// When the header view is shown or hidden, the layout needs to be
  /// updated and repainted.
  ///
  /// \param object The object which will receive the event.
  /// \param e The event to be sent.
  virtual bool eventFilter(QObject *object, QEvent *e);

  /// \name Model Setup Methods
  //@{
  QAbstractItemModel *getModel() const {return this->Model;}
  void setModel(QAbstractItemModel *model);

  QModelIndex getRootIndex() const;
  void setRootIndex(const QModelIndex &index);
  //@}

  /// \name Selection Setup Methods
  //@{
  QItemSelectionModel *getSelectionModel() const {return this->Selection;}
  void setSelectionModel(QItemSelectionModel *selectionModel);

  SelectionBehavior getSelectionBehavior() const {return this->Behavior;}
  void setSelectionBehavior(SelectionBehavior behavior);

  SelectionMode getSelectionMode() const {return this->Mode;}
  void setSelectionMode(SelectionMode mode);
  //@}

  /// \name Column Management Methods
  //@{
  QHeaderView *getHeader() const {return this->HeaderView;}
  void setHeader(QHeaderView *headerView);

  /// \brief
  ///   Gets whether or not the column size is managed by the view.
  ///
  /// Column size management is on by default and used when the
  /// view header is hidden. When size management is on, the columns
  /// will be resized to fit the model data in the column.
  ///
  /// \return
  ///   True if the column size is managed by the view.
  bool isColumnSizeManaged() const {return this->ManageSizes;}

  /// \brief
  ///   Sets whether or not the column size is managed by the view.
  /// \param managed True if the column size should be managed.
  /// \sa
  ///   pqFlatTreeView::isColumnSizeManaged()
  void setColumnSizeManaged(bool managed);
  //@}

  QModelIndex getIndexVisibleAt(const QPoint &point) const;
  QModelIndex getIndexCellAt(const QPoint &point) const;

  /// \name Editing Methods
  //@{
  bool startEditing(const QModelIndex &index);
  void finishEditing();
  void cancelEditing();
  //@}

signals:
  void activated(const QModelIndex &index);
  void clicked(const QModelIndex &index);

public slots:
  void reset();
  void selectAll();
  void expand(const QModelIndex &index);
  void collapse(const QModelIndex &index);
  void scrollTo(const QModelIndex &index);

protected slots:
  /// \name Model Change Handlers
  //@{
  void insertRows(const QModelIndex &parent, int start, int end);
  void startRowRemoval(const QModelIndex &parent, int start, int end);
  void finishRowRemoval(const QModelIndex &parent, int start, int end);
  void insertColumns(const QModelIndex &parent, int start, int end);
  void startColumnRemoval(const QModelIndex &parent, int start, int end);
  void finishColumnRemoval(const QModelIndex &parent, int start, int end);
  void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight);
  //@}

protected:
  /// \name Keyboard Event Handlers
  //@{
  virtual void keyPressEvent(QKeyEvent *e);
  void keyboardSearch(const QString &search);
  //@}

  /// \name Mouse Event Handlers
  //@{
  virtual void mousePressEvent(QMouseEvent *e);
  virtual void mouseMoveEvent(QMouseEvent *e);
  virtual void mouseReleaseEvent(QMouseEvent *e);
  virtual void mouseDoubleClickEvent(QMouseEvent *e);
  virtual void wheelEvent(QWheelEvent *e);
  //@}

  int horizontalOffset() const;
  int verticalOffset() const;

  virtual void resizeEvent(QResizeEvent *e);
  virtual bool viewportEvent(QEvent *e);
  virtual void paintEvent(QPaintEvent *e);
  QStyleOptionViewItem getViewOptions() const;

private slots:
  void handleSectionResized(int index, int oldSize, int newSize);
  void handleSectionMoved(int index, int oldVisual, int newVisual);

  void changeCurrent(const QModelIndex &current, const QModelIndex &previous);
  void changeCurrentRow(const QModelIndex &current,
      const QModelIndex &previous);
  void changeCurrentColumn(const QModelIndex &current,
      const QModelIndex &previous);
  void changeSelection(const QItemSelection &selected,
      const QItemSelection &deselected);

private:
  void resetRoot();
  void resetPreferredSizes();
  void layoutEditor();
  void layoutItems();
  void layoutItem(pqFlatTreeViewItem *item, int &point,
      const QFontMetrics &fm);
  int getDataWidth(const QModelIndex &index, const QFontMetrics &fm) const;
  int getWidthSum(pqFlatTreeViewItem *item, int column) const;
  bool updateContentsWidth();
  void updateScrollBars();
  void addChildItems(pqFlatTreeViewItem *item, int parentChildCount);
  bool getIndexRowList(const QModelIndex &index,
      pqFlatTreeViewItemRows &rowList) const;
  pqFlatTreeViewItem *getItem(const pqFlatTreeViewItemRows &rowList) const;
  pqFlatTreeViewItem *getItem(const QModelIndex &index) const;
  pqFlatTreeViewItem *getItemAt(int contentsY) const;
  pqFlatTreeViewItem *getNextVisibleItem(pqFlatTreeViewItem *item) const;
  pqFlatTreeViewItem *getPreviousVisibleItem(pqFlatTreeViewItem *item) const;
  pqFlatTreeViewItem *getLastVisibleItem() const;

  void drawBranches(QPainter &painter, pqFlatTreeViewItem *item,
      int halfIndent, const QColor &branchColor, const QColor &expandColor,
      QStyleOptionViewItem &options);

private:
  QAbstractItemModel *Model;
  QItemSelectionModel *Selection;
  SelectionBehavior Behavior;
  SelectionMode Mode;
  QHeaderView *HeaderView;
  pqFlatTreeViewItem *Root;
  pqFlatTreeViewInternal *Internal;
  int ItemHeight;
  int IndentWidth;
  int ContentsWidth;
  int ContentsHeight;
  bool FontChanged;
  bool ManageSizes;
  bool InUpdateWidth;
  bool HeaderOwned;
  bool SelectionOwned;

  static int TextMargin;
  static int DoubleTextMargin;
  static int PipeLength;
};

#endif
