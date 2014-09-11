/*  C/C++ Implementation: vs_dlg_addexisting_sln
*
* Description: Contains definitions for dialog box classes for:
*  - add existing solution
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/

/* Qt */
#include <qapplication.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qgroupbox.h>
#include <qregexp.h>

/* KDE */
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogress.h>

/* KDevelop */
#include <kfilednddetailview.h>
#include <kfiledndiconview.h>
#include <kimporticonview.h>

#include "vs_part.h"
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

    //HACK:
    QWidget *spacer = new QWidget(hb);
    hb->setStretchFactor(spacer, 1);
    hb->setMaximumHeight(up->height());

    pathcombo = new KURLComboBox(KURLComboBox::Directories, true, this, "path combo");
    pathcombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    KURLCompletion *cmpurl = new KURLCompletion();
    pathcombo->setCompletionObject(cmpurl);
    vl->addWidget(pathcombo);

    dir = new KDnDDirOperator(KURL(), this, "operator");
    dir->setView(KFile::Simple);
    dir->setMode(mode);

    vl->addWidget(dir);
    vl->setStretchFactor(dir, 2);

    QHBox *filterbox = new QHBox(this);
    fltico = new QLabel(filterbox);
    fltico->setPixmap(BarIcon("filter"));
    fltcombo = new KHistoryCombo(filterbox, "filter");
    fltcombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    filterbox->setStretchFactor(fltcombo, 2);
    vl->addWidget(filterbox);

    connect(fltcombo, SIGNAL(textChanged(const QString&)), this, SLOT(slotFilterChanged(const QString&)));
    connect(fltcombo, SIGNAL(activated(const QString&)), this, SLOT(slotFilterChanged(const QString&)));
    connect(fltcombo, SIGNAL(returnPressed(const QString&)), this, SLOT(filterReturnPressed(const QString&)));

    connect(home, SIGNAL(clicked()), dir, SLOT(home()));
    connect(up, SIGNAL(clicked()), dir, SLOT(cdUp()));
    connect(back, SIGNAL(clicked()), dir, SLOT(back()));
    connect(forward, SIGNAL(clicked()), dir, SLOT(forward()));

    connect(pathcombo, SIGNAL(urlActivated(const KURL&)), this, SLOT(cmbPathActivated(const KURL&)));
    connect(pathcombo, SIGNAL(returnPressed(const QString&)), this, SLOT(cmbPathReturnPressed(const QString&)));

    connect(dir, SIGNAL(urlEntered(const KURL&)), this, SLOT(dirUrlEntered(const KURL&)));
    connect(dir, SIGNAL(finishedLoading()), this, SLOT(dirFinishedLoading()));

    QStringList list;

    QDomElement doc_e = part->projectDom()->documentElement();
    QDomElement grp_e = doc_e.namedItem("kdevfileview").namedItem("groups").firstChild().toElement();

    while(!grp_e.isNull()) {
      if(grp_e.tagName().compare("group") == 0) {
        list << grp_e.attribute("pattern").replace(QRegExp(";"), " ") + " (" + grp_e.attribute("name") + ")";
      }
      grp_e = grp_e.nextSibling().toElement();
    }
    fltcombo->setHistoryItems(list);
  }

  FileSelectorWidget::~FileSelectorWidget() {
  }

  void FileSelectorWidget::slotFilterChanged(const QString &nf) {
    dir->setNameFilter(nf);
    dir->updateDir();
  }

  void FileSelectorWidget::setDir(KURL &url) {
    dir->setURL(url, true);
  }

  void FileSelectorWidget::setDir(const QString &u) {
    dir->setURL(KURL(u), true);
  }

  void FileSelectorWidget::cmbPathActivated(const KURL &u) {
    dir->setURL(u, true);
  }

  void FileSelectorWidget::cmbPathReturnPressed(const QString &u) {
    dir->setFocus();
    dir->setURL(KURL(u), true);
  }

  void FileSelectorWidget::dirUrlEntered(const KURL &u) {
    pathcombo->removeURL(u);
    QStringList urls = pathcombo->urls();
    urls.prepend(u.url());
    while(urls.count() >= static_cast<uint>(pathcombo->maxItems())) {
      urls.remove(urls.last());
    }
    pathcombo->setURLs(urls);
  }

  void FileSelectorWidget::dirFinishedLoading() {
    //HACK: enable nav buttons to have to wait the diroperator...
    home->setEnabled(dir->actionCollection()->action("home")->isEnabled());
    up->setEnabled(dir->actionCollection()->action("up")->isEnabled());
    back->setEnabled(dir->actionCollection()->action("back")->isEnabled());
    forward->setEnabled(dir->actionCollection()->action("forward")->isEnabled());
  }

  void FileSelectorWidget::filterReturnPressed(const QString &nf) {
    setDir(nf);
  }

  void FileSelectorWidget::focusInEvent(QFocusEvent */*e*/) {
    dir->setFocus();
  }

  void FileSelectorWidget::dragEnterEvent(QDragEnterEvent */*e*/) {
  }

  void FileSelectorWidget::dropEvent(QDropEvent *e) {
#ifdef DEBUG
    kddbg << "Drop event.\n";
#endif
    QString path="Something was dropped into destination directory file-selector";
    emit dropped(path);
  }

  KDnDDirOperator* FileSelectorWidget::dirOperator() const {
    return dir;
  }

  //===========================================================================
  // Visual studio AddExistingSlnDlg methods
  //===========================================================================
  AddExistingSlnDlg::AddExistingSlnDlg(VSPart *prt, QWidget *p/*=0*/, const char *nm/*=0*/, bool m/*=TRUE*/,
                                       WFlags f/*=0*/)
  : AddExistingDlgBase(prt->explorerWidget(), nm, m, f)
  , fselector(0)
  , part(prt) {
    KFile::Mode mode = KFile::Files;
    fselector = new FileSelectorWidget(part, mode, grb_sourcedir, "sln file selector");
    grb_sourcedirLayout->addWidget(fselector);
    iview = new KImportIconView("Drag solutions you need to this box", grb_destination, "destination icon view");
    iview->setSelectionMode(KFile::Multi);
    grb_destinationLayout->addWidget(iview);
    fselector->setFocus();
    setIcon(SmallIcon("fileimport.png"));

    // Init
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slotOk()));
    connect(iview, SIGNAL(dropped(QDropEvent*)), this, SLOT(slotDropped(QDropEvent*)));

    pbr_adding->hide();
    iview->setMode(KIconView::Select);
    iview->setItemsMovable(false);
    fselector->setDir(part->projectDirectory());
  }

  AddExistingSlnDlg::~AddExistingSlnDlg() {
  }

  void AddExistingSlnDlg::slotDropped(QDropEvent* ev) {
    KURL::List urls;
    KURLDrag::decode(ev, urls);

    KFileItem *item = 0;
    KMimeType::Ptr type = 0;

    for(KURL::List::iterator it=urls.begin(); it!=urls.end(); ++it) {
      if((*it).isLocalFile()) {
        type = KMimeType::findByURL((*it));

        if(type->name() != KMimeType::defaultMimeType()) {
          item = new KFileItem((*it), type->name(), 0);
        }
        else {
          //NOTE: We'll set text icon instead icon with question mark
          item = new KFileItem((*it), "text/plain", 0);
        }
        ilist.append(item);
      }
    }
    importItems();
  }

  void AddExistingSlnDlg::slotOk() {
    if(iview->items() != 0) {
      if(iview->items()->count() != 0) {
        pbr_adding->show();
        pbr_adding->setFormat(i18n("Importing... %p%"));

        qApp->processEvents();

        KFileItemListIterator items(*(iview->items()));
        items.toFirst();

        // QMap<QString, QString> replaceMap;
        // FileItem *fitem = 0;
        // QStringList filelist;

        for(; items.current(); ++items) {
          KURL url((*items)->url());
          kddbg << " add sln [" << (*items)->name() << "] \"" << url.url() << "\"\n";
          // filelist.append(n_path);
          if(!part->loadVsSolution((*items)->name(), url)) {
            kddbg << QString("Can't load sln [%1] \"%2\".\n").arg((*items)->name()).arg((*items)->localPath());
            KMessageBox::error(this, "Failed to load solution", "Adding fail");
            return;
          }
          pbr_adding->setValue(pbr_adding->value() + 1);
        }
        QDialog::accept();
      }
      else {
        QDialog::reject();
      }
    }
  }

  void AddExistingSlnDlg::importItems() {
    if(iview->items() != 0) {
      KFileItemListIterator itemlist(ilist); // Items added via drag and drop
      KFileItemListIterator implist(*(iview->items())); // Items already added to the import view

      QStringList dupslist;

      // Prevent adding duplicated solutions
      BOOSTVEC_FOR(vse_ci, it, part->solutions()) {
        itemlist.toFirst();
        vss_p sln = static_cast<vss_p>(*it);
        if(sln != 0) {
          for(; itemlist.current(); ++itemlist) {
            if(sln->getURL().cmp((*itemlist)->url(), false)) {
              dupslist.append((*itemlist)->name());
              ilist.remove((*itemlist));
            }
            else if(sln->getName().compare((*itemlist)->name()) == 0) {
              dupslist.append((*itemlist)->name());
              ilist.remove((*itemlist));
            }
          }
        }
      }

      // Remove all dups from already added items list
      implist.toFirst();
      for(; implist.current(); ++implist) {
        itemlist.toFirst();

        for(; itemlist.current(); ++itemlist) {
          if((*implist)->name().compare((*itemlist)->name()) == 0) {
            ilist.remove((*itemlist));

            if(!dupslist.remove((*implist)->name())) {
              dupslist.append((*implist)->name());
            }
          }
        }
      }

      if(dupslist.count() > 0) {
        if(KMessageBox::warningContinueCancelList(this, i18n("Following files are already loaded into project.\n"
           "Under same or other name, absolute paths are compared too.\n"
           "Press \"Continue\" to import only new files.\nPress \"Cancel\" to abort import at all."), dupslist,
                                                 "Warning", KGuiItem(i18n("Continue"))) == KMessageBox::Cancel) {
          ilist.clear();
          return;
        }
      }

      itemlist.toFirst();

      for(; itemlist.current(); ++itemlist) {
        if(!(*itemlist)->isDir()) {
          iview->insertItem((*itemlist));
        }
      }

      iview->somethingDropped(true);
      ilist.clear();
      iview->update();
    }
  }
}; /* namespace VStudio */

#include "vs_dlg_addexisting_sln.moc"