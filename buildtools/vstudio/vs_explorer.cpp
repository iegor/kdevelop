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
// #include <kpopupmenu.h>
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

namespace VStudio {
  //===========================================================================
  // Visual studio explorer widget methods
  //===========================================================================
  VSExplorer::VSExplorer(VSPart * part, QWidget *parent, const char *name)
    : VsExplorerWidget(parent, name), m_part(part) {
    m_listView->addColumn("");
    m_listView->header()->hide();
    m_listView->setSorting(0);
    m_listView->setRootIsDecorated(true);
    m_listView->setAllColumnsShowFocus(true);

  //   connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));

    //NOTE: kind of "track active item in solution explorer"
  //   m_actionFollowEditor = new KToggleAction(i18n("Follow Editor"), KShortcut(), this, SLOT(slotFollowEditor()), m_part->actionCollection(), "classview_follow_editor");

    // Configure explorer from config file
  //   KConfig* config = m_part->instance()->config();
  //   config->setGroup("General");
  //   m_doFollowEditor = config->readBoolEntry("FollowEditor", false);
  }

  VSExplorer::~VSExplorer() {
  //   KConfig* config = m_part->instance()->config();
  //   config->setGroup("General");
  //   config->writeEntry( "ViewMode", viewMode() );
  //   config->writeEntry("FollowEditor", m_doFollowEditor);
  //   config->sync();
  }

  void VSExplorer::slotProjectOpened() {
  }

  void VSExplorer::slotProjectClosed() {
  }

  VSSlnNode* VSExplorer::addSolutionNode(VSSolution *sln) {
    VSSlnNode *item = new VSSlnNode(m_listView, sln);
    if(item == 0) {
      kddbg << "Error! Out of memory" << endl;
    } else {
      item->setPixmap(0, SmallIcon("tar"));
      //setPixmap( 0, SmallIcon( "tar" ) : SmallIcon( "binary" ) );
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
