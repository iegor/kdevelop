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

//#include <boost/container/vector.hpp>

#include "vs_part.h"
#include "vs_model.h"

static const KDevPluginInfo data("kdevpart_vs");

namespace VStudio {
  typedef KDevGenericFactory<VSPart> VSFactory;
  K_EXPORT_COMPONENT_FACTORY(libkdevvs, VSFactory(data))

  VSPart::VSPart(QObject *parent, const char *name, const QStringList &/*args*/)
    : KDevBuildTool(&data, parent, name ? name : "VSPart") {
    KAction *action;
    setInstance(VSFactory::instance());
    setXMLFile("kdevpart_vs.rc");

    m_explorer_widget = new VSExplorer(this);
    m_explorer_widget->setIcon(SmallIcon(info()->icon()));
    m_explorer_widget->setCaption(i18n("VS explorer"));
    QWhatsThis::add(m_explorer_widget, i18n("<b>VS explorer</b><p> The tree looks like a standard VS project explorer,"
                                            "yet it has some additiona \"power\" features."));

    mainWindow()->embedSelectView(m_explorer_widget, i18n("VS Explorer"),
                                  i18n("VS Explorer, manages vs solution files"));

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

  //   connect(buildConfigAction, SIGNAL(activated(const QString&)), this, SLOT(slotBuildConfigChanged(const QString&)));
  //   connect(buildConfigAction->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(slotBuildConfigAboutToShow()));
  }

  VSPart::~VSPart() {
    if(m_explorer_widget) {
      mainWindow()->removeView (m_explorer_widget);
    }
    delete m_explorer_widget;
  }

  void VSPart::openProject(const QString &dirName, const QString &projectName) {
    m_prjpath = dirName;
    m_prjname = projectName;

    QDomDocument &dom = *projectDom();
  //     QStringList vsitms;
  //     vsitms = DomUtil::readEntry(dom, "kdevvstudioproject/general");

    // Read all solutions and projects parse and setup them
    DomUtil::PairList vsitems = DomUtil::readPairListEntry(dom,
        "kdevvstudioproject/general", VSPART_SOLUTION_ITEM, "name", "path");
    DomUtil::PairList::ConstIterator it;
    for(it = vsitems.begin(); it != vsitems.end(); ++it) {
      if(!loadVsSolution((*it).first, (*it).second)) {
        continue; } }

    KDevProject::openProject(dirName, projectName);
  }

  void VSPart::closeProject() {
  }

  QString VSPart::projectName() const {
    return m_projectName;
  }

  QString VSPart::projectDirectory() const {
    return m_projectPath;
  }

  DomUtil::PairList VSPart::runEnvironmentVars() const {
  }

  QString VSPart::mainProgram() const {
    return "";
  }

  QString VSPart::runDirectory() const {
    return "";
  }

  QString VSPart::runArguments() const {
    return "";
  }

  QString VSPart::debugArguments() const {
    return "";
  }

  QString VSPart::activeDirectory() const {
    return "";
  }

  QString VSPart::buildDirectory() const {
    return "";
  }

  QStringList VSPart::allFiles() const {
    return "";
  }

  QStringList VSPart::distFiles() const {
    return "";
  }

  void VSPart::addFile(const QString &fileName) {
  }

  void VSPart::addFiles(const QStringList& fileList) {
  }

  void VSPart::removeFile(const QString &fileName) {
  }

  void VSPart::removeFiles(const QStringList& fileList) {
  }

  KDevProject::Options VSPart::options() const {
    return UsesOtherBuildSystem;
  }

  void VSPart::restorePartialProjectSession(const QDomElement* /*el*/) {
  }

  void VSPart::savePartialProjectSession(QDomElement* /*el*/) {
  }

