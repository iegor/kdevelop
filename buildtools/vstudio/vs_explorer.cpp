/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs
*
* Description:
*  ! Parts were inherited from autoprojectpart src
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
/* Qt */
#include <qlayout.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qheader.h>
#include <qdir.h>
#include <qstylesheet.h>

/* KDE */
#include <kiconloader.h>
#include <kinstance.h>
#include <kurl.h>
#include <kaction.h>
#include <kactionclasses.h>
#include <kpopupmenu.h>
#include <kconfig.h>
#include <klistview.h>
#include <klocale.h>
#include <kdebug.h> //NOTE: for debug messaging purposes

/* KDevelop */
#include "kdevpartcontroller.h"

// #include <urlutil.h>
// #include <kdevcore.h>
// #include <kdevlanguagesupport.h>
// #include <kdevproject.h>
// #include <kdevpartcontroller.h>
// #include <codemodel.h>
// #include <codemodel_utils.h>

/* VStudio */
#include "vs_part.h"
#include "vs_explorer.h"

class KPopupMenu;

namespace VStudio {
  //===========================================================================
  // ListWidgetItem methods
  //===========================================================================
  ListWidgetItem::ListWidgetItem(QWidget *pnt, const char *nm, WFlags fl)
  : QVBox(pnt, nm, fl)
  , pnt(0)
  , sbl(0)
  , chd(0)
  , maybeTotalHeight(-1)
  , lvl(0) {
  }

  ListWidgetItem::~ListWidgetItem() {
  }

  int ListWidgetItem::totalHeight() {
    if(!isVisible()) { return 0; }
    if(maybeTotalHeight >= 0) { return maybeTotalHeight; }

    maybeTotalHeight = height();

    lwi_p chi = chd;
    while(chi != 0) {
      maybeTotalHeight += chi->totalHeight();
      chi = const_cast<lwi_p>(chi->sibling());
    }

    return maybeTotalHeight;
  }

  void ListWidgetItem::invalidateHeight() {
    if(maybeTotalHeight < 0) { return; }
    maybeTotalHeight = -1;
  }

  vsinline lwi_cp ListWidgetItem::parent() const vsinline_attrib { return pnt; }
  vsinline lwi_cp ListWidgetItem::sibling() const vsinline_attrib { return sbl; }
  vsinline lwi_cp ListWidgetItem::child() const vsinline_attrib { return chd; }
  vsinline void ListWidgetItem::setParent(lwi_cp p) vsinline_attrib { pnt = const_cast<lwi_p>(p); }
  vsinline void ListWidgetItem::setSibling(lwi_cp s) vsinline_attrib { sbl = const_cast<lwi_p>(s); }
  vsinline void ListWidgetItem::setChild(lwi_cp c) vsinline_attrib { chd = const_cast<lwi_p>(c); }
  vsinline int ListWidgetItem::level() const vsinline_attrib { return lvl; }
  vsinline void ListWidgetItem::setLevel(int level) vsinline_attrib { lvl = level; }

  //===========================================================================
  // ListWidget methods
  //===========================================================================
  ListWidget::ListWidget(QWidget *p, const char *nm, WFlags fl)
  : QScrollView(p, nm, fl)
  , lvl_shift(10)
  , focusItem(0)
  , prevFocusItem(0)
  , selectedItem(0) {
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(WheelFocus);
    viewport()->setBackgroundMode(PaletteBase);
    setBackgroundMode(PaletteBackground, PaletteBase);

    // Create root item
    root = new RootItem(this, "lwi_root");
    root->hide();
    root->setEnabled(false);
  }

  ListWidget::~ListWidget() {
    /*BOOSTVEC_FOR(lwi_ci, it, items) {
      lwi_p itm(*it);
      // if(itm != 0) { delete itm; }
    }*/
  }

  int ListWidget::lvlShift() const { return lvl_shift; }
  void ListWidget::setLvlShift(int shift) { lvl_shift = shift; }

