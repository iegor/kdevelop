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
#include "vstudiopart.h"
#include "vsmanagerwidget.h"

#include <kiconloader.h>
#include <kinstance.h>
#include <kurl.h>
#include <kaction.h>
#include <kactionclasses.h>
#include <kpopupmenu.h>
#include <kconfig.h>

#include <urlutil.h>
#include <kdevcore.h>
#include <kdevlanguagesupport.h>
#include <kdevproject.h>
#include <kdevpartcontroller.h>
#include <codemodel.h>
#include <codemodel_utils.h>

#include <klocale.h>
#include <kdebug.h>

#include <qheader.h>
#include <qdir.h>
#include <qstylesheet.h>
#include <qstringlist.h>

VSExplorer::VSExplorer(VStudioPart * part, QWidget *parent, const char *name)
  : VSExplorerWidget(parent, name) , m_part(part) {
  addColumn("");
  header()->hide();
  setSorting(0);
  setRootIsDecorated(true);
  setAllColumnsShowFocus(true);

//   connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotExecuted(QListViewItem*)));

  //NOTE: kind of "track active item in solution explorer"
  m_actionFollowEditor = new KToggleAction(i18n("Follow Editor"), KShortcut(), this, SLOT(slotFollowEditor()), m_part->actionCollection(), "classview_follow_editor");

  // Configure explorer from config file
//   KConfig* config = m_part->instance()->config();
//   config->setGroup("General");
//   m_doFollowEditor = config->readBoolEntry("FollowEditor", false);
}

VSManagerWidget::~VSManagerWidget() {
//   KConfig* config = m_part->instance()->config();
//   config->setGroup("General");
//   config->writeEntry( "ViewMode", viewMode() );
//   config->writeEntry("FollowEditor", m_doFollowEditor);
//   config->sync();
}

void VSManagerWidget::slotProjectOpened() {
}

void VSManagerWidget::slotProjectClosed() {
}

ProjectItem::ProjectItem(Type type, ProjectItem *parent, const QString &text)
  : QListViewItem(parent, text), typ(type) {
  bld = false;
}

void ProjectItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment) {
//   if(isBold()) {
//     QFont font(p->font());
//     font.setBold(true);
//     p->setFont(font);
//   }
//   QListViewItem::paintCell(p, cg, column, width, alignment);
}

#include "vsmanagerwidget.moc"
