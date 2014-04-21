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

  bool VSPart::loadVsSolution(const QString &internal_name, const QString &path) {
    QString abspath = m_prjpath+"/"+path;
    QFile sln_f;
    QString ln;
    bool slnguid_found = false;
    QUuid sguid;  // Solution GUID

    kddbg << "Solution internal NAME: " << internal_name << endl;
    kddbg << "Solution PATH: " << path << endl;

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
      kddbg << "Error! Empty line at begin of file detected" << endl;
      ms_guard = str.readLine();
    }

    ms_sharp_guard = str.readLine();

    if(ms_sharp_guard.compare(QString("# Visual Studio 2008")) == 0) {
      sln_ver_expected = 9;
    } else if(ms_sharp_guard.compare(QString("# Visual Studio 2005")) == 0) {
      sln_ver_expected = 8;
    } else {
      sln_ver_expected = 7;
    }

    kddbg << "MS_GUARD: " << ms_guard << endl;
    kddbg << "MS_#GUARD: " << ms_sharp_guard << endl;

    ms_ver = ms_guard.right(5);
    ms_ver.remove(ms_ver.find('.'), 3);
    kddbg << "MS_VER: \"" << ms_ver << "\" expected (" << sln_ver_expected << ")\n";
    bool b_ok = false;
    sln_ver = ms_ver.toInt(&b_ok, 10);

    if(!b_ok) {
      kddbg << "Error! Can't convert version string to int" << endl;
      return false;
    }

    kddbg << "Solution ver: " << sln_ver << endl;
    //END // Read the MS guards and version info

    // Create model::solution item
    VSSolution *sln = new VSSolution(internal_name, abspath);
    if(sln == 0) { kddbg << "Can't parse solution file" << endl; return false; }
    m_entities.append((VSEntity**)&sln);
    // Create and add widget solution item
    VSSlnNode *sln_n = m_explorer_widget->addSolutionNode(sln);
    if(!sln_n) { kddbg << "failed to add solution node" << endl; return false;}

    while(!str.atEnd()) {
      ln = str.readLine();
      //BEGIN // Read project info
      if(0 == ln.left(7).compare(QString("Project"))) {
        kddbg << "Project found\n";
        while(0 != ln.left(10).compare(QString("EndProject"))) {
          kddbg << "Line: " << ln << endl;

          //BEGIN // Read project section info, dependencies
          if(ln.find(QRegExp("ProjectSection"), 0) >= 0) {
            kddbg << "Project section found\n";

            //TODO: Analyze project setion header line
            /** e.g. ProjectSection(ProjectDependencies) = postProject */
            QTextStream s(&ln, IO_ReadOnly);
            QChar ch(0);
            QString psname;  // Section name
            QString pssetting; // Sestion setting
            while(!s.atEnd()) {
              switch(ch.latin1()) {
                case '(': {
                  s >> ch;
                  do {
                    psname.append(ch);
                    s >> ch;
                  } while(ch.latin1() != ')');
                  break; }
                case '=': {
                  s >> pssetting;
                  break; }
                default:
                  s >> ch;
                  break;
              }
            }

            kddbg << "\t\tSection: " << psname << " set: " << pssetting << endl;
            ln = str.readLine();

            while(ln.find(QRegExp("EndProjectSection"), 0) < 0) {
              QTextIStream si(&ln);
              ch = 0;
              QUuid uuid1, uuid2, *uid=&uuid1;

              while(!si.atEnd()) {
                switch(ch.latin1()) {
                  case '{': {
                    if(!readGUID(si, *uid))
                      kddbg << "Failed to obtain GUID !"<< endl;
                    uid=&uuid2;
                    si >> ch;
                    break; }
                  default:
                    si >> ch;
                    break;
                }
              }
              kddbg << "\t\t\t" << psname << ": " << uuid1.toString() << " = " << uuid2.toString() << endl;
              ln = str.readLine();
            }
          }
          //END // Read project section info, dependencies
          //BEGIN // Read and analyze project data
          else {
            // Read project data
            QTextStream prjstream(&ln, IO_ReadOnly);
            bool internaluid_found = false;
            bool prjuid_found = false;
            bool prjname_found = false;
            bool prjrltpath_found = false;
            QUuid puid; //Internal and project UIDs
            QString prjname, prjpath_rlt;
            QChar ch(0);
            char latin1ch;
            /** Analyze project string to get project parameters:
            * Project info inside sln looks like this.
            * Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "testing_stuf", "testing_stuf.vcproj", "{4B448DC1-8FF4-41AC-8734-A655187A84D7}"
            */
            while(!prjstream.atEnd()) {
              switch(ch.latin1()) {
                case '"': {
                  if(!internaluid_found) {
                    prjstream >> ch;
                    if(ch.latin1() != '{') {
                      kddbg << "Error! Can't get GUID, incorrect character" << endl;
                      return false;
                    }
                    if(!readGUID(prjstream, sguid))
                      kddbg << "Failed to obtain internal GUID !"<< endl;
                    kddbg << "\tInternal solution GUID: " << sguid.toString() << endl;
                    if(!slnguid_found) {
                      sln->uidSet(sguid);
                    }
                    internaluid_found = true;
                    prjstream >> ch >> ch >> ch >> ch >> ch;
                  }
                  else if(internaluid_found && !prjname_found) {
                    prjstream >> ch;
                    do {
                      prjname.append(ch);
                      prjstream >> ch;
                    } while(ch.latin1() != '"');
                    kddbg << "\tProject name: " << prjname << endl;
                    prjname_found = true;
                    prjstream >> ch >> ch;
                  }
                  else if(prjname_found && !prjrltpath_found) {
                    prjstream >> ch;
                    do {
                      prjpath_rlt.append(ch);
                      prjstream >> ch;
                    } while(ch.latin1() != '"');
                    kddbg << "\tProject path: " << prjpath_rlt << endl;
                    prjrltpath_found = true;
                    prjstream >> ch >> ch;
                  }
                  else if(!prjuid_found) {
                    prjstream >> ch;
                    if(ch.latin1() != '{') {
                      kddbg << "Error! Can't get GUID, incorrect character" << endl;
                      return false;
                    }
                    if(!readGUID(prjstream, puid))
                      kddbg << "Failed to obtain project GUID !"<< endl;
                    kddbg << "\tProject GUID: " << puid.toString() << endl;
                    prjuid_found = true;
                    prjstream >> ch >> ch;
                  }
                  break;
                }
                default:
                  prjstream >> ch;
                  latin1ch = ch.latin1();
                  break;
              }
            }
            // Create and add model representation
            VSProject *prj = new VSProject(prjname, puid, prjpath_rlt);
            if(prj == 0) { kddbg << "Error! Out of memory space" << endl; return false; }
            sln->insertProj(prj);
            // Create and add widget project item
            m_explorer_widget->addProjectNode(sln_n, prj);
          }
          //END // Read and analyze project data
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

  bool VSPart::saveVsSolution(VSSolution &sln) {
    kddbg << "<<<<<< Saving: " << sln.getName() << " >>>>>>" << endl;
    QString abspath = m_prjpath+"/"+sln.getRelativePath();
    QString prj_layout;
    QFile sln_f;

    if(!sln_f.exists(abspath)) { kddbg << "solution: " << abspath << " will be created from scratch" << endl; }
    sln_f.setName(abspath);
    if(!sln_f.open(IO_WriteOnly|IO_Raw)) { kddbg << "can't open solution file" << endl; return false; }
    if(!sln_f.isWritable()) { kddbg << "is not writable" << endl; return false; }

    QString os;

    os.append("Microsoft Visual Studio Solution File, Format Version 10.00\n");
    os.append("# Visual Studio 2008\n");

    sln.dumpProjectsLayout(prj_layout);
    os.append(prj_layout);

    kddbg << "<<<<<<" << sln.getName() << " saved >>>>>>" << endl;
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
