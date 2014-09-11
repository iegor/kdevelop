/*  C/C++ Implementation: vs
*
* Description:
*  ! Parts were inherited from autoprojectpart src
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/

/* Qt */
#include <qobjectlist.h>
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
    QWidget::enterEvent(e);
    //setMicroFocusHint(x(), y(), width(), height(), false);
    list->setFocused(this); // Set this item as focused in list
  }

  void ListWidgetItem::leaveEvent(QEvent *e) {
    QWidget::leaveEvent(e);
  }

  int ListWidgetItem::totalHeight() {
    if(!isVisible()) { return 0; }
    if(maybeTotalHeight >= 0) { return maybeTotalHeight; }

    maybeTotalHeight = height();

    if(isExpanded()) {
      lwi_p chi = chd;
      while(chi != 0) {
        maybeTotalHeight += chi->totalHeight();
        chi = chi->sbl;
      }
    }

    return maybeTotalHeight;
  }

  void ListWidgetItem::invalidateHeight() {
    if(maybeTotalHeight < 0) { return; }
    maybeTotalHeight = -1;
    if(pnt != 0 && pnt->isExpanded()) {
      pnt->invalidateHeight();
    }
  }

  bool ListWidgetItem::isExpandable() const { return check_bit(flags, IS_EXPANDABLE); }
  void ListWidgetItem::setExpandable(bool is_expandable) {
    if(is_expandable) { set_bit(flags, IS_EXPANDABLE); } else { clear_bit(flags, IS_EXPANDABLE); }
  }

  /*void ListWidgetItem::remap_siblings() {
    int vshift=0;
    const QRect &prc = geometry();
    adjustSize();
    // First move siblings
    lwi_p i = sbl;
    while(i != 0) {
      if(isVisible()) {
        int pnt_bottom = list->childY(i->pnt) + prc.height();
        list->moveChild(i, list->lvlShift()*i->lvl, pnt_bottom + vshift);
        if(i->isExpanded()) {
          i->chd->remap_siblings();
        }
      }
      vshift += i->totalHeight();
      i = i->sbl;
    }
  }*/

  /*void ListWidgetItem::update_children() {
    lwi_p i = chd;
    while(i != 0) {
      if(i->pnt->isExpanded()) {
        i->__attachToList();
        if(i->canExpand()) { i->update_children(); }
      }
      else {
        i->__detachFromList();
        if(i->canExpand()) { i->update_children(); }
      }
      i = i->sbl;
    }
  }*/

  void ListWidgetItem::__expand_show() {
    lwi_p i = chd;
    while(i != 0) {
      i->__attachToList();
      i->__expand_show();
      i = i->sbl;
    }
  }

  void ListWidgetItem::__collapse_hide() {
    lwi_p i = chd;
    while(i != 0) {
      i->__detachFromList();
      i->__collapse_hide();
      i = i->sbl;
    }
  }

  void ListWidgetItem::expand() {
    if(canExpand() && !isExpanded()) {
      //const QRect &prc = pnt->geometry();
      //invalidateHeight();
      set_bit(flags, IS_EXPANDED);
      list->slotUpdateContents();
    }
  }

  void ListWidgetItem::collapse() {
    if(canExpand() && isExpanded()) {
      //invalidateHeight();
      clear_bit(flags, IS_EXPANDED);
      list->slotUpdateContents();
    }
  }

  void ListWidgetItem::addChild(lwi_p child) {
    if(child != 0) {
      child->setLevel(lvl + 1);

      // Set item's relations
      child->sbl = chd;
      chd = child;
      ++numChildren;
      chd->pnt = this;
      setExpandable(true);
    }
    else {
      kddbg << "lwi child item is 0.\n";
    }
  }

  void ListWidgetItem::sort_children() {
    int vshift=0;
    const QRect &prc = geometry();

    invalidateHeight();
    adjustSize();

    lwi_p i = chd;
    while(i != 0) {
      const QRect &irc = i->geometry();

      // Handle expansion/collapsing of children items
      if(isExpanded() && isVisible()) {
        int pnt_bottom = list->childY(i->pnt) + prc.height();
        list->moveChild(i, list->lvlShift()*i->lvl, pnt_bottom + vshift);
        i->__attachToList();
        //i->__expand_show();
      }
      else {
        //i->__collapse_hide();
        i->__detachFromList();
      }

      i->sort_children();

      vshift += i->totalHeight();
      i = i->sbl;
    }
  }

  bool ListWidgetItem::hit_item(const QPoint &p) const {
    return rect().contains(p);
  }

  //===========================================================================
  // ListWidget methods
  //===========================================================================
  ListWidget::ListWidget(QWidget *p, const char *nm, WFlags fl)
  : QScrollView(p, nm, fl)
  , lvl_shift(10)
  , focusItem(0)
  , selectedItem(0)
  , prevFocusItem(0)
  , prevSelectedItem(0) {
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    enableClipper(true);

    // Create root item
    root = new RootItem(viewport(), "lwi_root");
    addChild(root);
    //root->hide();
    root->list = this;
    // root->setEnabled(false);
    set_bit(root->flags, lwi::IS_ROOT);
    set_bit(root->flags, lwi::IS_EXPANDED); //NOTE: otherwise list::updateItems() will hide all children of root

    viewport()->setFocusProxy(this);
    viewport()->setFocusPolicy(WheelFocus);
    viewport()->setBackgroundMode(Qt::PaletteMid); //PaletteBase
    //setBackgroundMode(PaletteBackground, PaletteBase);
  }

  ListWidget::~ListWidget() {
    //BOOSTVEC_FOR(lwi_ci, it, items) {
    //  lwi_p itm(*it);
    //  // if(itm != 0) { delete itm; }
    //}
  }

  void ListWidget::paintEvent(QPaintEvent *e) {
  }

  //void ListWidget::drawContents(QPainter *p, int cx, int cy, int cw, int ch) {
    //if(!dqueue.empty()) {
      //QPointArray ellipse;
      //p->save();
      //ellipse.makeEllipse(20, 30, 40, 30);
      //p->setBrush(QBrush(QColor(35, 16, 49)));
      //p->drawPolygon(ellipse);
      //p->drawRect(geometry());
      //p->restore();
    //}
  //}

  // int ListWidget::lvlShift() const { return imapper->lvl_shift; }
  // void ListWidget::setLvlShift(int shift) { lvl_shift = shift; }

  bool ListWidget::insertItem(lwi_p p, lwi_p pnt/*=0*/) {
    if(p != 0) {
      BOOSTVEC_PUSHBACK(items, p);
      addChild(p);
      p->list = this;

      if(pnt == 0) { pnt = root; }
      pnt->addChild(p);

      p->adjustSize();
      p->updateGeometry();
      return true;
    }
    return false;
  }

  void ListWidget::slotUpdateContents() {
    int tw=width();
    int th=0;
    int max_lvl=0;
    root->sort_children();
    th=root->totalHeight();
    verticalScrollBar()->raise();
    //viewport()->resize(tw, th);
    clipper()->resize(tw, th);
    resizeContents(tw, th);
  }

  //void ListWidget::slotMapChildren(lwi_p i) {
  //  imapper->map_children(i);
  //}

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

  void ListWidget::recalcContents() {
    int tw=width();
    int th=0;
    lwi_p itm = root->chd;
    while(itm != 0) {
      //th +=
      itm = itm->sbl;
    }
  }

  //===========================================================================
  // ListWidget::RootItem methods
  //===========================================================================
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

  bool VSExplorerListWidget::eventFilter(QObject *o, QEvent *e) {
    if(o == viewport()) {
      QFocusEvent *fe = static_cast<QFocusEvent*>(e);
      switch(e->type()) {
        case QEvent::FocusIn: {
          focusInEvent(fe);
          return true; }
        case QEvent::FocusOut: {
          focusOutEvent(fe);
          return true; }
        default: {
          break; }
      }
    }
    else {
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
    }

    return QScrollView::eventFilter(o, e);
  }

  //===========================================================================
  // VStudio::VSExplorerEntity methods
  //===========================================================================
  VSExplorerEntity::VSExplorerEntity(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : ListWidgetItem(pnt, nm, fl)
  , enflg(0)
  , btn_expand(0) {
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
    vb_ctrl->setMinimumSize(QSize(0,0));
    vb_ctrl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    vb_ctrl->setSpacing(0);
    vb_ctrl->setMargin(0);
    hbl_main->addWidget(vb_ctrl, 0, Qt::AlignLeft);
    // Vertical main layout spacer, to keep main controls up
    spc_vbctrl = new QSpacerItem(VSPART_UIBTN_MINW, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vb_ctrl->layout()->addItem(spc_vbctrl);
    vb_ctrl->hide();
    // Vertical element layout, used to keep entity name, icon, config, progress, errors, info etc
    vbl_elem = new QVBoxLayout(hbl_main, 0, "vbl_elem");
    vbl_elem->setMargin(0);
    vbl_elem->setSpacing(0);
    // HBox to contain icon and name labels, common for all VS UI entities
    hb_top = new QHBox(this, "hbl_top");
    vbl_elem->addWidget(hb_top);
    // Select checkbox
    chb_select = new QCheckBox(hb_top, "chb_select");
    chb_select->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    chb_select->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    chb_select->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    chb_select->hide(); chb_select->setEnabled(false);
    // Icon label
    lbl_icon = new QLabel(hb_top, "lbl_icon");
    lbl_icon->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    lbl_icon->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    // lbl_icon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    // Name label
    lbl_name = new QLabel(hb_top, "lbl_name");
    lbl_name->setMinimumSize(150, VSPART_UIBTN_MINH);
    lbl_name->setMaximumHeight(VSPART_UIBTN_MAXH);
    // lbl_name->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    vbl_elem->setStretchFactor(hb_top, 1);
    hb_top->setStretchFactor(lbl_icon, 0);
    hb_top->setStretchFactor(lbl_name, 1);
  }

  VSExplorerEntity::~VSExplorerEntity () {
  }

  void VSExplorerEntity::enterEvent(QEvent *e) {
    ListWidgetItem::enterEvent(e);
    set_bit(enflg, IS_HOVERED_ON);
    //VSExplorerListWidget *explorer = static_cast<VSExplorerListWidget*>(list);
    //setBackgroundMode(Qt::PaletteHighlight);
    chb_select->setEnabled(true); chb_select->show();

    invalidateHeight();
    list->slotUpdateContents();

    // Color children into our color
    // lwi_p chi = chd;
    // while(chi != 0) {
    //   chi->setBackgroundMode(Qt::PaletteHighlight);
    //   chi = chi->sibling();
    // }

    installEventFilter(list);
    list->viewport()->setFocusProxy(this);
    setFocus();
  }

  void VSExplorerEntity::leaveEvent(QEvent *e) {
    ListWidgetItem::leaveEvent(e);
    clear_bit(enflg, IS_HOVERED_ON);
    //VSExplorerListWidget *explorer = static_cast<VSExplorerListWidget*>(list);
    //setBackgroundMode(Qt::PaletteBackground);
    chb_select->setEnabled(false); chb_select->hide();

    if(controlsVisible()) {
      hideControls();
      adjustSize();
    }

    invalidateHeight();
    list->slotUpdateContents();

    // Color children into our color
    // lwi_p chi = chd;
    // while(chi != 0) {
    //   chi->setBackgroundMode(Qt::PaletteBackground);
    //   chi = chi->sibling();
    // }

    removeEventFilter(list);
    list->viewport()->setFocusProxy(list);
    list->setFocus();
  }

  //void VSExplorerEntity::invalidateHeight() {
  //  ListWidgetItem::invalidateHeight();
  //}

  void VSExplorerEntity::expand() {
    ListWidgetItem::expand();
    btn_expand->setText("-");
  }

  void VSExplorerEntity::collapse() {
    ListWidgetItem::collapse();
    btn_expand->setText("+");
  }

  bool VSExplorerEntity::hit_item(const QPoint &p) const {
    return hb_top->rect().contains(p);
  }

  void VSExplorerEntity::setExpandable(bool e) {
    if(e) {
      if(!isExpandable() || btn_expand == 0) {
        ListWidgetItem::setExpandable(e);
        vb_ctrl->show();

        // Expand button
        btn_expand = new QPushButton(vb_ctrl, "btn_expand");
        btn_expand->setToggleButton(true);
        btn_expand->setText("+");
        btn_expand->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
        btn_expand->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
        btn_expand->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        connect(btn_expand, SIGNAL(toggled(bool)), this, SLOT(slotToggleExpand(bool)));

        // btn_expand->setEnabled(false); btn_expand->hide();
        btn_expand->show();
      }
    }
    else {
      ListWidgetItem::setExpandable(e);
      disconnect(btn_expand, SIGNAL(toggled(bool)), this, SLOT(slotToggleExpand(bool)));
      vb_ctrl->layout()->remove(btn_expand);
      vb_ctrl->removeChild(btn_expand);
      delete btn_expand;

      if(vb_ctrl->children()->count()  == 0) { vb_ctrl->hide(); }
    }

    adjustSize();
    updateGeometry();
  }

  void VSExplorerEntity::showControls() {
    invalidateHeight();
    set_bit(enflg, CONTROLS_VISIBLE);
  }

  void VSExplorerEntity::hideControls() {
    invalidateHeight();
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
    explorer->setVScrollBarMode(QScrollView::AlwaysOn);
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
        m_part->selectPrj(prj);
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
        m_part->selectFile(fl);
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
    if(sln->getUI() == 0) {
      // Solution item
      uivss_p uisln = new VSSlnNode(sln, explorer->viewport());
      if(uisln != 0) {
        BOOSTVEC_PUSHBACK(uients, uisln);
        explorer->insertItem(uisln, 0);
#ifdef DEBUG
        kddbg << QString("UISLN [%1] +\n").arg(sln->getName());
#endif
        sln->setUI(uisln);

        //TODO: Think about referencing to the parent UI sln before UI sln is actually in the list
        // since insertion happens below

        // Insert filters, if any
        //NOTE: Filters must be populated first, because we track UI dependencies via filters
        //  all UI project|files are not attached to "model" representation in that matter
        if(!sln->filts().empty()) {
          BOOSTVEC_FOR(vsf_ci, it, sln->filts()) {
            vsf_p flt(*it);
            if(flt != 0 && flt->getUI() == 0) {
              uivsf_p uiflt = addFilterNode(uisln, flt);
#ifdef DEBUG
              if(uiflt != 0) {
                kddbg << QString("UISLN [%1] << UIFLT [%2].\n").arg(sln->getName()).arg(flt->getName());
              }
              else {
                kddbg << QString(VSPART_ERROR"UISLN [%1] can't create UIFLT [%2].\n").
                    arg(sln->getName()).arg(flt->getName());
                //delete uisln;
                //return 0;
              }
#endif
            }
            //else { //NOTE: corrupted list report
            //}
          }
        }

        // Create UI items for all projects that are not in filters
        if(!sln->projs().empty()) {
          BOOSTVEC_FOR(vsp_ci, it, sln->projs()) {
            vsp_p prj(*it);
            if(prj != 0 && prj->getUI() == 0) {
              uivsp_p uiprj = addProjectNode(uisln, prj);
#ifdef DEBUG
              if(uiprj != 0) {
                kddbg << QString("UISLN [%1] << UIPRJ [%2].\n").arg(sln->getName()).arg(prj->getName());
              }
              else {
                kddbg << QString(VSPART_ERROR"UISLN [%1] can't create UIPRJ [%2].\n").
                    arg(sln->getName()).arg(prj->getName());
                //delete uisln;
                //return 0;
              }
#endif
            }
            //else { //NOTE: corrupted list report
            //}
          }
        }

        explorer->slotUpdateContents();
        return uisln;
      }
      else { kddbg << g_err_notenoughmem.arg(QString("uivss: [%1]").arg(sln->getName())).
        arg("VSExplorer::addSolutionNode"); }
    }
    return 0;
  }

  uivsp_p VSExplorer::addProjectNode(uivse_p uipnt, vsp_p prj) {
    if(prj->getUI() == 0) {
      switch(uipnt->getType()) {
        case vs_filter:
        case vs_solution: {
          // Project item
          uivsp_p uiprj = new VSPrjNode(prj, explorer->viewport());
          if(uiprj != 0) {
            BOOSTVEC_PUSHBACK(uients, uiprj);
            explorer->insertItem(uiprj, uipnt);
#ifdef DEBUG
            kddbg << QString("UIPRJ [%1] +\n").arg(prj->getName());
#endif
            prj->setUI(uiprj);

            // Filters in the project
            //NOTE: We populate filters first, because in UI context they contain their children.
            // In "model" context parent can be only project|solution
            if(!prj->filts().empty()) {
              BOOSTVEC_FOR(vsf_ci, it, prj->filts()) {
                vsf_p flt(*it);
                if(flt != 0 && flt->getUI() == 0) {
                  uivsf_p uiflt = addFilterNode(uiprj, flt);
#ifdef DEBUG
                  if(uiflt != 0) {
                    kddbg << QString("UIPRJ [%1] << UIFLT [%2].\n").arg(prj->getName()).arg(flt->getName());
                  }
                  else {
                    kddbg << QString(VSPART_ERROR"UIPRJ [%1] can't create UIFLT [%2].\n").
                        arg(prj->getName()).arg(flt->getName());
                    //delete uiprj;
                    //return 0;
                  }
#endif
                }
                //else { //NOTE: corrupted list report
                //}
              }
            }

            // Files in the project
            if(!prj->fls().empty()) {
              BOOSTVEC_FOR(vsfl_ci, it, prj->fls()) {
                vsfl_p fl(*it);
                if(fl != 0 && fl->getUI() == 0) {
                  uivsfl_p uifl = addFileNode(uiprj, fl);
#ifdef DEBUG
                  if(uifl != 0) {
                    kddbg << QString("UIPRJ [%1] << UIFL [%2].\n").arg(prj->getName()).arg(fl->getName());
                  }
                  else {
                    kddbg << QString(VSPART_ERROR"UIPRJ [%1] can't create UIFL [%2].\n").
                        arg(prj->getName()).arg(fl->getName());
                    //delete uiprj;
                    //return 0;
                  }
#endif
                }
                //else { //NOTE: corrupted list report
                //}
              }
            }

            return uiprj;
          }
          else { kddbg << g_err_notenoughmem.arg(QString("uivsp: [%1]").arg(prj->getName())).
            arg("VSExplorer::addProjectNode"); }
          break; }
        default: { break; }
      }
    }
    return 0;
  }

  uivsf_p VSExplorer::addFilterNode(uivse_p uipnt, vsf_p flt) {
    if(flt->getUI() == 0) {
      switch(uipnt->getType()) {
        case vs_solution:
        case vs_project:
        case vs_filter: {
          // Filter item
          uivsf_p uiflt = new VSFltNode(flt, explorer->viewport());
          if(uiflt != 0) {
            BOOSTVEC_PUSHBACK(uients, uiflt);
            explorer->insertItem(uiflt, uipnt);
#ifdef DEBUG
            kddbg << QString("UIFLT [%1] +\n").arg(flt->getName());
#endif
            flt->setUI(uiflt);

            // Contents of filter
            if(!flt->children().empty()) {
              BOOSTVEC_FOR(vse_ci, it, flt->children()) {
                vse_p ent(*it);
                if(ent != 0) {
                  switch(ent->getType()) {
                    case vs_project: {
                      vsp_p prj = static_cast<vsp_p>(ent);
                      if(prj->getUI() == 0) {
                        uivsp_p uiprj = addProjectNode(uiflt, prj);
#ifdef DEBUG
                        if(uiprj != 0) {
                          kddbg << QString("UIFLT [%1] << UIPRJ [%2].\n").arg(flt->getName()).arg(prj->getName());
                        }
                        else {
                          kddbg << QString(VSPART_ERROR"UIFLT [%1] can't create UIPRJ [%2].\n").
                              arg(flt->getName()).arg(prj->getName());
                          delete uiflt;
                          return 0;
                        }
#endif
                      }
                      break; }
                    case vs_filter: {
                      vsf_p eflt = static_cast<vsf_p>(ent);
                      if(eflt->getUI() == 0) {
                        uivsf_p euiflt = addFilterNode(uiflt, eflt);
#ifdef DEBUG
                        if(euiflt != 0) {
                          kddbg << QString("UIFLT [%1] << UIFLT [%2].\n").arg(flt->getName()).arg(eflt->getName());
                        }
                        else {
                          kddbg << QString(VSPART_ERROR"UIFLT [%1] can't create UIFLT [%2].\n").
                              arg(flt->getName()).arg(eflt->getName());
                          delete uiflt;
                          return 0;
                        }
#endif
                      }
                      break; }
                    case vs_file: {
                      vsfl_p fl = static_cast<vsfl_p>(ent);
                      if(fl->getUI() == 0) {
                        uivsfl_p uifl = addFileNode(uiflt, fl);
#ifdef DEBUG
                        if(uifl != 0) {
                          kddbg << QString("UIFLT [%1] << UIFL [%2].\n").arg(flt->getName()).arg(fl->getName());
                        }
                        else {
                          kddbg << QString(VSPART_ERROR"UIFLT [%1] can't create UIFL [%2].\n").
                              arg(flt->getName()).arg(fl->getName());
                          delete uiflt;
                          return 0;
                        }
#endif
                      }
                      break; }
                    default: { break; }
                  }
                }
                else { //NOTE: corrupted list report
                  kddbg << g_err_list_corrupted.arg("children").arg("VSExplorer::addFilterNode");
                  return 0;
                }
              } /* BOOSTVEC( for each child ) */
            }

            return uiflt;
          }
          else { kddbg << g_err_notenoughmem.arg(QString("uivsf: [%1]").arg(flt->getName())).
            arg("VSExplorer::addFilterNode"); }
          break; }
        default: { break; }
      }
    }
    return 0;
  }

  uivsfl_p VSExplorer::addFileNode(uivse_p uipnt, vsfl_p fl) {
    if(fl->getUI() == 0) {
      switch(uipnt->getType()) {
        case vs_project:
        case vs_filter: {
          // UI File item
          uivsfl_p uifl = new VSFilNode(fl, explorer->viewport());
          if(uifl != 0) {
            BOOSTVEC_PUSHBACK(uients, uifl);
            explorer->insertItem(uifl, uipnt);
#ifdef DEBUG
            kddbg << QString("UIFL [%1] +\n").arg(fl->getName());
#endif
            fl->setUI(uifl);
            return uifl;
          }
          else { kddbg << g_err_notenoughmem.arg(QString("uivsfl: [%1]").arg(fl->getName())).
            arg("VSExplorer::addFileNode"); }
          return 0; }
        default: { break; }
      }
    }
    return 0;
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
    // Tools layout
    hb_tools = new QHBox(this, "hb_tools");
    // QHBoxLayout *hbl_tools = new QHBoxLayout(hb_tools, 1, 0, "hbl_tools");
    //QSpacerItem *spc_tools = new QSpacerItem(2, VSPART_UIBTN_MINH, QSizePolicy::Expanding, QSizePolicy::Minimum);
    //hbl_tools->layout()->addItem(spc_tools);
    vbl_elem->addWidget(hb_tools);
    // Configure btn
    btn_cfg = new QPushButton(hb_tools, "btn_cfg");
    btn_cfg->setPixmap(SmallIcon("package_utilities"));
    btn_cfg->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_cfg->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_cfg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QToolTip::add(btn_cfg, i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));
    // hbl_tools->addWidget(btn_cfg);
    // btn_cfg->setEnabled(false); btn_cfg->hide();
    // Configure combo
    cmb_cfg = new QComboBox(hb_tools, "cmb_cfg");
    cmb_cfg->setAutoCompletion(false);
    cmb_cfg->setMinimumHeight(VSPART_UIBTN_MINH);
    cmb_cfg->setMaximumHeight(VSPART_UIBTN_MAXH);
    cmb_cfg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    BOOSTVEC_FOR(vsbb_ci, it, sln->bbs()) {
      vsbb_p bb(*it);
      if(bb != 0) {
        cmb_cfg->insertItem(bb->config().toString());
      }
    }
    // hbl_tools->addWidget(cmb_cfg);
    // Build btn
    btn_bld = new QPushButton(hb_tools, "btn_bld");
    btn_bld->setPixmap(SmallIcon("build"));
    btn_bld->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_bld->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_bld->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_bld, i18n(VSPART_ACTION_BUILD_SOLUTION_TIP));
    // hbl_tools->addWidget(btn_bld);
    // btn_bld->setEnabled(false); btn_bld->hide();
    // Clear btn
    btn_clr = new QPushButton(hb_tools, "btn_clr");
    btn_clr->setPixmap(SmallIcon("eraser"));
    btn_clr->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_clr->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_clr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_clr, i18n(VSPART_ACTION_CLEAN_SOLUTION_TIP));
    // hbl_tools->addWidget(btn_clr);
    // btn_clr->setEnabled(false); btn_clr->hide();

    // hbl_tools->setStretchFactor(lbl_cfg, 1);
    // hbl_tools->setStretchFactor(btn_cfg, 1);
    // hbl_tools->setStretchFactor(btn_bld, 0);
    // hbl_tools->setStretchFactor(btn_clr, 0);

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
  }

  void VSSlnNode::leaveEvent(QEvent *e) {
    VSExplorerEntity::leaveEvent(e);
    //lbl_icon->setBackgroundMode(Qt::PaletteBackground);
  }

  void VSSlnNode::showControls() {
    VSExplorerEntity::showControls();
    hb_tools->setEnabled(true); hb_tools->show();
    list->slotUpdateContents();
  }

  void VSSlnNode::hideControls() {
    VSExplorerEntity::hideControls();
    hb_tools->setEnabled(false); hb_tools->hide();
    list->slotUpdateContents();
  }

  void VSSlnNode::setState(const QString &/*state*/) {
  }

  void VSSlnNode::slotRefreshText() {
    lbl_name->setText(sln->getName());

    if(!sln->isReachable()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      //btn_cfg->setText(i18n("unreachable"));
      return;
    }

    if(!sln->isLoaded()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      //btn_cfg->setText(i18n("load error"));
      return;
    }

    // See if solution in "detached" state (i.e. no config selected for build)
    if(sln->isDetached()) {
      lbl_icon->setPixmap(SmallIcon("error"));
      //btn_cfg->setText(i18n("detached"));
    }
    else {
      vcfg_cp cfg = sln->currentCfg();
      // The count of projects
      lbl_icon->setPixmap(SmallIcon("gohome"));
      //btn_cfg->setText(QString((cfg != 0) ? cfg->toString() : "error"));
    }
  }

  //===========================================================================
  // VStudio explorer widget entity project class methods
  //===========================================================================
  VSPrjNode::VSPrjNode(vsp_p p, QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : VSExplorerEntity(pnt, nm, fl)
  , prj(p) {
    sln = static_cast<vss_p>(p->getParent())->getUI();
    // Tools layout
    hb_tools = new QHBox(this, "hb_tools");
    vbl_elem->addWidget(hb_tools);
    // Tools h-spacer
    //QSpacerItem *spc_tools = new QSpacerItem(2, VSPART_UIBTN_MINH, QSizePolicy::Expanding, QSizePolicy::Minimum);
    //hb_tools->layout()->addItem(spc_tools);
    // Configure btn
    btn_cfg = new QPushButton(hb_tools, "btn_cfg");
    btn_cfg->setPixmap(SmallIcon("package_utilities"));
    btn_cfg->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_cfg->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_cfg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QToolTip::add(btn_cfg, i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));
    // Configure combo
    cmb_cfg = new QComboBox(hb_tools, "cmb_cfg");
    cmb_cfg->setAutoCompletion(false);
    cmb_cfg->setMinimumHeight(VSPART_UIBTN_MINH);
    cmb_cfg->setMaximumHeight(VSPART_UIBTN_MAXH);
    cmb_cfg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    BOOSTVEC_FOR(vsbb_ci, it, prj->bbs()) {
      vsbb_p bb(*it);
      if(bb != 0) {
        cmb_cfg->insertItem(bb->config().toString());
      }
    }
    // Build btn
    btn_bld = new QPushButton(hb_tools, "btn_bld");
    btn_bld->setPixmap(SmallIcon("build"));
    btn_bld->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_bld->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_bld->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_bld, i18n(VSPART_ACTION_BUILD_SOLUTION_TIP));
    // Clear btn
    btn_clr = new QPushButton(hb_tools, "btn_clr");
    btn_clr->setPixmap(SmallIcon("eraser"));
    btn_clr->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_clr->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_clr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_clr, i18n(VSPART_ACTION_CLEAN_SOLUTION_TIP));

    hb_tools->hide();
    hb_tools->setEnabled(false);

    slotRefreshText();
  }

  VSPrjNode::~VSPrjNode() {
  }

  void VSPrjNode::enterEvent(QEvent *e) {
    VSExplorerEntity::enterEvent(e);
  }

  void VSPrjNode::leaveEvent(QEvent *e) {
    VSExplorerEntity::leaveEvent(e);
  }

  void VSPrjNode::showControls() {
    VSExplorerEntity::showControls();
    hb_tools->setEnabled(true); hb_tools->show();
    list->slotUpdateContents();
  }

  void VSPrjNode::hideControls() {
    VSExplorerEntity::hideControls();
    hb_tools->setEnabled(false); hb_tools->hide();
    list->slotUpdateContents();
  }

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
    // Init container pointer
    switch(flt->getParent()->getType()) {
      case vs_project: { container = static_cast<vsp_p>(flt->getParent())->getUI(); break; }
      case vs_solution: { container = static_cast<vss_p>(flt->getParent())->getUI(); break; }
      case vs_filter: { container = static_cast<vsf_p>(flt->getParent())->getUI(); break; }
      default: { break; }
    }
    // Tools layout
    hb_tools = new QHBox(this, "hb_tools");
    vbl_elem->addWidget(hb_tools);
    // Tools h-spacer
    QSpacerItem *spc_tools = new QSpacerItem(2, VSPART_UIBTN_MINH, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hb_tools->layout()->addItem(spc_tools);
    // Build btn
    btn_bld = new QPushButton(hb_tools, "btn_bld");
    btn_bld->setPixmap(SmallIcon("build"));
    btn_bld->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_bld->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_bld->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_bld, i18n(VSPART_ACTION_BUILD_SOLUTION_TIP));
    // Clear btn
    btn_clr = new QPushButton(hb_tools, "btn_clr");
    btn_clr->setPixmap(SmallIcon("eraser"));
    btn_clr->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_clr->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_clr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_clr, i18n(VSPART_ACTION_CLEAN_SOLUTION_TIP));

    hb_tools->hide();
    hb_tools->setEnabled(false);

    slotRefreshText();
  }

  VSFltNode::~VSFltNode() {
  }

  void VSFltNode::enterEvent(QEvent *e) {
    VSExplorerEntity::enterEvent(e);
  }

  void VSFltNode::leaveEvent(QEvent *e) {
    VSExplorerEntity::leaveEvent(e);
  }

  void VSFltNode::showControls() {
    VSExplorerEntity::showControls();
    hb_tools->setEnabled(true); hb_tools->show();
    list->slotUpdateContents();
  }

  void VSFltNode::hideControls() {
    VSExplorerEntity::hideControls();
    hb_tools->setEnabled(false); hb_tools->hide();
    list->slotUpdateContents();
  }

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
    // Init container pointer
    switch(file->getParent()->getType()) {
      case vs_project: { container = static_cast<vsp_p>(file->getParent())->getUI(); break; }
      case vs_filter: { container = static_cast<vsf_p>(file->getParent())->getUI(); break; }
      default: { break; }
    }
    // Tools layout
    hb_tools = new QHBox(this, "hb_tools");
    vbl_elem->addWidget(hb_tools);
    // Tools h-spacer
    //QSpacerItem *spc_tools = new QSpacerItem(2, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    //hbl_tools->addItem(spc_tools);
    // Configure btn
    btn_cfg = new QPushButton(hb_tools, "btn_cfg");
    btn_cfg->setPixmap(SmallIcon("package_utilities"));
    btn_cfg->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_cfg->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_cfg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QToolTip::add(btn_cfg, i18n(VSPART_ACTION_CONFIGURE_ENTITY_TIP));
    // Configure combo
    cmb_cfg = new QComboBox(hb_tools, "cmb_cfg");
    cmb_cfg->setAutoCompletion(false);
    cmb_cfg->setMinimumHeight(VSPART_UIBTN_MINH);
    cmb_cfg->setMaximumHeight(VSPART_UIBTN_MAXH);
    cmb_cfg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    if(file->bbs().empty()) {
      cmb_cfg->setEnabled(false);
    }
    else {
      BOOSTVEC_FOR(vsbb_ci, it, file->bbs()) {
        vsbb_p bb(*it);
        if(bb != 0) {
          cmb_cfg->insertItem(bb->config().toString());
        }
      }
    }
    // Build btn
    btn_bld = new QPushButton(hb_tools, "btn_bld");
    btn_bld->setPixmap(SmallIcon("build"));
    btn_bld->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_bld->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_bld->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_bld, i18n(VSPART_ACTION_BUILD_SOLUTION_TIP));
    // Clear btn
    btn_clr = new QPushButton(hb_tools, "btn_clr");
    btn_clr->setPixmap(SmallIcon("eraser"));
    btn_clr->setMinimumSize(QSize(VSPART_UIBTN_MINW,VSPART_UIBTN_MINH));
    btn_clr->setMaximumSize(QSize(VSPART_UIBTN_MAXW,VSPART_UIBTN_MAXH));
    btn_clr->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QToolTip::add(btn_clr, i18n(VSPART_ACTION_CLEAN_SOLUTION_TIP));

    hb_tools->hide();
    hb_tools->setEnabled(false);

    delete vb_ctrl;

    slotRefreshText();
  }

  VSFilNode::~VSFilNode() {
  }

  void VSFilNode::enterEvent(QEvent *e) {
    VSExplorerEntity::enterEvent(e);
  }

  void VSFilNode::leaveEvent(QEvent *e) {
    VSExplorerEntity::leaveEvent(e);
  }

  void VSFilNode::showControls() {
    VSExplorerEntity::showControls();
    hb_tools->setEnabled(true); hb_tools->show();
    list->slotUpdateContents();
  }

  void VSFilNode::hideControls() {
    VSExplorerEntity::hideControls();
    hb_tools->setEnabled(false); hb_tools->hide();
    list->slotUpdateContents();
  }

  void VSFilNode::setState(const QString &st) {
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
}; /* namespace VStudio */
#include "vs_explorer.moc"
