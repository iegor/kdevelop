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

#include <configwidgetproxy.h>
#include <kdevplugininfo.h>
#include <urlutil.h>

#include <kcomboview.h>

#include "vs_part.h"
#include "vs_model.h"

static const KDevPluginInfo data("kdevpart_vs");

namespace VStudio {
  typedef KDevGenericFactory<VSPart> VSFactory;
  K_EXPORT_COMPONENT_FACTORY(libkdevvs, VSFactory(data))

  VSPart::VSPart(QObject *parent, const char *name, const QStringList &/*args*/)
    : KDevBuildTool(&data, parent, name ? name : "VSPart")
    , selected_sln(0)
    , active_sln(0)
    , active_prj(0) {
    setInstance(VSFactory::instance());
    setXMLFile("kdevpart_vs.rc");

    m_explorer_widget = new VSExplorer(this);
    m_explorer_widget->setIcon(SmallIcon(info()->icon()));
    m_explorer_widget->setCaption(i18n("VS explorer"));
    QWhatsThis::add(m_explorer_widget, i18n("<b>VS explorer</b><p> The tree looks like a standard VS project explorer,"
                                            "yet it has some additiona \"power\" features."));

    mainWindow()->embedSelectView(m_explorer_widget, i18n("VS Explorer"), i18n("VS Explorer, manages vs solution files"));

    // Add solution action
    actAddSolution = new KAction(i18n("Insert solution"), "make_kdevelop", 0, this,
                                 SLOT(slotAddSolution()), actionCollection(), VSPART_ACTION_ADD_SOLUTION);
    actAddSolution->setToolTip(i18n(VSPART_ACTION_ADD_SOLUTION_TIP));
    actAddSolution->setWhatsThis(i18n(VSPART_ACTION_ADD_SOLUTION_WIT));
    actAddSolution->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Build solution action
    actBuildSolution = new KAction(i18n("&Build solution"), "make_kdevelop", VSPART_ACTION_BUILD_SOLUTION_KEY, this,
                                   SLOT(slotBuildSolution()), actionCollection(), VSPART_ACTION_BUILD_SOLUTION);
    actBuildSolution->setToolTip(i18n(VSPART_ACTION_BUILD_SOLUTION_TIP));
    actBuildSolution->setWhatsThis(i18n(VSPART_ACTION_BUILD_SOLUTION_WIT));
    actBuildSolution->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Rebuild solution action
    actRebuildSolution = new KAction(i18n("&Rebuild solution"), "make_kdevelop", 0, this,
                                     SLOT(slotRebuildSolution()), actionCollection(), VSPART_ACTION_REBUILD_SOLUTION);
    actRebuildSolution->setToolTip(i18n(VSPART_ACTION_REBUILD_SOLUTION_TIP));
    actRebuildSolution->setWhatsThis(i18n(VSPART_ACTION_REBUILD_SOLUTION_WIT));
    actRebuildSolution->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Clean solution action
    actCleanSolution = new KAction(i18n("&Clean solution"), "make_kdevelop", 0, this,
                                   SLOT(slotCleanSolution()), actionCollection(), VSPART_ACTION_CLEAN_SOLUTION);
    actCleanSolution->setToolTip(i18n(VSPART_ACTION_CLEAN_SOLUTION_TIP));
    actCleanSolution->setWhatsThis(i18n(VSPART_ACTION_CLEAN_SOLUTION_WIT));
    actCleanSolution->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Add project action
    actAddProject = new KAction(i18n("Insert project"), "make_kdevelop", 0, this,
                                SLOT(slotAddProject()), actionCollection(), VSPART_ACTION_ADD_PROJECT);
    actAddProject->setToolTip(i18n(VSPART_ACTION_ADD_PROJECT_TIP));
    actAddProject->setWhatsThis(i18n(VSPART_ACTION_ADD_PROJECT_WIT));
    actAddProject->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Build project action
    actBuildProject = new KAction(i18n("&Build project"), "make_kdevelop", VSPART_ACTION_BUILD_PROJECT_KEY, this,
                                  SLOT(slotBuildProject()), actionCollection(), VSPART_ACTION_BUILD_PROJECT);
    actBuildProject->setToolTip(i18n(VSPART_ACTION_BUILD_PROJECT_TIP));
    actBuildProject->setWhatsThis(i18n(VSPART_ACTION_BUILD_PROJECT_WIT));
    actBuildProject->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Rebuild project action
    actRebuildProject = new KAction(i18n("&Rebuild project"), "make_kdevelop", 0, this,
                                    SLOT(slotRebuildProject()), actionCollection(), VSPART_ACTION_REBUILD_PROJECT);
    actRebuildProject->setToolTip(i18n(VSPART_ACTION_REBUILD_PROJECT_TIP));
    actRebuildProject->setWhatsThis(i18n(VSPART_ACTION_REBUILD_PROJECT_WIT));
    actRebuildProject->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Clean project action
    actCleanProject = new KAction(i18n("Clean project"), "make_kdevelop", 0, this,
                                  SLOT(slotCleanProject()), actionCollection(), VSPART_ACTION_CLEAN_PROJECT);
    actCleanProject->setToolTip(i18n(VSPART_ACTION_CLEAN_PROJECT_TIP));
    actCleanProject->setWhatsThis(i18n(VSPART_ACTION_CLEAN_PROJECT_WIT));
    actCleanProject->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Add file action
    actAddFile = new KAction(i18n("Add file"), "make_kevelop", 0, this,
                             SLOT(slotAddFile()), actionCollection(), VSPART_ACTION_ADD_FILE);
    actAddFile->setToolTip(VSPART_ACTION_ADD_FILE_TIP);
    actAddFile->setWhatsThis(VSPART_ACTION_ADD_FILE_WIT);
    actAddFile->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Build file action
    actBuildFile = new KAction(i18n("Compile file"), "make_kevelop", 0, this,
                               SLOT(slotAddFile()), actionCollection(), VSPART_ACTION_BUILD_FILE);
    actBuildFile->setToolTip(VSPART_ACTION_BUILD_FILE_TIP);
    actBuildFile->setWhatsThis(VSPART_ACTION_BUILD_FILE_WIT);
    actBuildFile->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Clean file action
    actCleanFile = new KAction(i18n("Clean file"), "make_kevelop", 0, this,
                               SLOT(slotCleanFile()), actionCollection(), VSPART_ACTION_CLEAN_FILE);
    actCleanFile->setToolTip(VSPART_ACTION_CLEAN_FILE_TIP);
    actCleanFile->setWhatsThis(VSPART_ACTION_CLEAN_FILE_WIT);
    actCleanFile->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Add filter action
    actAddFilter = new KAction(i18n("Add filter"), "make_kdevelop", 0, this,
                               SLOT(slotAddFilter()), actionCollection(), VSPART_ACTION_ADD_FILTER);
    actAddFilter->setToolTip(VSPART_ACTION_ADD_FILTER_TIP);
    actAddFilter->setWhatsThis(VSPART_ACTION_ADD_FILTER_WIT);
    actAddFilter->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Build filter action
    actBuildFilter = new KAction(i18n("Build filter"), "make_kdevelop", 0, this,
                                 SLOT(slotAddFilter()), actionCollection(), VSPART_ACTION_BUILD_FILTER);
    actBuildFilter->setToolTip(VSPART_ACTION_BUILD_FILTER_TIP);
    actBuildFilter->setWhatsThis(VSPART_ACTION_BUILD_FILTER_WIT);
    actBuildFilter->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Clean filter action
    actCleanFilter = new KAction(i18n("Clean filter"), "make_kdevelop", 0, this,
                                 SLOT(slotAddFilter()), actionCollection(), VSPART_ACTION_CLEAN_FILTER);
    actCleanFilter->setToolTip(VSPART_ACTION_CLEAN_FILTER_TIP);
    actCleanFilter->setWhatsThis(VSPART_ACTION_CLEAN_FILTER_WIT);
    actCleanFilter->setGroup(VSPART_ACTION_TOOLS_GROUP);

    // Configuration name action
    actConfigName = new KListViewAction(new KComboView(false, 250, 0, "actConfigName"),
                                        i18n("Configuration name"), 0, 0, 0, actionCollection(),
                                        VSPART_ACTION_CONFIGURATION_CFGNAME, true);
    connect(actConfigName->view(), SIGNAL(activated(QListViewItem*)), this, SLOT(slotSelectCfgName(QListViewItem*)));
    // actConfigName->view()->setEditable(false);
    actConfigName->view()->setDuplicatesEnabled(false);
    actConfigName->view()->setAutoCompletion(false);
    // connect(actConfigName->view(), SIGNAL(focusGranted()), navigator, SLOT(functionNavFocused()));
    // connect(actConfigName->view(), SIGNAL(focusLost()), navigator, SLOT(functionNavUnFocused()));
    actConfigName->setToolTip(i18n(VSPART_ACTION_CONFIGURATION_CFGNAME_TIP));
    actConfigName->setWhatsThis(i18n(VSPART_ACTION_CONFIGURATION_CFGNAME_WIT));
    actConfigName->setGroup(VSPART_ACTION_TOOLS_GROUP);
    // actConfigName->view()->setCurrentText(QString("test"));
    // actConfigName->view()->setDefaultText(QString("test: default text"));

    // Configuration platform action
    actConfigPlatform = new KListViewAction(new KComboView(false, 100, 0, "actConfigPlatform"),
                                            i18n("Configuration platform"), 0, 0, 0, actionCollection(),
                                                VSPART_ACTION_CONFIGURATION_CFGPLATFORM, true);
    connect(actConfigPlatform->view(), SIGNAL(activated(QListViewItem*)), this, SLOT(slotSelectCfgPlatform(QListViewItem*)));
    actConfigPlatform->view()->setDuplicatesEnabled(false);
    actConfigPlatform->view()->setAutoCompletion(false);
    actConfigPlatform->setToolTip(i18n(VSPART_ACTION_CONFIGURATION_CFGPLATFORM_TIP));
    actConfigPlatform->setWhatsThis(i18n(VSPART_ACTION_CONFIGURATION_CFGPLATFORM_WIT));
    actConfigPlatform->setGroup(VSPART_ACTION_TOOLS_GROUP);
    // actConfigPlatform->view()->setCurrentText(QString("test"));
    // actConfigPlatform->view()->setDefaultText(QString("test: default text"));

    // connect(buildConfigAction, SIGNAL(activated(const QString&)), this, SLOT(slotBuildConfigChanged(const QString&)));
    // connect(buildConfigAction->popupMenu(), SIGNAL(aboutToShow()), this, SLOT(slotBuildConfigAboutToShow()));

    VSEntity::setPart(this);
  }