  bool VSPart::loadVsSolution(const QString &name, const QString &path) {
    QString abspath = m_prjpath+"/"+path;
    QFile sln_f;
    QString ln;

    kddbg << "[VS SOLUTION]: |" << name << "| in " << path << endl;

    if(!sln_f.exists(abspath)) { kddbg << "is not there" << endl; return false; }
    sln_f.setName(abspath);
    if(!sln_f.open(IO_ReadWrite|IO_Raw)) { kddbg << "can't open solution file" << endl; return false; }
    if(!sln_f.isReadable()) { kddbg << "is not readable" << endl; return false; }
    if(!sln_f.isWritable()) { kddbg << "is not writable" << endl; return false; }
    if(!sln_f.isReadWrite()) { kddbg << "can't read and write" << endl; return false; }

    kddbg << "<<<<< Parsing solution file >>>>>" << endl;

    QTextStream str(&sln_f);

    //BEGIN // Read the MS guards and version info
    QString ms_guard, ms_sharp_guard, ms_ver;
    int sln_ver = 0;
    int sln_ver_expected = 0;

    ms_guard = str.readLine();
    if(ms_guard.isEmpty()) {
      kddbg << "empty line at begin of file detected" << endl;
      ms_guard = str.readLine(); }

    ms_sharp_guard = str.readLine();

    if(ms_sharp_guard.compare(QString("# Visual Studio 2008")) == 0) {
      sln_ver_expected = 10;
    } else if(ms_sharp_guard.compare(QString("# Visual Studio 2005")) == 0) {
      sln_ver_expected = 9;
    } else {
      sln_ver_expected = 8;
    }

    printf("MS_GUARD: %s\n", ms_guard.ascii());
    printf("MS_#GUARD: %s\n", ms_sharp_guard.ascii());

    ms_ver = ms_guard.right(5);
    ms_ver.remove(ms_ver.find('.'), 3);
    printf("ms_ver: \"%s\"\n", ms_ver.ascii());
    bool b_ok = false;
    sln_ver = ms_ver.toInt(&b_ok, 10);

    if(!b_ok) {
      kddbg << "can't convert version string to int" << endl;
      return false;
    }

    printf("Solution ver: %i\n", sln_ver);
    //END // Read the MS guards and version info

    // Create model::solution item
    VSSolution *sln = new VSSolution(name, abspath);
    if(sln == 0) { kddbg << "Can't parse solution file" << endl; return false; }
    m_entities.append((VSEntity**)&sln);

    // Create and add widget solution item
    if(!m_explorer_widget->addSolutionNode(name)) { kddbg << "failed to add solution node" << endl; return false;}

    while(!str.atEnd()) {
      ln = str.readLine();
      //BEGIN // Read project info
      if(0 == ln.left(7).compare(QString("Project"))) {
        while(0 != ln.left(10).compare(QString("EndProject"))) {

          //BEGIN // Read project section info, dependencies
          if(0 == ln.left(15).compare(QString("ProjectSection"))) {
            while(0 != ln.left(17).compare(QString("EndProjectSection"))) {
              printf("Project section data: %s\n", ln.ascii());
              ln = str.readLine();
            }
          }
          //END // Read project section info, dependencies

          // Read project data
          /**
          * E.G. project info inside sln looks like this.
          * Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "testing_stuf", "testing_stuf.vcproj", "{4B448DC1-8FF4-41AC-8734-A655187A84D7}"
          * So we need to split it into these pieces:
          *
          * - "Project" string
          *
          * - ("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}")
          *   solution internal UUID of project
          *
          * - "project name" internal for solution
          *
          */
          QTextStream prjstream(&ln, IO_ReadOnly);
          QString token;
          prjstream >> token;
          kddbg << "Token: " << token << endl;

          // Get the project GUID
          QRegExp rx("[A-F0-9]{8}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{12}");
          int irx = rx.search(token);
          QString prjname;
          QString prjguid = token.mid(irx, 36);
          QUuid prj_uid(prjguid);

          kddbg << "Project \"" << prjname << "data:\n";
          printf("\tGUID: %X\n", prj_uid.data1);

          // Create and add widget project item
          //m_explorer_widget->addProjectNode("test_prj");
          ln = str.readLine();
        }
      }
      //END // Read project info
      else if(0 == ln.compare("Global")) {
      }
    }

    kddbg << "<<<<< Parsing FINISHED >>>>>" << endl;
    return true;
  }

  bool VSPart::unloadVsSolution(const QString &/*sln_path*/) {
    return true;
  }

  bool VSPart::loadVsProject(const QString &/*prj_path*/) {
    return true;
  }

  bool VSPart::unloadVsProject(const QString &/*prj_path*/) {
    return true;
  }

  void VSPart::slotClean() {
  }

  void VSPart::slotCompileFile() {
  }

  void VSPart::slotBuild() {
  }
};
#include "vs_part.moc"
