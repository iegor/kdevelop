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
  : QFrame(pnt, nm, fl)
  , pnt(0)
  , sbl(0)
  , chd(0)
  , maybeTotalHeight(-1)
  , lvl(0)
  , flags(0)
  , numChildren(0) {
  }

  ListWidgetItem::~ListWidgetItem() {
  }

  void ListWidgetItem::mousePressEvent(QMouseEvent *e) {
    if(hit_item(e->pos())) {
      set_bit(flags, IS_PRESSED);
      switch(e->button()) {
        case Qt::LeftButton: {
          set_bit(flags, IS_LMB_DOWN);
          break; }
        default: {
          break; }
      }
    }
  }

  void ListWidgetItem::mouseReleaseEvent(QMouseEvent *e) {
    if(hit_item(e->pos())) {
      switch(e->button()) {
        case Qt::LeftButton: {
          if(check_bit(flags, IS_PRESSED|IS_LMB_DOWN)) {
            clear_bit(flags, IS_LMB_DOWN);
            emit released();
            emit clicked();

            // Make list widget emit it's signals
            list->select(this);
          }
          break; }
        default: {
          break; }
      }
      clear_bit(flags, IS_PRESSED);
    }
  }

  void ListWidgetItem::mouseDoubleClickEvent(QMouseEvent *e) {
  }

  void ListWidgetItem::enterEvent(QEvent *e) {
    //setMicroFocusHint(x(), y(), width(), height(), false);
    list->setFocused(this); // Set this item as focused in list
    installEventFilter(list);
    list->viewport()->setFocusProxy(this);
    setFocus();
  }

  void ListWidgetItem::leaveEvent(QEvent *e) {
    removeEventFilter(list);
    list->viewport()->setFocusProxy(list);
    list->setFocus();
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

  lwi_p ListWidgetItem::parent() const { return pnt; }
  lwi_p ListWidgetItem::sibling() const { return sbl; }
  lwi_p ListWidgetItem::child() const { return chd; }
  void ListWidgetItem::setParent(lwi_cp p) { pnt = const_cast<lwi_p>(p); }
  void ListWidgetItem::setSibling(lwi_cp s) { sbl = const_cast<lwi_p>(s); }
  void ListWidgetItem::setChild(lwi_cp c) { chd = const_cast<lwi_p>(c); }
  int ListWidgetItem::level() const { return lvl; }
  void ListWidgetItem::setLevel(int level) { lvl = level; }
  bool ListWidgetItem::isRoot() const { return check_bit(flags, IS_ROOT); }
  bool ListWidgetItem::isExpanded() const { return check_bit(flags, IS_EXPANDED); }
  bool ListWidgetItem::canExpand() const { return (chd == 0) && (numChildren > 0); }

  void ListWidgetItem::expand() {
    set_bit(flags, IS_EXPANDED);
  }

  void ListWidgetItem::collapse() {
    clear_bit(flags, IS_EXPANDED);
  }

  void ListWidgetItem::addChild(lwi_p child) {
    if(child != 0) {
      child->setLevel(lvl + 1);

      // Set item's relations
      child->setSibling(chd);
      chd = child;
      numChildren++;
      chd->setParent(this);

      // Sort items if expanded
      //if(isExpanded()) {
        child->show();
        child->setEnabled(true);
      //}
    }
    else {
      kddbg << "lwi child item is 0.\n";
    }
  }

  void ListWidgetItem::sort() {
  }

  void ListWidgetItem::updateArrangement() {
    // Update position of every sibling starting frim this item
    // lwi_p itm = sbl;
    // while(itm != 0) {
    //  itm = itm->sbl;
    // }
    // Update position of every item starting from this item's parent
  }

  bool ListWidgetItem::hit_item(const QPoint &p) const {
    return rect().contains(p);
  }

  //===========================================================================
  // ListWidget methods
  //===========================================================================
  ListWidget::ListWidget(QWidget *p, const char *nm, WFlags fl)
  : QScrollView(p, nm, fl)
  //, lvl_shift(10)
  , focusItem(0)
  , selectedItem(0)
  , prevFocusItem(0)
  , prevSelectedItem(0) {
    setMouseTracking(true);
    viewport()->setMouseTracking(true);

    // Create root item
    root = new RootItem(this, "lwi_root");
    root->hide();
    // root->setEnabled(false);
    set_bit(root->flags, lwi::IS_ROOT);
    set_bit(root->flags, lwi::IS_EXPANDED); //NOTE: otherwise list::updateItems() will hide all children of root

    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(WheelFocus);
    viewport()->setBackgroundMode(PaletteBase);
    setBackgroundMode(PaletteBackground, PaletteBase);

    // List items mapper
    imapper = new Tree_LM(this);
  }

  ListWidget::~ListWidget() {
    /*BOOSTVEC_FOR(lwi_ci, it, items) {
      lwi_p itm(*it);
      // if(itm != 0) { delete itm; }
    }*/

    if(imapper != 0) { delete imapper; imapper=0; }
  }

  void ListWidget::paintEvent(QPaintEvent *e) {
  }

  void ListWidget::drawContents(QPainter *p, int cx, int cy, int cw, int ch) {
    if(!dqueue.empty()) {
      QPointArray ellipse;
      ellipse.makeEllipse(100, 100, 40, 30);
      p->setBrush(QBrush(QColor(35, 16, 49)));
      p->drawPolygon(ellipse);
    }
  }

  // int ListWidget::lvlShift() const { return imapper->lvl_shift; }
  // void ListWidget::setLvlShift(int shift) { lvl_shift = shift; }

  bool ListWidget::insertItem(lwi_p p, lwi_p pnt/*=0*/) {
    if(p != 0) {
      BOOSTVEC_PUSHBACK(items, p);
      addChild(p, 0, 0);
      p->list = this;

      p->invalidateHeight();
      p->totalHeight(); //HACK: force recalc of p->maybeTotalHeight

      if(pnt == 0) { pnt = root; }
      pnt->addChild(p);

      int tw=width();
      int th=0;
      // int max_lvl=0;

      BOOSTVEC_FOR(lwi_ci, it, items) {
        lwi_p item(*it);
        if(item != 0) { th += item->height(); }
      }

      resizeContents(tw, th);
      slotRemapItems();
      return true;
    }
    return false;
  }

  void ListWidget::slotRemapItems() {
    imapper->map();
  }

  void ListWidget::setFocused(lwi_p item) {
    prevFocusItem = focusItem;
    focusItem = item;
  }

  void ListWidget::select(lwi_p itm) {
    //Deselect previuosly selected item
    prevSelectedItem = selectedItem;

    selectedItem = itm;

    emit selectionChanged(itm);
  }

  ListWidget::RootItem::RootItem(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : ListWidgetItem (pnt, nm, fl) {
    lvl = -1;
  }

  ListWidget::RootItem::~RootItem() {
  }

  ListWidget::LayoutMapper::LayoutMapper(lw_p hst)
  : host(hst) {
  }

  ListWidget::LayoutMapper::~LayoutMapper() {
  }

  void ListWidget::LayoutMapper::map() {
  }

  ListWidget::Tree_LM::Tree_LM(lw_p hst)
  : ListWidget::LayoutMapper(hst) {
    lvl_shift = 10;
  }

  ListWidget::Tree_LM::~Tree_LM() {
  }

  void ListWidget::Tree_LM::map() {
    /* if(i == 0) { i = root->child(); }
    lwi_p itm = i;

    if(itm->parent()->isExpanded()) {
      itm->setEnabled(true);
      itm->show();

      int ht_cnt = 0;
      while(itm != 0) {
        moveChild(itm, lvl_shift*itm->lvl, ht_cnt);
        //itm->resize(200, 20);

        //NOTE: Dive to update item's children
        if(itm->canExpand() && itm->isExpanded()) { updateItems(itm); }

        ht_cnt += itm->totalHeight();
        itm = itm->sibling();
      }
    }
    else {
      itm->setEnabled(false);
      itm->hide();
    } */

    //NOTE: testing only
    //TODO: do some sorting, for all "trees" (solutions)
    int ht_cnt=0;
    BOOSTVEC_FOR(lwi_ci, it, host->items) {
      lwi_p item(*it);
      if(item != 0) {
        host->moveChild(item, lvl_shift * item->lvl, ht_cnt);
        // item->resize(200, 20);
        ht_cnt += item->height();
      }
    }
  }

  //===========================================================================
  // VStudio::VSExplorerListWidget methods
  //===========================================================================
  VSExplorerListWidget::VSExplorerListWidget(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : ListWidget(pnt, nm, fl) {
  }

  VSExplorerListWidget::~VSExplorerListWidget() {
  }

  bool VSExplorerListWidget::eventFilter(QObject *o, QEvent *e) {
    if(o == viewport()) {
      QFocusEvent *fe = static_cast<QFocusEvent*>(e);
      switch(e->type()) {
        case QEvent::FocusIn: {
          focusInEvent(fe);
          return true;
          break; }
        case QEvent::FocusOut: {
          focusOutEvent(fe);
          return true;
          break; }
        default: {
          break; }
      }
    }

    if(e->type() == QEvent::KeyPress) {
      QKeyEvent *ke(static_cast<QKeyEvent*>(e));
      switch(ke->key()) {
        case Qt::Key_Shift: {
          uivse_p item(static_cast<uivse_p>(focusItem));
          // Expand item's controls
          // item->showControls();
          return true; }
        default: { break; }
      }
    }
    else if(e->type() == QEvent::KeyRelease) {
      QKeyEvent *ke(static_cast<QKeyEvent*>(e));
      switch(ke->key()) {
        case Qt::Key_Shift: {
          uivse_p item(static_cast<uivse_p>(focusItem));
          // Show/hide item's controls
          if(!item->controlsVisible()) { item->showControls(); }
          else { item->hideControls(); }
          return true; }
        default: { break; }
      }
    }

    return QScrollView::eventFilter(o, e);
  }

  //===========================================================================
  // VStudio::VSExplorerEntity methods
  //===========================================================================
  VSExplorerEntity::VSExplorerEntity(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : ListWidgetItem(pnt, nm, fl)
  , enflg(0) {
    //setMinimumSize(QSize(200, 16));
    //setMaximumSize(QSize(500, 100));
    //setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFrameStyle(QFrame::Box|QFrame::Plain);
    setLineWidth(1);
    setMidLineWidth(1);
    // Main layout
    hbl_main = new QHBoxLayout(this, 1, 0, "hbl_main");
    layout()->setMargin(1);
    layout()->setSpacing(0);
    // Vertical main layout for side controls
    vb_ctrl = new QVBox(this, "vb_ctrl");
    vb_ctrl->setSpacing(0);
    vb_ctrl->setMargin(0);
    hbl_main->addWidget(vb_ctrl, 0, Qt::AlignLeft);
    // Vertical main layout spacer, to keep main controls up
    spc_vbctrl = new QSpacerItem(10, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vb_ctrl->layout()->addItem(spc_vbctrl);
    // Select checkbox
    chb_select = new QCheckBox(vb_ctrl, "chb_select");
    chb_select->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    chb_select->setMinimumSize(QSize(10,10));
    chb_select->setMaximumSize(QSize(16,16));
    chb_select->hide();
    chb_select->setEnabled(false);
    // Vertical element layout, used to keep entity name, icon, config, progress, errors, info etc
    vbl_elem = new QVBoxLayout(hbl_main, 0, "vbl_elem");
    vbl_elem->setMargin(0);
    vbl_elem->setSpacing(0);
    // vbl_elem->setResizeMode(QLayout::Minimum);
    // vbl_elem->setAlignment(Qt::AlignTop);
    // HBox to contain icon and name labels, common for all VS UI entities
    hb_top = new QHBox(this, "hbl_top");
    //hb_top->setMinimumHeight(16);
    //hb_top->setMaximumHeight(16);
    // hb_top->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    vbl_elem->addWidget(hb_top);
    vbl_elem->setStretchFactor(hb_top, 1);
    // Icon label
    lbl_icon = new QLabel(hb_top, "lbl_icon");
    lbl_icon->setMinimumSize(QSize(10,10));
    lbl_icon->setMaximumSize(QSize(16,16));
    // lbl_icon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    // Name label
    lbl_name = new QLabel(hb_top, "lbl_name");
    lbl_name->setMinimumSize(150, 10);
    lbl_name->setMaximumHeight(16);
    // lbl_name->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    hb_top->setStretchFactor(lbl_icon, 0);
    hb_top->setStretchFactor(lbl_name, 1);
  }

  VSExplorerEntity::~VSExplorerEntity () {
  }

  void VSExplorerEntity::enterEvent(QEvent *e) {
    ListWidgetItem::enterEvent(e);
    set_bit(enflg, IS_HOVERED_ON);
    // VSExplorerListWidget *explorer = static_cast<VSExplorerListWidget*>(list);
    setBackgroundMode(Qt::PaletteHighlight);
    // list->updateItems();
    chb_select->setEnabled(true);
    chb_select->show();

    invalidateHeight();

    // vb_ctrl->adjustSize();
    // vb_ctrl->updateGeometry();
    // hb_top->adjustSize();
    // hb_top->updateGeometry();

    // lwi_p chi = chd;
    // while(chi != 0) {
    //   chi->setBackgroundMode(Qt::PaletteHighlight);
    //   chi = chi->sibling();
    // }
  }

  void VSExplorerEntity::leaveEvent(QEvent *e) {
    ListWidgetItem::leaveEvent(e);
    clear_bit(enflg, IS_HOVERED_ON);
    // VSExplorerListWidget *explorer = static_cast<VSExplorerListWidget*>(list);
    setBackgroundMode(Qt::PaletteBackground);
    // list->updateItems();
    chb_select->setEnabled(false);
    chb_select->hide();

    invalidateHeight();

    // vb_ctrl->adjustSize();
    // vb_ctrl->updateGeometry();
    // hb_top->adjustSize();
    // hb_top->updateGeometry();

    // lwi_p chi = chd;
    // while(chi != 0) {
    //   chi->setBackgroundMode(Qt::PaletteBackground);
    //   chi = chi->sibling();
    // }
  }

  void VSExplorerEntity::invalidateHeight() {
    ListWidgetItem::invalidateHeight();
    //if(parent && parent->isOpen()) { parent->invalidateHeight(); }
  }

  void VSExplorerEntity::expand() {
    ListWidgetItem::expand();
  }

  void VSExplorerEntity::collapse() {
    ListWidgetItem::collapse();
  }

  void VSExplorerEntity::addChild(lwi_p child) {
    ListWidgetItem::addChild(child);

    /* if(canExpand()) {
      btn_expand->setEnabled(true);
      btn_expand->show();
    } */
  }

  bool VSExplorerEntity::hit_item(const QPoint &p) const {
    return hb_top->rect().contains(p);
  }

  void VSExplorerEntity::showControls() {
    set_bit(enflg, CONTROLS_VISIBLE);
  }

  void VSExplorerEntity::hideControls() {
    clear_bit(enflg, CONTROLS_VISIBLE);
  }

  bool VSExplorerEntity::controlsVisible() const { return check_bit(enflg, CONTROLS_VISIBLE); }
  bool VSExplorerEntity::isSelected() const { return chb_select->isChecked(); }

  void VSExplorerEntity::slotToggleExpand(bool on) { if(on) { expand(); } else { collapse(); } }

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
    // m_actionFollowEditor = new KToggleAction(i18n("Follow Editor"), KShortcut(), this, SLOT(slotFollowEditor()),
    //    m_part->actionCollection(), "classview_follow_editor");

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
    actSetEntityRltPath = new KAction(i18n("Set relative path:"), 0, this, SLOT(slotSetEntityRltPath()),
                                      actions, "setrelpath");
    actSetEntityRltPath->setToolTip(i18n("Set relative path for entity"));
    actSetEntityRltPath->setWhatsThis(i18n("<qt><b>Set relative path</b>"
        "<p>Change the relative path for selected entity.</p></qt>"));
    actSetEntityRltPath->setGroup(VSPART_ACTION_TOOLS_GROUP);
    //###################################################################

    actCfgEntity = new KAction(i18n("Configure"), 0, this, SLOT(slotConfigureEntity()), actions,
                               VSPART_ACTION_CONFIGURE_ENTITY);
    actCfgEntity->setToolTip(i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));
    actCfgEntity->setWhatsThis(i18n(VSPART_ACTION_CONFIGURE_ENTITY_WIT));
    actCfgEntity->setGroup(VSPART_ACTION_TOOLS_GROUP);

    actRenameEntity = new KAction(i18n("Rename"), 0, this, SLOT(slotRenameEntity()), actions,
                                  VSPART_ACTION_RENAME_ENTITY);
    actRenameEntity->setToolTip(i18n(VSPART_ACTION_RENAME_ENTITY_TIP));
    actRenameEntity->setWhatsThis(i18n(VSPART_ACTION_RENAME_ENTITY_WIT));
    actRenameEntity->setGroup(VSPART_ACTION_TOOLS_GROUP);

    /* connect(m_listView, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
            this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&))); */
    /* connect(m_listView, SIGNAL(itemRenamed(QListViewItem*, const QString&, int)),
            this, SLOT(slotEntityRenamed(QListViewItem*, const QString&, int))); */
    connect(explorer, SIGNAL(selectionChanged(lwi_p)), this, SLOT(slotSelectItem(lwi_p)));
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

  void VSExplorer::slotSelectItem(lwi_p itm) {
    uivse_p ent = static_cast<uivse_p>(itm);

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
    }
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
    // vb_ctrl->layout()->remove(chb_select);
    // vb_ctrl->layout()->removeItem(spc_vbctrl);
    // Expand button
    btn_expand = new QPushButton(vb_ctrl, "btn_expand");
    btn_expand->setToggleButton(true);
    btn_expand->setText("+");
    btn_expand->setMinimumSize(QSize(10,10));
    btn_expand->setMaximumSize(QSize(16,16));
    btn_expand->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    //btn_expand->setEnabled(false); btn_expand->hide();
    // vb_ctrl->layout()->add(chb_select);
    // vb_ctrl->layout()->addItem(spc_vbctrl);
    // Configure btn
    btn_cfg = new QPushButton(this, "btn_cfg");
    btn_cfg->setPixmap(SmallIcon("package_utilities"));
    btn_cfg->setText("< configuration >");
    btn_cfg->setMaximumSize(QSize(50, 10));
    btn_cfg->setMinimumSize(QSize(100, 16));
    btn_cfg->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_cfg, i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));
    vbl_elem->addWidget(btn_cfg);
    btn_cfg->setEnabled(false); btn_cfg->hide();
    // Tools layout
    hb_tools = new QHBox(this, "hbl_tools");
    QHBoxLayout *hbl_tools = new QHBoxLayout(hb_tools, 1, 0, "hbl_tools");
    QSpacerItem *spc_tools = new QSpacerItem(2, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hbl_tools->addItem(spc_tools);
    vbl_elem->addWidget(hb_tools);
    // Build btn
    btn_bld = new QPushButton(this, "btn_bld");
    btn_bld->setPixmap(SmallIcon("build"));
    btn_bld->setMinimumSize(QSize(10,10));
    btn_bld->setMaximumSize(QSize(16,16));
    btn_bld->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_bld, i18n(VSPART_ACTION_BUILD_SOLUTION_TIP));
    //hbl_tools->addWidget(btn_bld);
    btn_bld->setEnabled(false); btn_bld->hide();
    // Clear btn
    btn_clr = new QPushButton(this, "btn_clr");
    btn_clr->setPixmap(SmallIcon("eraser"));
    btn_clr->setMinimumSize(QSize(10,10));
    btn_clr->setMaximumSize(QSize(16,16));
    btn_clr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_clr, i18n(VSPART_ACTION_CLEAN_SOLUTION_TIP));
    //hbl_tools->addWidget(btn_clr);
    btn_clr->setEnabled(false); btn_clr->hide();

    // hbl_tools->setStretchFactor(lbl_cfg, 1);
    hbl_tools->setStretchFactor(btn_cfg, 1);
    hbl_tools->setStretchFactor(btn_bld, 0);
    hbl_tools->setStretchFactor(btn_clr, 0);

    connect(btn_expand, SIGNAL(toggled(bool)), this, SLOT(slotToggleExpand(bool)));

    hb_tools->hide();
    hb_tools->setEnabled(false);

    slotRefreshText();
    // hideControls();
  }

  VSSlnNode::~VSSlnNode() {
  }

  void VSSlnNode::enterEvent(QEvent *e) {
    VSExplorerEntity::enterEvent(e);
    //lbl_icon->setBackgroundMode(Qt::PaletteHighlight);
    //hb_tools->setBackgroundMode(Qt::PaletteHighlight);

    btn_cfg->setEnabled(true);
    btn_cfg->show();

    adjustSize();
    // updateArrangement();
    list->slotRemapItems();
  }

  void VSSlnNode::leaveEvent(QEvent *e) {
    VSExplorerEntity::leaveEvent(e);
    //lbl_icon->setBackgroundMode(Qt::PaletteBackground);

    if(controlsVisible()) { hideControls(); }

    btn_cfg->setEnabled(false);
    btn_cfg->hide();

    adjustSize();
    // updateArrangement();
    list->slotRemapItems();
  }

  void VSSlnNode::expand() {
    VSExplorerEntity::expand();

    lwi_p itm = chd;
    while(itm != 0) {
      itm->show();
      itm = itm->sibling();
    }

    btn_expand->setText("-");
  }

  void VSSlnNode::collapse() {
    VSExplorerEntity::collapse();

    lwi_p itm = chd;
    while(itm != 0) {
      itm->hide();
      itm = itm->sibling();
    }

    btn_expand->setText("+");
  }

  vse_p VSSlnNode::getModel() const { return sln; }
  const QUuid& VSSlnNode::getUID() const { return uid_null; }
  e_VSEntityType VSSlnNode::getType() const { return sln->getType(); }

  void VSSlnNode::showControls() {
    VSExplorerEntity::showControls();
    // hb_tools->setEnabled(true);
    // hb_tools->show();

    vb_ctrl->layout()->add(btn_bld);
    btn_bld->setEnabled(true); btn_bld->show();
    btn_clr->setEnabled(true); btn_clr->show();
    vb_ctrl->layout()->add(btn_clr);
    //vb_ctrl->layout()->invalidate();
    //vb_ctrl->adjustSize();

    adjustSize();
    //updateGeometry();
    updateArrangement();

    list->slotRemapItems();
  }

  void VSSlnNode::hideControls() {
    VSExplorerEntity::hideControls();
    // hb_tools->setEnabled(false);
    // hb_tools->hide();

    vb_ctrl->layout()->remove(btn_bld);
    vb_ctrl->layout()->remove(btn_clr);
    btn_bld->setEnabled(false); btn_bld->hide();
    btn_clr->setEnabled(false); btn_clr->hide();
    //vb_ctrl->layout()->invalidate();
    //vb_ctrl->adjustSize();

    adjustSize();
    //updateGeometry();
    updateArrangement();

    list->slotRemapItems();
  }

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
    // Expand button
    btn_expand = new QPushButton(vb_ctrl, "btn_expand");
    btn_expand->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn_expand->setToggleButton(true);
    // btn_expand->setToggleType(QButton::Toggle);
    btn_expand->setMinimumSize(QSize(16,16));
    btn_expand->setMaximumSize(QSize(16,16));
    btn_expand->setText("+");
    //btn_expand->hide();
    //btn_expand->setEnabled(false);

    connect(btn_expand, SIGNAL(toggled(bool)), this, SLOT(slotToggleExpand(bool)));

    slotRefreshText();
  }

  VSPrjNode::~VSPrjNode() {
  }

  vse_p VSPrjNode::getModel() const { return prj; }
  const QUuid& VSPrjNode::getUID() const { return prj->getUID(); }
  e_VSEntityType VSPrjNode::getType() const { return prj->getType(); }

  void VSPrjNode::slotRefreshText() {
    lbl_name->setText(prj->getName());
    if(!static_cast<vsfs_p>(prj)->isReachable()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      //btn_cfg->setText(i18n("unreachable"));
      return;
    }

    if(!static_cast<vsfs_p>(prj)->isLoaded()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      //btn_cfg->setText(i18n("load error"));
      return;
    }

    if(prj->isDetached()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      //btn_cfg->setText(i18n("detached"));
      return;
    }

    vcfg_cp cfg = prj->currentCfg();
    lbl_icon->setPixmap(SmallIcon("tar"));
    //btn_cfg->setText(QString((cfg != 0) ? cfg->toString() : "error"));
  }

  //===========================================================================
  // VStudio explorer widget entity filter class methods
  //===========================================================================
  VSFltNode::VSFltNode(vsf_p filter, QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : VSExplorerEntity(pnt, nm, fl)
  , flt(filter)
  , container(0) {
    // Expand button
    btn_expand = new QPushButton(vb_ctrl, "btn_expand");
    btn_expand->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn_expand->setToggleButton(true);
    // btn_expand->setToggleType(QButton::Toggle);
    btn_expand->setMinimumSize(QSize(16,16));
    btn_expand->setMaximumSize(QSize(16,16));
    btn_expand->setText("+");
    //btn_expand->hide();
    //btn_expand->setEnabled(false);

    connect(btn_expand, SIGNAL(toggled(bool)), this, SLOT(slotToggleExpand(bool)));

    slotRefreshText();
  }

  VSFltNode::~VSFltNode() {
  }

  vse_p VSFltNode::getModel() const { return flt; }
  const QUuid& VSFltNode::getUID() const { return flt->getUID(); }
  e_VSEntityType VSFltNode::getType() const { return flt->getType(); }
  uivse_p VSFltNode::getUIContainer() const { return container; }

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
    slotRefreshText();
  }

  VSFilNode::~VSFilNode() {
  }

  vse_p VSFilNode::getModel() const { return file; }
  const QUuid& VSFilNode::getUID() const { return uid_null; }
  e_VSEntityType VSFilNode::getType() const { return file->getType(); }
  uivse_p VSFilNode::getUIContainer() const { return container; }

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
