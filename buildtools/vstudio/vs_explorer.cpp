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

  void VSExplorer::slotContextMenu(KListView */*lv*/, QListViewItem *item, const QPoint &p) {
    if(!item) {
      kddbg << "Error! no item" << endl;
      return;
    }

    VSExplorerEntity *i = (VSExplorerEntity*)item;
    KPopupMenu menu(this);
    menu.insertTitle(*i->pixmap(0), i18n("Solution \"%1\"").arg(i->getModelRepresentation()->getName()));
    actSetEntityRltPath->plug(&menu);
    actCfgEntity->plug(&menu);
    actRenameEntity->plug(&menu);
    menu.exec(p);
  }

  void VSExplorer::slotEntityRenamed(QListViewItem *item, const QString &name, int /*col*/) {
    if(!item) {
      kddbg << "Error! no item" << endl;
      return;
    }
    kddbg << "Entity is renamed to: " << name << endl;
    ((VSExplorerEntity*)(item))->getModelRepresentation()->setName(name);
  }

  void VSExplorer::slotProjectOpened() {
  }

  void VSExplorer::slotProjectClosed() {
  }

  void VSExplorer::slotSetEntityRltPath() {
    // ((VSExplorerEntity*)item)->getModelRepresentation()->setRelativePath("");
    /*
    kddbg << "\"" << ((VSExplorerEntity*)item)->getModelRepresentation()->getName() <<
        "\" is set to \"" << ((VSExplorerEntity*)item)->getModelRepresentation()->getRelativePath()
        << "\"\n";
    */
  }

  void VSExplorer::slotConfigureEntity() {
    // kddbg << "slotConfigureEntity" << endl;
    QPtrList<QListViewItem> items = m_listView->selectedItems();
    for(items_ci it=items.begin(); it!=items.end(); ++it) {
      VSExplorerEntity* ent = (VSExplorerEntity*)(*it);
      if(ent->getType() == vs_solution) {
        m_part->saveVsSolution((vss_p)ent->getModelRepresentation());
      }
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
      VSExplorerEntity* ent = (VSExplorerEntity*)(*it);
      kddbg << "remaning item: " << ent->getModelRepresentation()->getName() << endl;
      m_listView->rename((*it), 0);
    }
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
    for(uive_ci it=uients.begin(); it!=uients.end(); ++it) {
#else
    //TODO: Implement this
#endif
      if((*it)->uidGet() == uid) {
        return (*it);
      }
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
    if(getType() == vs_solution) {
      QFont font(p->font());
      font.setBold(true);
      p->setFont(font);
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

  //===========================================================================
  // Visual studio explorer widget entity file class methods
  //===========================================================================
  VSFilNode::VSFilNode(uivse_p pnt, vsfl_p fl)
  : VSExplorerEntity(vs_file, pnt, fl->getName())
  , prj((uivsp_p)pnt)
  , file(fl) {
    setPixmap(0, SmallIcon("file"));
  }

  VSFilNode::~VSFilNode() {
  }
};
#include "vs_explorer.moc"