  VSPart::~VSPart() {
    selected_sln = 0;
    active_sln = 0;

    // Delete all entities
#ifdef USE_BOOST
    for(ve_ci it=m_entities.begin(); it!=m_entities.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
        delete (*it); /*(*it)=0;*/
      } else {
        kddbg << "Error! vspart's entities list corrupted.\n";
      }
    }
    if(m_explorer_widget) {
      mainWindow()->removeView (m_explorer_widget);
    }
    delete m_explorer_widget;

    VSEntity::setPart(0);
  }

  void VSPart::openProject(const QString &dirName, const QString &projectName) {
    m_prjpath = dirName;
    m_prjname = projectName;

    QDomDocument &dom = *projectDom();

    // Read all solutions and projects parse and setup them
    QDomElement el = DomUtil::elementByPath(dom, VSPART_XML_SECTION_GENERAL);
    QDomElement subEl = el.firstChild().toElement();
    while(!subEl.isNull()) {
      if(subEl.tagName() == VSPART_SOLUTION) {
        QString sln_iname = subEl.attribute("name");
        QString sln_path = subEl.attribute("path");
        QString active_prj = subEl.attribute("active");
        if(loadVsSolution(sln_iname, sln_path)) {
          vss_p sln = static_cast<vss_p>(getSlnByName(sln_iname));
          sln->setActivePrj(active_prj);
        } else {
          kddbg << g_err_slnload.arg(sln_iname);
          subEl = subEl.nextSibling().toElement();
          continue;
        }
      }
      subEl = subEl.nextSibling().toElement();
    }

    // Set active solution
    QString activesln = DomUtil::readEntry(dom, VSPART_XML_SECTION_ACTIVESLN);
    vss_p sln = static_cast<vss_p>(getSlnByName(activesln));
    if(sln == 0) {
      kddbg << g_err_slnactivate.arg(activesln);
    } else {
      activateSln(sln);
      selectSln(sln);
    }

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
    kddbg << VSPART_WARNING"Restore session partially.\n";
  }

  void VSPart::savePartialProjectSession(QDomElement* /*el*/) {
    kddbg << VSPART_WARNING"Saving session partially.\n";
  }

  bool VSPart::loadVsSolution(const QString &internal_name, const QString &path) {
    QString abspath = m_prjpath+"/"+path;
    QFile sln_f;
    QString ln;
    vsp_p prj_active = 0; // Active project to collect dependencies from project section

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
    vss_p sln = new VSSolution(internal_name, abspath);
    if(sln == 0) { kddbg << "Can't parse solution file" << endl; return false; }
#ifdef USE_BOOST
    m_entities.push_back(sln);
#else
    m_entities.append(sln);
#endif

    while(!str.atEnd()) {
      ln = str.readLine();
      //BEGIN // Read project info
      if(0 == ln.left(7).compare(QString("Project"))) {
        // kddbg << " >>>>> Solution item section\n";
        while(0 != ln.left(10).compare(QString("EndProject"))) {
          // kddbg << "Line: " << ln << endl;

          //BEGIN // Read project section info, dependencies
          if(ln.find(QRegExp("ProjectSection"), 0) >= 0) {
            // kddbg << "Project section found\n";

            //TODO: Analyze project setion header line
            /** e.g. ProjectSection(ProjectDependencies) = postProject */
            QTextIStream s(&ln);
            QChar ch(0);
            QString psname;  // Section name
            QString pssetting; // Section setting

            if(!parseSectionHeader(s, psname, pssetting)) {
              kddbg << "Error! Can't parse section header\n";
              return false;
            }
            ln = str.readLine();

            // Read section data
            e_VSPrjSection stype = string2PrjSectionType(psname);
            switch(stype) {
              //BEGIN // ProjectDependencies section
              case prjs_dependencies: {
                while(ln.find(QRegExp("EndProjectSection"), 0) < 0) {
                  QTextIStream si(&ln);
                  ch = 0;
                  QUuid uuid1, uuid2, *uid=&uuid1;

                  while(!si.atEnd()) {
                    switch(ch.latin1()) {
                      case '{': {
                        if(!parseGUID(si, ch, *uid)) return false;
                        uid=&uuid2;
                        si >> ch;
                        break; }
                      default:
                        si >> ch;
                        break;
                    }
                  }
                  // kddbg << "\t\t\t" << psname << ": " << uuid1.toString() << " = " << uuid2.toString() << endl;
                  // prj_active->addDependency((vsp_p)sln->getByUID(uuid2));
                  VSSolution::vsmd_p mdp = sln->metaDependency(prj_active->getUID());
                  if(mdp != 0) {
                    mdp->addDependency(uuid2);
                  }
                  ln = str.readLine();
                }
                break; }
              //END // ProjectDependencies section
              //BEGIN // SolutionItems section
              case prjs_slnitems: {
                while(ln.find(QRegExp("EndProjectSection"), 0) < 0) {
                  kddbg << psname << ": " << ln << endl;
                  ln = str.readLine();
                }
                break; }
              //END // SolutionItems section
              default:
                kddbg << "Error! Unknown section type: " << psname << endl;
                return false;
            }
          }
          //END // Read project section info, dependencies
          //BEGIN // Read and analyze solution unit data
          else {
            // Read solution unit data
            QTextIStream is(&ln);
            bool typeuid_found = false;
            bool prjuid_found = false;
            bool prjname_found = false;
            bool prjrltpath_found = false;
            QUuid puid; //Internal and project UIDs
            e_VSEntityType typ;
            e_VSPrjLangType ltyp;
            QString prjname, prjpath_rlt;
            QChar ch(0);
            vse_p unit(0);
#ifdef DEBUG
            char latin1ch = 0x00;
#endif
            /** Analyze project string to get project parameters:
            * Project info inside sln looks like this.
            * Project("TYPE_GUID") = "internal name" , "rel path", "PRJ_GUID"
            */
            while(!is.atEnd()) {
              switch(ch.latin1()) {
                case '"': {
                  if(!typeuid_found) {
                    QUuid tuid;
                    is >> ch;
                    if(!parseGUID(is, ch, tuid)) return false;
                    typeuid_found = true;
                    is >> ch >> ch >> ch >> ch;
                    typ = uid2VSType(tuid);
                    ltyp = uid2PrjLangType(tuid);
                  }
                  else if(typeuid_found && !prjname_found) {
                    is >> ch;
                    do {
                      prjname.append(ch);
                      is >> ch;
                    } while(ch.latin1() != '"');
                    prjname_found = true;
                    is >> ch >> ch;
                  }
                  else if(prjname_found && !prjrltpath_found) {
                    is >> ch;
                    do {
                      prjpath_rlt.append(ch);
                      is >> ch;
                    } while(ch.latin1() != '"');
                    prjrltpath_found = true;
                    is >> ch >> ch;
                  }
                  else if(!prjuid_found) {
                    is >> ch;
                    if(!parseGUID(is, ch, puid)) return false;
                    prjuid_found = true;
                    is >> ch;
                  }
                  break;
                }
                default:
                  is >> ch;
#ifdef DEBUG
                  latin1ch = ch.latin1();
#endif
                  break;
              }
            }

            switch(typ) {
              case vs_project: {
                /* kddbg << "Project [" << prjLangType2String(ltyp) << "] "
                    << puid.toString() << " \"" << prjname << "\" under: \""
                    << prjpath_rlt << "\"\n";
                */
                switch(ltyp) {
                  case vs_prjlang_c: {
                    // Create and add model representation
                    unit = new VSProject_c(prjname, puid, prjpath_rlt);
                    break; }
                  case vs_prjlang_cs: {
                    kddbg << "VS Project for C# \"" << guid2String(puid) <<
                        "\" is not supported\n";
                    ln = str.readLine();
                    continue; }
                  default:
                    kddbg << "Error! " << type2String(typ) << " \"" << prjname
                        << "\": language [" << prjLangType2String(ltyp) <<
                        "] support is not implemented\n";
                    ln = str.readLine();
                    continue; //NOTE: Just skip this unknown project
                }
                if(unit==0) {
                  kddbg << "Error! Out of memory space" << endl;
                  return false; }
                // Set most recent "active" project ptr
                prj_active = static_cast<vsp_p>(unit);
                break; }
              case vs_filter: {
                /* kddbg << "Filter " << puid.toString() << " \"" << prjname
                    << "\" under: \"" << prjpath_rlt << "\"\n";
                */
                // Create and add model representation
                unit = new VSFilter(prjname, puid);
                if(unit == 0) {
                  kddbg << "Error! Out of memory space" << endl;
                  return false; }
                break; }
              default:
                kddbg << "Error!!! Uncompatible solution unit type \""
                    << type2String(typ) << "\" is requested\n";
                return false;
            }
            sln->insert(unit);
          }
          //END // Read and analyze solution unit data
          ln = str.readLine();
        }
      }
      //END // Read project info
      //BEGIN // Read global solution sections
      else if(0 == ln.compare("Global")) {
        kddbg << "Entering global settings section" << endl;
        while(ln.find(QRegExp("EndGlobal"), 0) < 0) {
          kddbg << "Line: " << ln << endl;
          if(ln.find(QRegExp("GlobalSection"), 0) >= 0) {

            kddbg << "(G)Line: " << ln << endl;
            QTextIStream si(&ln);
            QString sname; // Section name
            QString sparam; // Section parameter

            if(!parseSectionHeader(si, sname, sparam)) {
              kddbg << "Error! Can't parse section header\n";
              return false;
            }
            ln = str.readLine();

            // Read section data
            e_VSSlnSection stype = string2SlnSectionType(sname);
            switch(stype) {
              //BEGIN // SolutionConfigurationPlatforms
              case slns_sln_cfgplatforms: {
                // ln = str.readLine();
                while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) {
#ifdef DEBUG
                  kddbg << sname << ": " << ln << endl;
#endif
                  QString conf_name;
                  QString c_internal_name;
                  QRegExp rx("^\t\t(.+\|\w+)\ \=\ (.+\|\w+)$");
                  if(-1 != rx.search(ln)) {
                    conf_name = rx.cap(1);
                    c_internal_name = rx.cap(2);
                  }
#ifdef DEBUG
                  kddbg << "Configuration: " << conf_name << " presented as: "
                      << c_internal_name << endl;
#endif
                  // Create and add configuration
                  sln->addConfiguration(conf_name);
                  ln = str.readLine();
                }
                break; }
              //END // SolutionConfigurationPlatforms
              //BEGIN // ProjectConfigurationPlaftorms
              case slns_prj_cfgplatforms: {
                while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) {
                  // kddbg << sname << ": " << ln << endl;
                  ln = str.readLine();
                }
                break; }
              //END // ProjectConfigurationPlaftorms
              //BEGIN // SolutionProperties
              case slns_sln_properties: {
                while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) {
                  // kddbg << sname << ": " << ln << endl;
                  ln = str.readLine();
                }
                break; }
              //END // SolutionProperties
              //BEGIN // NestedProjects
              case slns_nested_prjs: {
                while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) {
                  // kddbg << sname << ": " << ln << endl;
                  QTextIStream si(&ln);
                  QChar ch(0);
                  QUuid uid1, uid2, *uid=&uid1;

                  while(!si.atEnd()) {
                    switch(ch.latin1()) {
                      case '{': {
                        if(!parseGUID(si, ch, *uid)) return false;
                        uid=&uid2;
                        si >> ch;
                        break; }
                      default:
                        si >> ch;
                        break;
                    }
                  }
                  //Get filer and project model representation
                  vse_p ent = sln->getByUID(uid1); // get entity
                  if(ent == 0) {
                    ent = sln->getFltByUID(uid1);
                    if(ent == 0) {
#ifdef DEBUG
                      kddbg << " >>>> Failed to obtain entity for " << uid1.toString() << endl;
#endif
                      ln = str.readLine();
                      continue;
                    }
                  }
                  vsf_p cnt = sln->getFltByUID(uid2); // get container
                  if(cnt == 0) {
#ifdef DEBUG
                    kddbg << " >>>> Failed to obtain container for " << uid2.toString() << endl;
#endif
                    ln = str.readLine();
                    continue;
                  }
#ifdef DEBUG
                  /* kddbg << type2String(cnt->getType()) << " \"" << cnt->getName()
                      << "\" <<< " << type2String(ent->getType()) << " \""
                      << ent->getName() << "\"\n"; */
#endif
                  cnt->insert(ent);
                  ln = str.readLine();
                }
                break; }
              //END // NestedProjects
              default:
                kddbg << "Unknown section type: " << sname << endl;
                return false;
            }
            // ln = str.readLine();
          }
          ln = str.readLine();
        }
      }
      //END // Read global solution sections
    }

    sln_f.close();
    kddbg << "<<<<< Parsing FINISHED >>>>>" << endl;

    if(!sln->updateDependencies()) {
      kddbg << "Error! Failed to update dependencies.\n";
    }
    // Create UI representation
    return sln->populateUI();
    // return true;
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

  vse_p VSPart::getByUID(const QUuid &uid) const {
    /*
#ifdef USE_BOOST
    for(ve_ci it=m_entities.begin(); it!=m_entities.end(); ++it) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
         if((*it)->getUID() == uid) { return (*it); }
      } else {
        kddbg << VSPART_ERR_ENTITYLIST_CORRUPTED;
        return 0;
      }
    }
    return 0;
    */
    return 0;
  }

  vss_p VSPart::getSlnByName(const QString &n) {
    if(n != QString::null) {
#ifdef USE_BOOST
      ve_ci it=m_entities.begin();
      for(; it!=m_entities.end(); ++it) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
        if((*it) != 0) {
          if((*it)->getType() == vs_solution) {
            vss_p sln = static_cast<vss_p>(*it);
            if(sln->getName() == n) { break; }
          }
        } else {
#ifdef DEBUG
          kddbg << VSPART_ERR_ENTITYLIST_CORRUPTED;
#endif
          return 0;
        }
      }
#ifdef USE_BOOST
      if(it!=m_entities.end()) { return static_cast<vss_p>(*it); }
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
#ifdef DEBUG
      else { kddbg << g_err_ent_notfound.arg(type2String(vs_solution)).arg(n); }
#endif
    }
    return 0;
  }

  bool VSPart::selectSln(vss_p s) {
    if(s != 0 && s->getType() == vs_solution) {
      selected_sln = s;
#ifdef DEBUG
      kddbg << g_msg_slnselect.arg(s->getName());
#endif
      // Update configuration selection combos
      actConfigName->view()->clear();
      actConfigPlatform->view()->clear();
#ifdef USE_BOOST
      pv_QString names;
      pv_QString plfms;
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif

      // Setup configurations for active solution
#ifdef USE_BOOST
      for(vcfg_ci it=selected_sln->vcfg().begin(); it!=selected_sln->vcfg().end(); ++it) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif

        // Parse through configuration names and insert new one, if it isn't there
#ifdef USE_BOOST
        qstr_ci ciit=names.begin();
        for(; ciit!=names.end(); ++ciit) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
          if((*ciit) == (*it)->getName()) { break; }
        }
        if(ciit == names.end()) {
          names.push_back((*it)->getName());
          QListViewItem *i = new QListViewItem(actConfigName->view()->listView(), (*it)->getName());
          i->setPixmap(0, SmallIcon("gear"));
          actConfigName->view()->addItem(i);
        }

        // Parse through platforms and insert new one, if it isn't there yet
#ifdef USE_BOOST
        qstr_ci pciit=plfms.begin();
        for(; pciit!=plfms.end(); ++pciit) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
          if((*pciit) == platform2String((*it)->platform())) { break; }
        }
        if(pciit == plfms.end()) {
          plfms.push_back(platform2String((*it)->platform()));
          QListViewItem *i = new QListViewItem(actConfigPlatform->view()->listView(), platform2String((*it)->platform()));
          i->setPixmap(0, SmallIcon("gear"));
          actConfigPlatform->view()->addItem(i);
        }
      }
      //Set latest configuration
      //NOTE: That is for now only
      //TODO: Make configuration selection depend on saved settings
      //  NOTE: Part of meta-information for solutions and projects
      actConfigName->view()->setCurrentText(names[0]);
      actConfigPlatform->view()->setCurrentText(plfms[0]);
      selected_sln->setConfiguration(names[0], plfms[0]);
      return true;
    }

    return false;
  }

  /*inline*/ vss_p VSPart::getSelectedSln() const {
    return selected_sln;
  }

  bool VSPart::activateSln(vss_p s) {
    if(s != 0) {
      if(active_sln != s) {
        if(active_sln != 0) {
          active_sln->setActive(false);
        }
        active_sln = s;
        active_sln->setActive(true);
      }
      return true;
    }
#ifdef DEBUG
    else { kddbg << g_err_nullptr.arg("VSPart::activateSln"); }
#endif
    return false;
  }

  /*inline*/ vss_p VSPart::getActiveSln() const {
    return active_sln;
  }

  bool VSPart::activatePrj(vsp_p p) {
    if(p != 0) {
      vss_p sln = static_cast<vss_p>(p->getParent());
      // Signal parent sln to activate project p and remember the choise
      if(active_prj != p) {
        active_prj = p;
        sln->setActivePrj(active_prj);
      }
      // Activate parent sln for active project if not active
      if(sln != active_sln) {
        if(activateSln(sln)) {
          return true;
        } else {
          kddbg << VSPART_WARNING"Failed to activate parent sln.\n";
        }
      }
      return true;
    }
#ifdef DEBUG
    else { kddbg << g_err_nullptr.arg("VSPart::activatePrj"); }
#endif
    return false;
  }

  /*inline*/ vsp_p VSPart::getActivePrj() const {
    return active_prj;
  }

  bool VSPart::saveSln(vss_p sln) {
    if(sln != 0) {
      QString abspath = sln->getRelPath();
      abspath.append(".test");
      QString str;
      QFile sln_f(abspath);
      // if(!sln_f.exists(abspath)) { kddbg << "solution: " << abspath << " will be created from scratch" << endl; }
      sln_f.setName(abspath);
      if(!sln_f.open(IO_WriteOnly|IO_Raw)) {
        kddbg << "can't open solution file: " << abspath << "\n";
        return false;
      }
      if(!sln_f.isWritable()) {
        kddbg << "is not writable" << endl;
        return false;
      }

      QTextStream s(&sln_f);
      kddbg << "<<<<<< Saving: " << sln->getName() << " >>>>>>" << endl;

      sln->dumpLayout(s);

      sln_f.flush();
      sln_f.close();

      // printf("%s\n", str.ascii()); //TEST: only stdout

      kddbg << "<<<<<<" << sln->getName() << " saved >>>>>>" << endl;
      return true;
    }
    return false;
  }

  bool VSPart::saveSlnAs(vss_p s, const QString& path) {
    kddbg << "SLOT: VSPart::slotSaveSlnAs \"" << path << "\".\n";
    return false;
  }

  bool VSPart::parseSectionHeader(QTextIStream &s, QString &nm, QString &prm) {
    QChar c(0);
#ifdef DEBUG
    char latin1c(0);
#endif
    while(!s.atEnd()) {
      switch(c.latin1()) {
        case '(': {
          s >> c;
          do {
            nm.append(c);
            s >> c;
          } while(c.latin1() != ')');
          break; }
          case '=': {
            s >> prm;
            break; }
        default:
          s >> c;
#ifdef DEBUG
          latin1c = c.latin1();
#endif
          break;
      }
    }
    // kddbg << "| Section: " << nm << " set: " << prm << " |\n";
    return true;
  }

  bool VSPart::parseGUID(QTextIStream &s, QChar &ch, QUuid &uid) {
    if(ch.latin1() != '{') {
      kddbg << "Error! Can't get GUID, incorrect uid string format, expect {XXXXX...XXXX} format" << endl;
      return false;
    }
    if(!readGUID(s, uid)) {
      kddbg << "Error! Failed to obtain GUID"<< endl;
      return false;
    }
    s >> ch;
    return true;
  }

  //BEGIN // Slot methods
  void VSPart::slotAddSolution() {
    kddbg << "slotAddSolution test" << endl;
  }

  void VSPart::slotBuildSolution() {
    kddbg << "slotBuildSolution test" << endl;
  }

  void VSPart::slotRebuildSolution() {
    kddbg << "slotRebuildSolution test" << endl;
  }

  void VSPart::slotCleanSolution() {
    kddbg << "slotCleanSolution test" << endl;
  }

  void VSPart::slotAddProject() {
    kddbg << "slotAddProject test" << endl;
  }

  void VSPart::slotBuildProject() {
    kddbg << "slotBuildProject test" << endl;
  }

  void VSPart::slotRebuildProject() {
    kddbg << "slotRebuildProject test" << endl;
  }

  void VSPart::slotCleanProject() {
    kddbg << "slotCleanProject test" << endl;
  }

  void VSPart::slotAddFile() {
    kddbg << "slotAddFile test" << endl;
  }

  void VSPart::slotBuildFile() {
    kddbg << "slotBuildFile test" << endl;
  }

  void VSPart::slotCleanFile() {
    kddbg << "slotCleanFile test" << endl;
  }

  void VSPart::slotAddFilter() {
    kddbg << "slotAddFilter test" << endl;
  }

  void VSPart::slotBuildFilter() {
    kddbg << "slotBuildFilter test" << endl;
  }

  void VSPart::slotCleanFilter() {
    kddbg << "slotCleanFilter test" << endl;
  }

  void VSPart::slotSelectCfgName(QListViewItem *i) {
    selected_sln->setConfiguration(i->text(0), actConfigPlatform->view()->currentText());
  }

  void VSPart::slotSelectCfgPlatform(QListViewItem *i) {
    selected_sln->setConfiguration(actConfigName->view()->currentText(), i->text(0));
  }
  //END // Slot methods
};
#include "vs_part.moc"
