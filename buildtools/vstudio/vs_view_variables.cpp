/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_view_variables
*
* Description:
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/

//#ifdef USE_BOOST
//#include <boost/foreach.hpp>
//#endif

#include "vs_part.h"
#include "vs_view_variables.h"

namespace VStudio {
  typedef map<QString, QString>::const_iterator vlmap_ci;
  typedef pair<QString, QString> vlpair;
  //===========================================================================
  // VStudio variables widget methods
  //===========================================================================
  VSViewVars::VSViewVars(VSPart *pt, QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : QTable(pnt, nm)
  , part(pt) {
    setNumCols(3);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setShowGrid(true);
    setSelectionMode(QTable::SingleRow);

    QStringList clabels;
    clabels.append("Variable name");
    clabels.append("Variable value");
    clabels.append("Variable description");

    setColumnLabels(clabels);
    setReadOnly(true);
    setDragEnabled(true);
    setDragAutoScroll(false);
    setSorting(false);
  }

  VSViewVars::~VSViewVars() {
  }

  void VSViewVars::resizeEvent(QResizeEvent *e) {
    QScrollView::resizeEvent(e);
    adjustColumn(0);
    adjustColumn(1);
    horizontalHeader()->setStretchEnabled(true, 2);
  }

  void VSViewVars::slotRefreshValues() {
    // Clear table
    int nr=numRows();
    int i=0;
    for(i=0; i>nr; ++i) {
      removeRow(i);
    }

    setNumRows(part->vars().size());

    i=0;
    for(vlmap_ci it=part->vars().begin(); it != part->vars().end(); ++it) {
#ifdef DEBUG
      // kddbg << "Adding item: " << it->first << endl;
#endif
      setItem(i, 0, new QTableItem(this, QTableItem::Never, it->first, SmallIcon("idea")));
      setItem(i, 1, new QTableItem(this, QTableItem::Never, it->second));
      setItem(i, 2, new QTableItem(this, QTableItem::Never, QString::null));
      ++i;
    }

    adjustColumn(0);
    adjustColumn(1);
    horizontalHeader()->setStretchEnabled(true, 2);
  }
}; /* namespace VStudio */