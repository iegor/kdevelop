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
#include <kcombobox.h>
#include <kurlcombobox.h>
#include <kurlcompletion.h>

/* KDevelop */
#include <kimporticonview.h>

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

    public slots:
      void slotFilterChanged(const QString &flt);
      void setDir(KURL& url);
      void setDir(const QString &dir);

    private slots:
      void cmbPathActivated(const KURL &url);
      void cmbPathReturnPressed(const QString &url);
      void dirUrlEntered(const KURL &url);
      void dirFinishedLoading();
      void filterReturnPressed(const QString &flt);

    protected:
      void focusInEvent(QFocusEvent *focus_event);
      void dragEnterEvent(QDragEnterEvent *drag_enter_event);
      void dropEvent(QDropEvent *drop_event);

    private:
      KURLComboBox *pathcombo;
      KHistoryCombo *fltcombo;
      QLabel *fltico;
      QPushButton *home, *up, *back, *forward;
      KDnDDirOperator *dir;
      VSPart *part;

    signals:
      void dropped(const QString&);
  };

  class AddExistingSlnDlg : public AddExistingDlgBase {
    Q_OBJECT
    public:
      AddExistingSlnDlg(VSPart* part, QWidget *parent=0, const char *name=0, bool modal=TRUE, WFlags fl=0);
      ~AddExistingSlnDlg();

    protected slots:
      void slotDropped(QDropEvent* drop_event);
      void slotOk();

    protected:
      void importItems();

    private:
      FileSelectorWidget *fselector; // File selector widget
      KImportIconView *iview; // Import icon-view widget
      VSPart *part;
      KFileItemList ilist; // List of imported items via drag&drop
  };
}; /* namespace VStudio */
//END //VStudio namespace

#endif /* __KDEVPART_VSTUDIOPART_ADDEXISTING_DLG_H__ */