  bool ListWidget::insertItem(lwi_p p, lwi_p pnt/*=0*/) {
    if(p != 0) {
      BOOSTVEC_PUSHBACK(items, p);
      addChild(p);
      p->list = this;

      p->invalidateHeight();
      p->totalHeight(); //HACK: force recalc of p->maybeTotalHeight

      if(pnt == 0) {
        pnt = root;
      }

      p->setLevel(pnt->level() + 1);

      // Set item's relations
      p->setSibling(pnt->child());
      pnt->setChild(p);
      p->setParent(pnt);

      p->show();
      p->setEnabled(true);

      updateItems();
      return true;
    }
    return false;
  }

  void ListWidget::updateItems() {
    //NOTE: testing only
    //TODO: do some sorting, for all "trees" (solutions)
    int tw=width();
    int th=0;
    // int max_lvl=0;

    BOOSTVEC_FOR(lwi_ci, it, items) {
      lwi_p item(*it);
      if(item != 0) {
        if(item->lvl == 0) { th += item->totalHeight(); }
          // if(item->lvl > max_lvl) { tw = item->rect().right(); }
      }
    }

    resizeContents(tw, th);

    int ht_cnt=0;
    BOOSTVEC_FOR(lwi_ci, it, items) {
      lwi_p item(*it);
      if(item != 0) {
        moveChild(item, lvl_shift * item->lvl, ht_cnt);
        ht_cnt += item->totalHeight();
      }
    }
  }

  void ListWidget::setFocused(lwi_p item) {
    focusItem = item;
  }

