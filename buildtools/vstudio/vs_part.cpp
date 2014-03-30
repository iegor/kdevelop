/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_part
*
* Description:
*  ! Parts were inherited from autoprojectpart src
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
#include <config.h>

#include <qdom.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qwhatsthis.h>
#include <qregexp.h>
#include <qgroupbox.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kaction.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kparts/part.h>
#include <kdeversion.h>
#include <kprocess.h>

#include <domutil.h>
#include <kdevcore.h>
#include <kdevmakefrontend.h>
#include <kdevappfrontend.h>
#include <kdevmainwindow.h>
#include <kdevpartcontroller.h>
#include <makeoptionswidget.h>
#include <runoptionswidget.h>
#include <envvartools.h>

#include <configwidgetproxy.h>
#include <kdevplugininfo.h>
#include <urlutil.h>

#include "vs_part.h"
#include "vs_model.h"

#define CONFIGURE_OPTIONS 1
#define RUN_OPTIONS 2
#define MAKE_OPTIONS 3

static const KDevPluginInfo data("kdevvs");
K_EXPORT_COMPONENT_FACTORY(libkdevvstudioproject, VStudioFactory(data))

VSPart::VSPart(QObject *parent, const char *name, const QStringList &/*args*/)
  : KDevBuildTool(&data, parent, name ? name : "VSPart") {
  KAction *action;
  setInstance(VStudioFactory::instance());
  setXMLFile("kdevvstudioproject.rc");

//   m_executeAfterBuild = false;
//    m_isKDE = ( args[0] == "kde" );
//   m_needMakefileCvs = false;

  m_widget = new vsw(this); //, m_isKDE);
  m_widget->setIcon(SmallIcon(info()->icon()));
  m_widget->setCaption(i18n("VS Manager"));
  QWhatsThis::add(m_widget, i18n("<b>VS manager</b><p>"
                                 "The tree looks like a standard VS project explorer,"
                                 "yet it has some additiona \"power\" features."));

  mainWindow()->embedSelectView(m_widget, i18n("VS Manager"), i18n("VS Manager, manages vs solution files"));

  // Build solution action
  action = new KAction(i18n("&Build Solution"), "make_kdevelop", Key_F8, this,
                       SLOT(slotBuildSolution()), actionCollection(), "build_build_solution");
  action->setToolTip(i18n("Builds entire solution"));
  action->setWhatsThis(i18n("<b>Build solution</b>"
                            "<p>Runs <b>build process</b>from the project directory.<br>"
                            "Environment variables and make arguments can be specified"
                            "in the project settings dialog, <b>Make Options</b> tab."));
  action->setGroup("vstools");

  // Build project action
  action = new KAction(i18n("&Build Project"), "make_kdevelop", Key_F7, this,
                       SLOT(slotBuildProject()), actionCollection(), "build_build_project");
  action->setToolTip(i18n("Builds selected(active) project"));
  action->setWhatsThis(i18n("<b>Build project</b>"
                            "<p>Runs <b>build process</b> from the project directory.<br>"
                            "Environment variables and make arguments can be specified"
                            "in the project settings dialog, <b>Make Options</b> tab."));
  action->setGroup("vstools");

  // Clean solution action
  action = new KAction(i18n("Clean solution"), "make_kdevelop", Key_F7, this,
                       SLOT(slotCleanSolution()), actionCollection(), "build_clean_solution");
  action->setToolTip(i18n("Cleans whole solution from produced binaries."));
  action->setWhatsThis(i18n("<b>Clean solution</b>"
                            "<p>Cleans out all output produced previously."));
  action->setGroup("vstools");

  connect(buildConfigAction, SIGNAL(activated(const QString&)), this, SLOT(slotBuildConfigChanged(const QString&)));
  connect(buildConfigAction->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(slotBuildConfigAboutToShow()));
}

VSPart::~VSPart() {
}

void VSPart::openProject(const QString &dirName, const QString &projectName) {
  QDomDocument &dom = *projectDom();
//     QStringList vsitms;
//     vsitms = DomUtil::readEntry(dom, "kdevvstudioproject/general");

  // Read all solutions and projects parse and setup them
  DomUtil::PairList vsitems = DomUtil::readPairListEntry(dom,
                              "kdevvstudioproject/general", "vsitem", "name", "path");
  DomUtil::PairList::ConstIterator it;
  for(it = vsitems.begin(); it != vsitems.end(); ++it) {
//         ( *it ).first;
//         ( *it ).second;
    kdDebug() << "[VS item]: " << (*it).first << " in " << (*it).second << endl;

    VSSolution sln((*it).first, (*it).second);
    if(!sln.Parse()) {
      kdDebug(9037) << "Can't parse solution file" << endl;
    }
  }

  KDevProject::openProject(dirName, projectName);
}

void VSPart::loadVsSolution(const QString &/*sln_path*/) {
}

void VSPart::unloadVsSolution(const QString &/*sln_path*/) {
}

void VSPart::loadVsProject(const QString &/*prj_path*/) {
}

void VSPart::unloadVsProject(const QString &/*prj_path*/) {
}

void VSPart::closeProject() {
//     m_widget->unloadProject ( "" );
}

QString VSPart::projectName() const {
    return m_projectName;
}

void VSPart::slotClean() {
}

void VSPart::restorePartialProjectSession(const QDomElement* /*el*/) {
}

void VSPart::savePartialProjectSession(QDomElement* /*el*/) {
}

// #include "vs_part.moc"
