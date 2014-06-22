/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_dlg_addexisting_sln
*
* Description: Contains declarations for classes and functions required to support dialog boxes:
*  - add existing solution
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_ADDEXISTING_DLG_H__
#define __KDEVPART_VSTUDIOPART_ADDEXISTING_DLG_H__

/* Qt */
#include <qwidget.h>
#include <qvbox.h>
#include <qpushbutton.h>

/* KDE */
#include <kfile.h>
#include <kdiroperator.h>

/* VStudio */
#include "vs_model.h"

#include "vs_dlg_addexisting_base.h"

class KFile;

//BEGIN //VStudio namespace
namespace VStudio {
  /*! Own drag and drop dir operator type
   * NOTE: Consider to move this as well as buildtools::autotools::KDnDDirOperator
   *   classes to something more higher in dir hierarchy, to make it common
   */
  class KDnDDirOperator : public KDirOperator {
    Q_OBJECT
    public:
      KDnDDirOperator(const KURL &urlName=KURL(), QWidget *parent=0, const char *name=0);

    protected:
      virtual KFileView* createView(QWidget *parent, KFile::FileView view);
  };

  /*! Own implementation of drag and drop enabled file selector for dialogs like:
   * - add existing solution
   * - add existing project
   * - add existing file
   */
  class FileSelectorWidget : public QWidget {
    Q_OBJECT
    public:
      FileSelectorWidget(VSPart *part, KFile::Mode, QWidget *parent=0, const char *name=0);
      ~FileSelectorWidget();

      KDnDDirOperator* dirOperator() const;

    private:
      QPushButton *home, *up, *back, *forward;
      KDnDDirOperator *dir;
      VSPart *part;

    signals:
      void dropped(const QString&);
  };

  class AddExistingSlnDlg : public AddExistingDlgBase {
    public:
      AddExistingSlnDlg(VSPart* part);

    private:
      VSPart *part;
  };
}; /* namespace VStudio */
//END //VStudio namespace

#endif /* __KDEVPART_VSTUDIOPART_ADDEXISTING_DLG_H__ */