  ListWidget::RootItem::RootItem(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : ListWidgetItem (pnt, nm, fl) {
    lvl = -1;
  }

  ListWidget::RootItem::~RootItem() {
  }

  //===========================================================================
  // VStudio::VSExplorerListWidget methods
  //===========================================================================
  VSExplorerListWidget::VSExplorerListWidget(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : ListWidget(pnt, nm, fl) {
  }

  VSExplorerListWidget::~VSExplorerListWidget() {
  }

  //===========================================================================
  // VStudio::VSExplorerEntity methods
  //===========================================================================
  VSExplorerEntity::VSExplorerEntity(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : ListWidgetItem(pnt, nm, fl)
  , enflg(0) {
    setMinimumSize(QSize(200, 16));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hb_main = new QHBox(this);
    hb_main->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //vbl_item->addWidget(hb_main);
    // DEBUG purposes only
    // setMinimumSize(QSize(100, 32));
    // resize(QSize(100, 32).expandedTo(minimumSizeHint()));
    layout()->setMargin(1); // So that background colorchange would be visible

    // Create service layout
    hb_service = new QHBox(this);
    hb_service->setMargin(0);
    hb_service->setMaximumSize(QSize(75, 400));

    btn_build = new QPushButton(hb_service, "btn_build");
    btn_build->setText(i18n("build"));
    btn_build->setMaximumSize(QSize(75,16));
    btn_build->hide();
    btn_build->setEnabled(false);
    btn_clear = new QPushButton(hb_service, "btn_clear");
    btn_clear->setText(i18n("clear"));
    btn_clear->setMaximumSize(QSize(75,16));
    btn_clear->hide();
    btn_clear->setEnabled(false);
  }

  VSExplorerEntity::~VSExplorerEntity () {
  }

  void VSExplorerEntity::enterEvent(QEvent *e) {
    // VSExplorerListWidget *explorer = static_cast<VSExplorerListWidget*>(list);
    set_bit(enflg, IS_HOVERED_ON);
    setBackgroundMode(Qt::PaletteHighlight);

    explorer->setFocused(this); // Set this item as focused in list

    btn_clear->show();
    btn_clear->setEnabled(true);
    btn_build->show();
    btn_build->setEnabled(true);

    // list->updateItems();
  }

  void VSExplorerEntity::leaveEvent(QEvent *e) {
    // VSExplorerListWidget *explorer = static_cast<VSExplorerListWidget*>(list);
    clear_bit(enflg, IS_HOVERED_ON);
    setBackgroundMode(Qt::PaletteBackground);
    btn_clear->hide();
    btn_clear->setEnabled(false);
    btn_build->hide();
    btn_build->setEnabled(false);

    // list->updateItems();
  }

  void VSExplorerEntity::invalidateHeight() {
    ListWidgetItem::invalidateHeight();
    //if(parent && parent->isOpen()) { parent->invalidateHeight(); }
  }

  //===========================================================================
  // VStudio::explorer widget methods
  //===========================================================================
  VSExplorer::VSExplorer(VSPart * part, QWidget *parent, const char *name)
    : VsExplorerWidget(parent, name)
    , m_part(part)
    , actions(0) {
    // m_listView->addColumn("");
    // m_listView->header()->hide();
    // m_listView->setSorting(0);
    // m_listView->setRootIsDecorated(true);
    // m_listView->setAllColumnsShowFocus(true);
    // m_listView->setItemsRenameable(true);

    // m_listView->setColumnWidthMode(0, QListView::Maximum);
    // m_listView->setSelectionMode(QListView::Multi);

    // connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));

    //NOTE: kind of "track active item in solution explorer"
    // m_actionFollowEditor = new KToggleAction(i18n("Follow Editor"), KShortcut(), this, SLOT(slotFollowEditor()), m_part->actionCollection(), "classview_follow_editor");

    // Configure explorer from config file
    // KConfig* config = m_part->instance()->config();
    // config->setGroup("General");
    // m_doFollowEditor = config->readBoolEntry("FollowEditor", false);

    explorer = new VSExplorerListWidget(this, "vslw_explorer");
    VsExplorerWidgetLayout->addWidget(explorer);

    explorer->enableClipper(true);
    // explorer->setVScrollBarMode(QScrollView::AlwaysOn);
    // explorer->setHScrollBarMode(QScrollView::AlwaysOn);
    // explorer->resizeContents(400, 1000); //TODO: for DEBUG purposes only

    actions = new KActionCollection(this);

    //TODO: Remove that later, that will be part of test only ###########
    actSetEntityRltPath = new KAction(i18n("Set relative path:"), 0, this, SLOT(slotSetEntityRltPath()), actions, "setrelpath");
    actSetEntityRltPath->setToolTip(i18n("Set relative path for entity"));
    actSetEntityRltPath->setWhatsThis(i18n("<qt><b>Set relative path</b>"
        "<p>Change the relative path for selected entity.</p></qt>"));
    actSetEntityRltPath->setGroup(VSPART_ACTION_TOOLS_GROUP);
    //###################################################################

    actCfgEntity = new KAction(i18n("Configure"), 0, this, SLOT(slotConfigureEntity()), actions, VSPART_ACTION_CONFIGURE_ENTITY);
    actCfgEntity->setToolTip(i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));
    actCfgEntity->setWhatsThis(i18n(VSPART_ACTION_CONFIGURE_ENTITY_WIT));
    actCfgEntity->setGroup(VSPART_ACTION_TOOLS_GROUP);

    actRenameEntity = new KAction(i18n("Rename"), 0, this, SLOT(slotRenameEntity()), actions, VSPART_ACTION_RENAME_ENTITY);
    actRenameEntity->setToolTip(i18n(VSPART_ACTION_RENAME_ENTITY_TIP));
    actRenameEntity->setWhatsThis(i18n(VSPART_ACTION_RENAME_ENTITY_WIT));
    actRenameEntity->setGroup(VSPART_ACTION_TOOLS_GROUP);

    /* connect(m_listView, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
            this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&))); */
    /* connect(m_listView, SIGNAL(itemRenamed(QListViewItem*, const QString&, int)),
            this, SLOT(slotEntityRenamed(QListViewItem*, const QString&, int))); */
    /* connect(m_listView, SIGNAL(selectionChanged(QListViewItem*)),
            this, SLOT(slotSelectItem(QListViewItem*))); */
  }

  VSExplorer::~VSExplorer() {
    if(explorer != 0) {
      delete explorer;
    }
  //   KConfig* config = m_part->instance()->config();
  //   config->setGroup("General");
  //   config->writeEntry( "ViewMode", viewMode() );
  //   config->writeEntry("FollowEditor", m_doFollowEditor);
  //   config->sync();
    delete actions;
    actions = 0;
  }

  void VSExplorer::slotSelectItem(QListViewItem *item) {
    /* uivse_p ent = static_cast<uivse_p>(item);

    switch(ent->getType()) {
      case vs_solution: {
        vss_p sln = static_cast<vss_p>(ent->getModel());
#ifdef DEBUG
        kddbg << g_msg_entselected.arg(type2String(sln->getType())).arg(sln->getName());
#endif
        m_part->selectSln(sln);
        break; }
      case vs_project: {
        vsp_p prj = static_cast<vsp_p>(ent->getModel());
#ifdef DEBUG
        kddbg << g_msg_entselected.arg(type2String(prj->getType())).arg(prj->getName());
#endif
        break; }
      case vs_filter: {
        vsf_p flt = static_cast<vsf_p>(ent->getModel());
#ifdef DEBUG
        kddbg << g_msg_entselected.arg(type2String(flt->getType())).arg(flt->getName());
#endif
        // ent->setOpen(!ent->isOpen());
        break; }
      case vs_file: {
        vsfl_p fl = static_cast<vsfl_p>(ent->getModel());
#ifdef DEBUG
        kddbg << g_msg_entselected.arg(type2String(fl->getType())).arg(fl->getName());
#endif
        m_part->partController()->editDocument(fl->getURL());
        break; }
      default: {
        break; }
    }

    // Upon selection of project|file|filter select parent solution
    switch(ent->getType()) {
      case vs_project:
      case vs_filter:
      case vs_file: {
        vss_p sln = getParentSln(ent->getModel());
        if(sln != 0) {
          if(m_part->getSelectedSln() != sln) {
            m_part->selectSln(sln);
          }
        }
        break; }
      default: {
        break; }
    } */
  }

  /* void VSExplorer::slotContextMenu(KListView lv, QListViewItem *item, const QPoint &p) {
    if(item != 0) {
      KPopupMenu menu(this);
      uivse_p i = static_cast<uivse_p>(item);
      switch(i->getType()) {
        case vs_solution: {
          vss_p sln = static_cast<vss_p>(i->getModel());
          menu.insertTitle(*i->pixmap(0), i18n("Solution: \"%1\"").arg(sln->getName()));
          menu.insertItem("Save", this, SLOT(slotSaveEntity()));
          menu.insertSeparator();
          menu.insertItem("Select config");
          actSetEntityRltPath->plug(&menu);
          actRenameEntity->plug(&menu);
          menu.insertItem("Set active", this, SLOT(slotActivateEntity()));
          menu.insertSeparator();
          actCfgEntity->plug(&menu);
          break; }
        case vs_project: {
          vsp_p prj = static_cast<vsp_p>(i->getModel());
          menu.insertTitle(*i->pixmap(0), i18n("Project: \"%1\"").arg(prj->getName()));
          menu.insertItem("Save", this, SLOT(slotSaveEntity()));
          menu.insertSeparator();
          actRenameEntity->plug(&menu);
          menu.insertSeparator();
          menu.insertItem("Set active", this, SLOT(slotActivateEntity()));
          menu.insertSeparator();
          actCfgEntity->plug(&menu);
          break; }
        case vs_filter: {
          vsf_p flt = static_cast<vsf_p>(i->getModel());
          menu.insertTitle(*i->pixmap(0), i18n("Filter: \"%1\"").arg(flt->getName()));
          actRenameEntity->plug(&menu);
          menu.insertSeparator();
          actCfgEntity->plug(&menu);
          break; }
        case vs_file: {
          vsfl_p fl = static_cast<vsfl_p>(i->getModel());
          menu.insertTitle(*i->pixmap(0), i18n("File: \"%1\"").arg(fl->getName()));
          menu.insertItem("Save", this, SLOT(slotSaveEntity()));
          menu.insertSeparator();
          actRenameEntity->plug(&menu);
          menu.insertSeparator();
          actCfgEntity->plug(&menu);
          break; }
        default: {
          kddbg << g_wrn_unsupportedtyp.arg(type2String(i->getType()));
          break; }
      }
      menu.exec(p);
    }
#ifdef DEBUG
    kddbg << "Warning! no item for context menu.\n";
#endif
  } */

  void VSExplorer::slotEntityRenamed(QListViewItem *item, const QString &name, int /*col*/) {
    /* if(item != 0) {
      uivse_p ent = static_cast<uivse_p>(item);
      switch(ent->getType()) {
        case vs_solution: {
          break; }
        case vs_project: {
          break; }
        case vs_filter: {
          break; }
        case vs_file: {
          break; }
        default: {
          break; }
      }
      // kddbg << "Entity is renamed to: " << name << endl;
    } else {
      kddbg << "Error! no item" << endl;
    } */
  }

  void VSExplorer::slotProjectOpened() {
  }

  void VSExplorer::slotProjectClosed() {
  }

  void VSExplorer::slotSetEntityRltPath() {
    // ((VSExplorerEntity*)item)->getModel()->setRelativePath("");
    /*
    kddbg << "\"" << ((VSExplorerEntity*)item)->getModel()->getName() <<
        "\" is set to \"" << ((VSExplorerEntity*)item)->getModel()->getRelativePath()
        << "\"\n";
    */
  }

  void VSExplorer::slotConfigureEntity() {
    /* kddbg << "slotConfigureEntity" << endl;
    QPtrList<QListViewItem> items = m_listView->selectedItems();
    for(items_ci it=items.begin(); it!=items.end(); ++it) {
      VSExplorerEntity* ent = (VSExplorerEntity*)(*it);
    } */
  }

  void VSExplorer::slotRenameEntity() {
    /* if(!m_listView->itemsRenameable()) {
      kddbg << "Items are not renamable" << endl;
      return;
    }
    kddbg << "slotRenameEntity" << endl;

    QPtrList<QListViewItem> items = m_listView->selectedItems();
    for(items_ci it=items.begin(); it!=items.end(); ++it) {
      uivse_p ent = static_cast<uivse_p>(*it);
      if(ent != 0) {
        kddbg << "Rename item: " << ent->text(0) << endl;
        m_listView->rename((*it), 0);
        //ent->setText(0, ent->getModel()->getName());
      }
    } */
  }

  void VSExplorer::slotActivateEntity() {
    /* uivse_p sel = static_cast<uivse_p>(m_listView->selectedItem());
    if(sel != 0) {
      switch(sel->getType()) {
        case vs_solution: {
          // uivss_p prevsln = static_cast<uivss_p>(m_part->getActiveSln()->getUI());
          m_part->activateSln(static_cast<vss_p>(sel->getModel()));
          // m_listView->repaintItem(prevsln);
          // m_listView->repaintItem(sel);
          //NOTE: Maybe a bit expensive, needs thorough investigation
          m_listView->triggerUpdate();
          break; }
        case vs_project: {
          m_part->activatePrj(static_cast<vsp_p>(sel->getModel()));
          m_listView->triggerUpdate();
          break; }
        default: {
          kddbg << g_wrn_unsupportedtyp.arg(type2String(sel->getType()));
          break; }
      }
    } */
  }

  void VSExplorer::slotHighlightContextMenuItem(int /*id*/) {
  }

  void VSExplorer::slotSaveEntity() {
    /* uivse_p sel = static_cast<uivse_p>(m_listView->selectedItem());
    switch(sel->getType()) {
      case vs_solution: {
        m_part->saveSln(static_cast<vss_p>(sel->getModel()));
        break; }
      case vs_project: {
        break; }
      case vs_file: {
        break; }
      default: {
        kddbg << g_wrn_unsupportedtyp.arg(type2String(sel->getType())).arg("VSExplorer::slotSaveEntity");
        break; }
    } */
  }

  void VSExplorer::slotSaveEntityAs() {
  }

  void VSExplorer::slotRefreshUI() {
    BOOSTVEC_FOR(uivse_ci, it, uients) {
      if((*it) != 0) {
        (*it)->slotRefreshText();
      }
    }
  }

  uivss_p VSExplorer::addSolutionNode(vss_p sln) {
    uivss_p item = new VSSlnNode(sln, explorer->viewport());
    if(item != 0) {
      BOOSTVEC_PUSHBACK(uients, item);
      explorer->insertItem(item, 0);
      return item;
    }
    else { kddbg << g_err_notenoughmem.arg("uivss"); }
    return 0;
  }

  uivsp_p VSExplorer::addProjectNode(uivse_p pnt, vsp_p prj) {
    switch(pnt->getType()) {
      case vs_filter:
      case vs_solution: {
        uivsp_p item = new VSPrjNode(prj, explorer->viewport());
        if(item != 0) {
          BOOSTVEC_PUSHBACK(uients, item);
          explorer->insertItem(item, static_cast<lwi_p>(pnt));
          return item;
        }
        else { kddbg << g_err_notenoughmem.arg("uivsp"); }
        return 0; }
      default: {
        return 0; }
    }
  }

  uivsf_p VSExplorer::addFilterNode(uivse_p pnt, vsf_p flt) {
    switch(pnt->getType()) {
      case vs_solution:
      case vs_project:
      case vs_filter: {
        uivsf_p item = new VSFltNode(flt, explorer->viewport());
        if(item != 0) {
          BOOSTVEC_PUSHBACK(uients, item);
          explorer->insertItem(item, static_cast<lwi_p>(pnt));
          return item;
        }
        else { kddbg << g_err_notenoughmem.arg("uivsf"); }
        return 0; }
      default: {
        return 0; }
    }
  }

  uivsfl_p VSExplorer::addFileNode(uivse_p pnt, vsfl_p fl) {
    switch(pnt->getType()) {
      case vs_project:
      case vs_filter: {
        uivsfl_p item = new VSFilNode(fl, explorer->viewport());
        if(item != 0) {
          BOOSTVEC_PUSHBACK(uients, item);
          explorer->insertItem(item, static_cast<lwi_p>(pnt));
          return item;
        }
        else { kddbg << g_err_notenoughmem.arg("uivsfl"); }
        return 0; }
      default: {
        return 0; }
    }
  }

  uivse_p VSExplorer::getByUID(const QUuid &uid) const {
    BOOSTVEC_FOR(uivse_ci, it, uients) {
      uivse_p ent = static_cast<uivse_p>(*it);
      if(ent != 0) {
        if(ent->getUID() == uid) { return (*it); }
      }
      else {
        kddbg << g_err_list_corrupted.arg("UI entity").arg("VSExplorer::getByUID");
        return 0;
      }
    }
    kddbg << "Can't find ui entity: " << guid2String(uid) << endl;
    return 0;
  }

  void VSExplorer::emitAddedFiles(const QStringList &list) {
    // emit m_part->addedFilesToProject(list);
  }

  //===========================================================================
  // VStudio SetPathWidget methods
  //===========================================================================
  SetPathWidget::SetPathWidget(QWidget *parent, const char *name, WFlags fl)
  : QWidget(parent, name, fl) {
    if(!name) setName(VSPART_SETPATH_WIDGET_NAME);
    layout = new QGridLayout(this, 1, 1, 1, 0, "SetPathWidgetLayout");
    btn_change = new QPushButton(this, "btn_change");
    layout->addWidget(btn_change, 0, 0);
    languageChange();
    resize(QSize(253, 670).expandedTo(minimumSizeHint()));
    clearWState(WState_Polished);
  }

  SetPathWidget::~SetPathWidget() {
    // no need to delete child widgets, Qt does it all for us
  }

  void SetPathWidget::languageChange() {
    setCaption(tr2i18n("Form2"));
  }

  void SetPathWidget::widgetDestroyed(QObject*) {
    qWarning("SetPathWidget::widgetDestroyed(QObject*): Not implemented yet");
  }

  //===========================================================================
  // VStudio explorer widget entity solution class methods
  //===========================================================================
  VSSlnNode::VSSlnNode(vss_p s, QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : VSExplorerEntity(pnt, nm, fl)
  , sln(s) {
    lbl_icon = new QLabel(hb_main, "lbl_icon");
    lbl_icon->setPixmap(SmallIcon("gohome"));
    lbl_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    lbl_name = new QLabel(hb_main, "lbl_name");
    lbl_name->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    btn_cfg = new QPushButton(hb_main, "btn_cfg");
    btn_cfg->setText(i18n("configuration"));
    btn_cfg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    btn_cfg->setMaximumSize(QSize(100, 16));
    btn_cfg->setMinimumSize(QSize(50, 16));
    QToolTip::add(btn_cfg, i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));

    hb_main->setStretchFactor(lbl_icon, 0);
    hb_main->setStretchFactor(lbl_name, 1);
    hb_main->setStretchFactor(btn_cfg, 0);

    slotRefreshText();
  }

  VSSlnNode::~VSSlnNode() {
  }

  vsinline vse_p VSSlnNode::getModel() const vsinline_attrib { return sln; }
  vsinline const QUuid& VSSlnNode::getUID() const vsinline_attrib { return uid_null; }
  vsinline e_VSEntityType VSSlnNode::getType() const vsinline_attrib { return sln->getType(); }

  void VSSlnNode::setState(const QString &/*state*/) {
  }

  void VSSlnNode::slotRefreshText() {
    lbl_name->setText(sln->getName());

    if(!sln->isReachable()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      btn_cfg->setText(i18n("unreachable"));
      return;
    }

    if(!sln->isLoaded()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      btn_cfg->setText(i18n("load error"));
      return;
    }

    // See if solution in "detached" state (i.e. no config selected for build)
    if(sln->isDetached()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      btn_cfg->setText(i18n("detached"));
    }
    else {
      vcfg_cp cfg = sln->currentCfg();
      // The count of projects
      lbl_icon->setPixmap(SmallIcon("gohome"));
      btn_cfg->setText(QString((cfg != 0) ? cfg->toString() : "error"));
    }
  }

  //===========================================================================
  // VStudio explorer widget entity project class methods
  //===========================================================================
  VSPrjNode::VSPrjNode(vsp_p p, QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : VSExplorerEntity(pnt, nm, fl)
  , prj(p) {
    lbl_icon = new QLabel(hb_main, "lbl_icon");
    lbl_icon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lbl_icon->setPixmap(SmallIcon("tar"));
    lbl_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl_name = new QLabel(hb_main, "lbl_name");
    lbl_name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    btn_cfg = new QPushButton(hb_main, "btn_cfg");
    btn_cfg->setText(i18n("configuration"));
    btn_cfg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    btn_cfg->setMaximumSize(QSize(100, 16));
    btn_cfg->setMinimumSize(QSize(50, 16));
    QToolTip::add(btn_cfg, i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));

    hb_main->setStretchFactor(lbl_icon, 0);
    hb_main->setStretchFactor(lbl_name, 1);
    hb_main->setStretchFactor(btn_cfg, 0);

    slotRefreshText();
  }

  VSPrjNode::~VSPrjNode() {
  }

  vsinline vse_p VSPrjNode::getModel() const vsinline_attrib { return prj; }
  vsinline const QUuid& VSPrjNode::getUID() const vsinline_attrib { return prj->getUID(); }
  vsinline e_VSEntityType VSPrjNode::getType() const vsinline_attrib { return prj->getType(); }

  void VSPrjNode::slotRefreshText() {
    lbl_name->setText(prj->getName());
    if(!static_cast<vsfs_p>(prj)->isReachable()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      btn_cfg->setText(i18n("unreachable"));
      return;
    }

    if(!static_cast<vsfs_p>(prj)->isLoaded()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      btn_cfg->setText(i18n("load error"));
      return;
    }

    if(prj->isDetached()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      btn_cfg->setText(i18n("detached"));
      return;
    }

    vcfg_cp cfg = prj->currentCfg();
    lbl_icon->setPixmap(SmallIcon("tar"));
    btn_cfg->setText(QString((cfg != 0) ? cfg->toString() : "error"));
  }

  //===========================================================================
  // VStudio explorer widget entity filter class methods
  //===========================================================================
  VSFltNode::VSFltNode(vsf_p filter, QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : VSExplorerEntity(pnt, nm, fl)
  , flt(filter)
  , container(0) {
    lbl_icon = new QLabel(hb_main, "lbl_icon");
    lbl_icon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lbl_icon->setPixmap(SmallIcon("folder"));
    lbl_name = new QLabel(hb_main, "lbl_name");
    lbl_name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    slotRefreshText();
  }

  VSFltNode::~VSFltNode() {
  }

  vsinline vse_p VSFltNode::getModel() const vsinline_attrib { return flt; }
  vsinline const QUuid& VSFltNode::getUID() const vsinline_attrib { return flt->getUID(); }
  vsinline e_VSEntityType VSFltNode::getType() const vsinline_attrib { return flt->getType(); }
  vsinline uivse_p VSFltNode::getUIContainer() const vsinline_attrib { return container; }

  void VSFltNode::slotRefreshText() {
    lbl_icon->setPixmap(SmallIcon("folder"));
    lbl_name->setText(flt->getName());
  }

  //===========================================================================
  // VStudio explorer widget entity file class methods
  //===========================================================================
  VSFilNode::VSFilNode(vsfl_p fil, QWidget *pnt, const char *nm/*=0*/, WFlags fl/*=0*/)
  : VSExplorerEntity(pnt, nm, fl)
  , file(fil)
  , container(0) {
    lbl_icon = new QLabel(hb_main, "lbl_icon");
    lbl_icon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lbl_icon->setPixmap(SmallIcon("folder"));
    lbl_name = new QLabel(hb_main, "lbl_name");
    lbl_name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    btn_cfg = new QPushButton(hb_main, "btn_cfg");
    btn_cfg->setText(i18n("configuration"));
    btn_cfg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    btn_cfg->setMaximumSize(QSize(100, 16));
    btn_cfg->setMinimumSize(QSize(50, 16));
    QToolTip::add(btn_cfg, i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));

    hb_main->setStretchFactor(lbl_icon, 0);
    hb_main->setStretchFactor(lbl_name, 1);
    hb_main->setStretchFactor(btn_cfg, 0);

    slotRefreshText();
  }

  VSFilNode::~VSFilNode() {
  }

  vsinline vse_p VSFilNode::getModel() const vsinline_attrib { return file; }
  vsinline const QUuid& VSFilNode::getUID() const vsinline_attrib { return uid_null; }
  vsinline e_VSEntityType VSFilNode::getType() const vsinline_attrib { return file->getType(); }
  vsinline uivse_p VSFilNode::getUIContainer() const vsinline_attrib { return container; }

  /*inline*/ void VSFilNode::setState(const QString &st) {
    if(st == "normal") {
      lbl_icon->setPixmap(SmallIcon("file"));
      lbl_name->setText(file->getName());
    }
  }

  void VSFilNode::slotRefreshText() {
    if(file->isReachable()) {
      lbl_icon->setPixmap(SmallIcon("file"));
      lbl_name->setText(file->getName());
    }
    else {
      lbl_icon->setPixmap(SmallIcon("error"));
      lbl_name->setText(QString(file->getName()).append("\n [%1]").arg("unreachable"));
    }
  }
};
#include "vs_explorer.moc"
