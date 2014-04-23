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

    QString caption_mask = i18n("Solution \"%1\"").arg(i->getModelRepresentation()->getName());
    QPixmap pixmap;

    switch(i->type()) {
      case vs_solution: {
        pixmap = SmallIcon("tar");
        break; }
      case vs_project: {
        pixmap = SmallIcon("file");
        break; }
      default:
        break;
    }

    KPopupMenu menu(this);
    menu.insertTitle(pixmap, caption_mask);
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
    kddbg << "slotSetEntityRltPath" << endl;
// //     if(!item) {
// //       kddbg << "Error! no item" << endl;
// //       return;
// //     }

//     kddbg << "Change path for " << ((VSExplorerEntity*)item)->getModelRepresentation()->getName() << endl;
  }

  void VSExplorer::slotConfigureEntity() {
    kddbg << "slotConfigureEntity" << endl;
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

  VSSlnNode* VSExplorer::addSolutionNode(VSSolution *sln) {
    VSSlnNode *item = new VSSlnNode(m_listView, sln);
    if(item == 0) {
      kddbg << "Error! Out of memory" << endl;
    } else {
      item->setPixmap(0, SmallIcon("tar"));
//     item->setText(1, name);
//     m_listView->insertItem(item);
    }
    return item;
  }

  VSPrjNode* VSExplorer::addProjectNode(VSSlnNode *sln, VSProject *prj) {
    VSPrjNode *item = new VSPrjNode(sln, prj);
    if(item == 0) {
      kddbg << "Error! Out of memory" << endl;
    } else {
      item->setPixmap(0, SmallIcon("file"));
    }
    return item;
  }

  //===========================================================================
  // Visual studio explorer widget entity base class methods
  //===========================================================================
  VSExplorerEntity::VSExplorerEntity(e_VSEntityType type, QListView *parent, const QString &text)
  : QListViewItem(parent, text)
  , typ(type) {
  }

  VSExplorerEntity::VSExplorerEntity(e_VSEntityType type, VSExplorerEntity *parent, const QString &text)
    : QListViewItem(parent, text), typ(type) {
  }

  void VSExplorerEntity::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {
    if(type() == vs_solution) {
      QFont font(p->font());
      font.setBold(true);
      p->setFont(font);
    }
    QListViewItem::paintCell(p, cg, column, width, alignment);
  }

  //===========================================================================
  // Visual studio explorer widget entity solution class methods
  //===========================================================================
  VSSlnNode::VSSlnNode(QListView *lv, VSSolution *s)
  : VSExplorerEntity(vs_solution, lv, s->getName())
  , sln(s) {
    name = s->getName();
  }

  //===========================================================================
  // Visual studio explorer widget entity project class methods
  //===========================================================================
  VSPrjNode::VSPrjNode(VSSlnNode *s, VSProject *p)
  : VSExplorerEntity(vs_project, s, p->getName())
  , prj(p) {
    name = p->getName();
  }
};
#include "vs_explorer.moc"
