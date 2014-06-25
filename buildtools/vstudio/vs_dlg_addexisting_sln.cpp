/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_dlg_addexisting_sln
*
* Description: Contains definitions for dialog box classes for:
*  - add existing solution
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/

/* Qt */
#include <qlayout.h>
#include <qtooltip.h>

/* KDE */
#include <klocale.h>

/* KDevelop */
#include <kfilednddetailview.h>
#include <kfiledndiconview.h>

#include "vs_dlg_addexisting_sln.h"

namespace VStudio {
  //===========================================================================
  // Visual studio DnD dir operator methods
  //===========================================================================
  KDnDDirOperator::KDnDDirOperator(const KURL &urlName/*=KURL()*/, QWidget *pnt/*=0*/, const char *name/*=0*/)
  : KDirOperator(urlName, pnt, name) {
  }

  KFileView* KDnDDirOperator::createView(QWidget *pnt, KFile::FileView view) {
    KFileView* new_view = 0;

    if(check_bit(view, KFile::Detail)) {
      new_view = new KFileDnDDetailView(pnt, "detail view");
    }
    else if(check_bit(view, KFile::Simple)) {
      new_view = new KFileDnDIconView(pnt, "icon view");
      new_view->setViewName(i18n("Short View"));
    }

    return new_view;
  }

  //===========================================================================
  // Visual studio file selector methods
  //===========================================================================
  FileSelectorWidget::FileSelectorWidget(VSPart *p, KFile::Mode mode, QWidget *pnt/*=0*/, const char *name/*=0*/)
  : QWidget(pnt, name)
  , part(p) {
    QVBoxLayout *vl = new QVBoxLayout(this);

    QHBox *hb = new QHBox(this);
    vl->addWidget(hb);

    home = new QPushButton(hb);
    home->setPixmap(SmallIcon("gohome"));
    QToolTip::add(home, i18n("Home directory"));
    up = new QPushButton(hb);
    up->setPixmap(SmallIcon("up"));
    QToolTip::add(up, i18n("Up one level"));
    back = new QPushButton(hb);
    back->setPixmap(SmallIcon("back"));
    QToolTip::add(back, i18n("Previous direcrtory"));
    forward = new QPushButton(hb);
    forward->setPixmap(SmallIcon("forward"));
    QToolTip::add(forward, i18n("Next directory"));

    dir = new KDnDDirOperator(KURL(), this, "operator");
    dir->setView(KFile::Simple);
    dir->setMode(mode);

    vl->addWidget(dir);
    vl->setStretchFactor(dir, 2);

    connect(home, SIGNAL(clicked()), dir, SLOT(home()));
    connect(up, SIGNAL(clicked()), dir, SLOT(cdUp()));
    connect(back, SIGNAL(clicked()), dir, SLOT(back()));
    connect(forward, SIGNAL(clicked()), dir, SLOT(forward()));

    connect(dir, SIGNAL(urlEntered(const KURL&)), this, SLOT(dirUrlEntered(const KURL&)));
    connect(dir, SIGNAL(finishedLoading()), this, SLOT(dirFinishedLoading()));
  }

  FileSelectorWidget::~FileSelectorWidget() {
  }

  KDnDDirOperator* FileSelectorWidget::dirOperator() const {
    return dir;
  }

  //===========================================================================
  // Visual studio AddExistingSlnDlg methods
  //===========================================================================
  AddExistingSlnDlg::AddExistingSlnDlg(VSPart *prt)
  : part(prt) {
  }
}; /* namespace VStudio */

#include "vs_dlg_addexisting_sln.moc"