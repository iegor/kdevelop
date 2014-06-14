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
#include <qlayout.h>
#include <qpainter.h>
#include <qheader.h>
#include <qdir.h>
#include <qstylesheet.h>

#include <kiconloader.h>
#include <kinstance.h>
#include <kurl.h>
#include <kaction.h>
#include <kactionclasses.h>
#include <kpopupmenu.h>
#include <kconfig.h>
#include <klistview.h>

// #include <urlutil.h>
// #include <kdevcore.h>
// #include <kdevlanguagesupport.h>
// #include <kdevproject.h>
// #include <kdevpartcontroller.h>
// #include <codemodel.h>
// #include <codemodel_utils.h>

#include <klocale.h>
#include <kdebug.h>

#include "vs_explorer.h"
#include "vs_part.h"

class KPopupMenu;

namespace VStudio {
  //===========================================================================
  // Visual studio explorer widget methods
  //===========================================================================
  VSExplorer::VSExplorer(VSPart * part, QWidget *parent, const char *name)
    : VsExplorerWidget(parent, name)
    , m_part(part)
    , actions(0) {
    m_listView->addColumn("");
    m_listView->header()->hide();
    m_listView->setSorting(0);
    m_listView->setRootIsDecorated(true);
    m_listView->setAllColumnsShowFocus(true);
    m_listView->setItemsRenameable(true);

  //   connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));

    //NOTE: kind of "track active item in solution explorer"
  //   m_actionFollowEditor = new KToggleAction(i18n("Follow Editor"), KShortcut(), this, SLOT(slotFollowEditor()), m_part->actionCollection(), "classview_follow_editor");

    // Configure explorer from config file
  //   KConfig* config = m_part->instance()->config();
  //   config->setGroup("General");
  //   m_doFollowEditor = config->readBoolEntry("FollowEditor", false);

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

    connect(m_listView, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
            this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&)));
    connect(m_listView, SIGNAL(itemRenamed(QListViewItem*, const QString&, int)),
            this, SLOT(slotEntityRenamed(QListViewItem*, const QString&, int)));
    connect(m_listView, SIGNAL(selectionChanged(QListViewItem*)),
            this, SLOT(slotSelectItem(QListViewItem*)));
  }

  VSExplorer::~VSExplorer() {
  //   KConfig* config = m_part->instance()->config();
  //   config->setGroup("General");
  //   config->writeEntry( "ViewMode", viewMode() );
  //   config->writeEntry("FollowEditor", m_doFollowEditor);
  //   config->sync();
    delete actions;
    actions = 0;
  }

  void VSExplorer::slotSelectItem(QListViewItem *item) {
    uivse_p ent = static_cast<uivse_p>(item);
#ifdef DEBUG
    kddbg << g_msg_entselected.arg(type2String(ent->getType())).arg(ent->getName());
#endif

    switch(ent->getType()) {
      case vs_solution: {
        m_part->selectSln(static_cast<vss_p>(ent->getModel()));
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

  void VSExplorer::slotContextMenu(KListView */*lv*/, QListViewItem *item, const QPoint &p) {
    if(item != 0) {
      KPopupMenu menu(this);
      uivse_p i = static_cast<uivse_p>(item);
      switch(i->getType()) {
        case vs_solution: {
          vss_p sln = static_cast<vss_p>(i->getModel());
          menu.insertTitle(*i->pixmap(0), i18n("Solution: \"%1\"").arg(sln->getName()));
          menu.insertItem("Save", this, SLOT(slotSaveEntity()));
          menu.insertSeparator();
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
  }

  void VSExplorer::slotEntityRenamed(QListViewItem *item, const QString &name, int /*col*/) {
    if(item != 0) {
      uivse_p ent = static_cast<uivse_p>(item);
      ent->setName(name);
      kddbg << "Entity is renamed to: " << name << endl;
    } else {
      kddbg << "Error! no item" << endl;
    }
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
    // kddbg << "slotConfigureEntity" << endl;
    QPtrList<QListViewItem> items = m_listView->selectedItems();
    for(items_ci it=items.begin(); it!=items.end(); ++it) {
      VSExplorerEntity* ent = (VSExplorerEntity*)(*it);
    }
  }

  void VSExplorer::slotRenameEntity() {
    if(!m_listView->itemsRenameable()) {
      kddbg << "Items are not renamable" << endl;
      return;
    }
    kddbg << "slotRenameEntity" << endl;

    QPtrList<QListViewItem> items = m_listView->selectedItems();
    for(items_ci it=items.begin(); it!=items.end(); ++it) {
      uivse_p ent = static_cast<uivse_p>(*it);
      if(ent != 0) {
        kddbg << "remaning item: " << ent->getName() << endl;
        m_listView->rename((*it), 0);
        ent->setName(ent->text(0));
      }
    }
  }

  void VSExplorer::slotActivateEntity() {
    uivse_p sel = static_cast<uivse_p>(m_listView->selectedItem());
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
    }
  }

  void VSExplorer::slotSaveEntity() {
    uivse_p sel = static_cast<uivse_p>(m_listView->selectedItem());
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
    }
  }

  void VSExplorer::slotSaveEntityAs() {
    m_part->saveSlnAs(0, QString::null);
  }

  uivss_p VSExplorer::addSolutionNode(vss_p sln) {
    uivss_p item = new VSSlnNode(m_listView, sln);
    if(item == 0) { kddbg << "Error! Out of memory" << endl; }
#ifdef USE_BOOST
    uients.push_back(item);
#else
    //TODO: Implement this
#endif
    return item;
  }

  uivsp_p VSExplorer::addProjectNode(uivse_p pnt, vsp_p prj) {
    switch(pnt->getType()) {
      case vs_filter:
      case vs_solution: {
        uivsp_p item = new VSPrjNode(pnt, prj);
        if(item == 0) { kddbg << "Error! Out of memory" << endl; }
        return item; }
      default:
        kddbg << "Can't add project into unsupported parent node of type \""
            << type2String(pnt->getType()) << "\"\n";
        return 0;
    }
  }

  uivsf_p VSExplorer::addFilterNode(uivse_p pnt, vsf_p flt) {
    switch(pnt->getType()) {
      case vs_solution:
      case vs_project:
      case vs_filter: {
        uivsf_p filter = new VSFltNode(pnt, flt);
        if(filter == 0) { kddbg << "Error! Out of memory" << endl; }
        return filter; }
      default:
        kddbg << "Can't add filter " << flt->getName()
            << " into unsupported parent node \"" << pnt->getName()
            << "\" of type \"" << type2String(pnt->getType()) << "\"\n";
        return 0;
    }
  }

  uivsfl_p VSExplorer::addFileNode(uivse_p pnt, vsfl_p fl) {
    switch(pnt->getType()) {
      case vs_project:
      case vs_filter: {
        uivsfl_p file = new VSFilNode(pnt, fl);
        if(file == 0) { kddbg << "Error! Out of memory" << endl; }
        return file; }
      default:
        kddbg << "Can't add file into unsupported parent node of type \""
            << type2String(pnt->getType()) << "\"\n";
        return 0;
    }
  }

  uivse_p VSExplorer::getByUID(const QUuid &uid) {
#ifdef USE_BOOST
    uivse_ci it=uients.begin();
    for(; it!=uients.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      uivse_p ent = static_cast<uivse_p>(*it);
      if(ent != 0) {
        if(ent->getUID() == uid) { break; }
      }
    }
#ifdef USE_BOOST
    if(it!=uients.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << "Can't find ui entity: " << guid2String(uid) << "\"\n";
      return 0;
    }
    return 0;
  }

  //===========================================================================
  // Visual studio SetPathWidget methods
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
  // Visual studio explorer widget entity base class methods
  //===========================================================================
  VSExplorerEntity::VSExplorerEntity(e_VSEntityType type, QListView *parent, const QString &text)
  : QListViewItem(parent, text)
  , typ(type) {
  }

  VSExplorerEntity::VSExplorerEntity(e_VSEntityType type, uivse_p parent, const QString &text)
  : QListViewItem(parent, text)
  , typ(type) {
  }

  VSExplorerEntity::~VSExplorerEntity () {
  }

  void VSExplorerEntity::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {
    switch(getType()) {
      case vs_project: {
        if(static_cast<vsp_p>(this->getModel())->isActive()) {
          QFont font(p->font());
          font.setBold(true);
          p->setFont(font);
        }
        break; }
      case vs_solution: {
        if(static_cast<vss_p>(this->getModel())->isActive()) {
          QFont font(p->font());
          font.setBold(true);
          p->setFont(font);
        }
        break; }
      default: {
        break; }
    }
    QListViewItem::paintCell(p, cg, column, width, alignment);
  }

  //===========================================================================
  // Visual studio explorer widget entity solution class methods
  //===========================================================================
  VSSlnNode::VSSlnNode(QListView *lv, vss_p s)
  : VSExplorerEntity(vs_solution, lv, s->getName())
  , sln(s) {
    name = s->getName();
    setPixmap(0, SmallIcon("home"));
    // item->setText(1, name);
    // m_listView->insertItem(item);
  }

  VSSlnNode::~VSSlnNode() {
  }

  /*inline*/ const QString& VSSlnNode::getName() const {
    return sln->getName();
  }

  void VSSlnNode::setName(const QString& name) {
    sln->setName(name);
  }

  /*inline*/ vse_p VSSlnNode::getModel() const {
    return sln;
  }

  /*inline*/ const QUuid& VSSlnNode::getUID() const {
    return uid_null;
  }

  //===========================================================================
  // Visual studio explorer widget entity project class methods
  //===========================================================================
  VSPrjNode::VSPrjNode(uivse_p e, vsp_p p)
  : VSExplorerEntity(vs_project, e, p->getName())
  , prj(p) {
    name = p->getName();
    setPixmap(0, SmallIcon("tar"));
  }

  VSPrjNode::~VSPrjNode() {
  }

  /*inline*/ const QString& VSPrjNode::getName() const {
    return prj->getName();
  }

  void VSPrjNode::setName(const QString& name) {
    prj->setName(name);
  }

  /*inline*/ vse_p VSPrjNode::getModel() const {
    return prj;
  }

  /*inline*/ const QUuid& VSPrjNode::getUID() const {
    return prj->getUID();
  }

  //===========================================================================
  // Visual studio explorer widget entity filter class methods
  //===========================================================================
  VSFltNode::VSFltNode(uivse_p pnt, vsf_p flt)
  : VSExplorerEntity(vs_filter, pnt, flt->getName())
  , filter(flt)
  , parent(pnt) {
    name = flt->getName();
    setPixmap(0, SmallIcon("folder"));
  }

  VSFltNode::~VSFltNode() {
  }

  /*inline*/ const QString& VSFltNode::getName() const {
    return filter->getName();
  }

  void VSFltNode::setName(const QString& name) {
    filter->setName(name);
  }

  /*inline*/ vse_p VSFltNode::getModel() const {
    return filter;
  }

  /*inline*/ const QUuid& VSFltNode::getUID() const {
    return filter->getUID();
  }

  //===========================================================================
  // Visual studio explorer widget entity file class methods
  //===========================================================================
  VSFilNode::VSFilNode(uivse_p pnt, vsfl_p fl)
  : VSExplorerEntity(vs_file, pnt, fl->getName())
  , file(fl)
  , prj((uivsp_p)pnt) {
    setPixmap(0, SmallIcon("file"));
  }

  VSFilNode::~VSFilNode() {
  }

  /*inline*/ const QString& VSFilNode::getName() const {
    return file->getName();
  }

  void VSFilNode::setName(const QString& name) {
    file->setName(name);
  }

  /*inline*/ vse_p VSFilNode::getModel() const {
    return file;
  }

  /*inline*/ const QUuid& VSFilNode::getUID() const {
    return file->getUID();
  }

  /*inline*/ uivsp_p VSFilNode::getProject() const {
    return prj;
  }
};
#include "vs_explorer.moc"
