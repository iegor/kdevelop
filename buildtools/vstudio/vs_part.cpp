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

/* Qt */
#include <qdom.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qwhatsthis.h>
#include <qgroupbox.h>

/* KDE */
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
#include <kurl.h>

/* KDevelop */
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

/* VStudio */
#include "vs_part.h"
#include "vs_model.h"

static const KDevPluginInfo data("kdevpart_vs");

namespace VStudio {
  typedef KDevGenericFactory<VSPart> VSFactory;
  K_EXPORT_COMPONENT_FACTORY(libkdevvs, VSFactory(data))

  VSPart::VSPart(QObject *parent, const char *name, const QStringList &/*args*/)
    : KDevBuildTool(&data, parent, name ? name : "VSPart")
    , selected_sln(0)
    , selected_prj(0)
    , selected_file(0)
    , active_sln(0)
    , active_prj(0)
    , active_cfg(0) {
    setInstance(VSFactory::instance());
    setXMLFile("kdevpart_vs.rc");

    m_explorer_widget = new VSExplorer(this);
    m_explorer_widget->setIcon(SmallIcon(info()->icon()));
    m_explorer_widget->setCaption(i18n("VS explorer"));
    QWhatsThis::add(m_explorer_widget, i18n("<b>VS explorer</b><p> The tree looks like a standard VS project explorer,"
                                            "yet it has some additiona \"power\" features."));

    mainWindow()->embedSelectView(m_explorer_widget, i18n("VS Explorer"), i18n("VS Explorer, manages vs solution files"));

    // Variables view widget
    mVViewWidget = new VSViewVars(this, 0, "variables_view");
    mVViewWidget->setIcon(SmallIcon("gear"));
    mVViewWidget->setCaption(i18n("VSview: variables"));
    QWhatsThis::add(mVViewWidget, i18n("Shows varialbes that are used throughout entire dev process"));
    mainWindow()->embedOutputView(mVViewWidget, "VSVars", i18n("VSView: variables"));

    // Add solution action
    actAddSolution = new KAction(i18n("Insert solution"), "fileimport.png", 0, this,
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

    // Create configuration action
    actCreateConfig = new KAction(i18n("Create Configuration"), "gear", 0, this,
                                 SLOT(slotCreateConfig()), actionCollection(), VSPART_ACTION_ADD_CONFIG);
    actCreateConfig->setToolTip(VSPART_ACTION_ADD_CONFIG_TIP);
    actCreateConfig->setWhatsThis(VSPART_ACTION_ADD_CONFIG_WIT);
    actCreateConfig->setGroup(VSPART_ACTION_TOOLS_GROUP);

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

    connect(this, SIGNAL(uisync()), m_explorer_widget, SLOT(slotRefreshUI()));

    // Init variables
    m_variables[VSPART_V_CFGNAME] = QString::null;
    m_variables[VSPART_V_DNVDIR] = QString::null;
    m_variables[VSPART_V_FRWDIR] = QString::null;
    m_variables[VSPART_V_FRWSDKDIR] = QString::null;
    m_variables[VSPART_V_FRWVER] = QString::null;
    m_variables[VSPART_V_FXCOPDIR] = QString::null;
    m_variables[VSPART_V_INPDIR] = QString::null;
    m_variables[VSPART_V_INPEXT] = QString::null;
    m_variables[VSPART_V_INPFLN] = QString::null;
    m_variables[VSPART_V_INPNAME] = QString::null;
    m_variables[VSPART_V_INPPATH] = QString::null;
    m_variables[VSPART_V_INTDIR] = QString::null;
    m_variables[VSPART_V_OUTDIR] = QString::null;
    m_variables[VSPART_V_PNTNAME] = QString::null;
    m_variables[VSPART_V_PLATFNAME] = QString::null;
    m_variables[VSPART_V_PRJDIR] = QString::null;
    m_variables[VSPART_V_PRJEXT] = QString::null;
    m_variables[VSPART_V_PRJFLN] = QString::null;
    m_variables[VSPART_V_PRJNAME] = QString::null;
    m_variables[VSPART_V_PRJPATH] = QString::null;
    m_variables[VSPART_V_REFERENCES] = QString::null;
    m_variables[VSPART_V_REMOTEM] = QString::null;
    m_variables[VSPART_V_ROOTNS] = QString::null;
    m_variables[VSPART_V_INPNAME_S] = QString::null;
    m_variables[VSPART_V_PNTNAME_S] = QString::null;
    m_variables[VSPART_V_ROOTNS_S] = QString::null;
    m_variables[VSPART_V_SLNDIR] = QString::null;
    m_variables[VSPART_V_SLNEXT] = QString::null;
    m_variables[VSPART_V_SLNFLN] = QString::null;
    m_variables[VSPART_V_SLNNAME] = QString::null;
    m_variables[VSPART_V_SLNPATH] = QString::null;
    m_variables[VSPART_V_TGTDIR] = QString::null;
    m_variables[VSPART_V_TGTEXT] = QString::null;
    m_variables[VSPART_V_TGTFLN] = QString::null;
    m_variables[VSPART_V_TGTFRW] = QString::null;
    m_variables[VSPART_V_TGTNAME] = QString::null;
    m_variables[VSPART_V_TGTPATH] = QString::null;
    m_variables[VSPART_V_VCINSTDIR] = QString::null;
    m_variables[VSPART_V_VSINSTDIR] = QString::null;
    m_variables[VSPART_V_WEBDEPLPATH] = QString::null;
    m_variables[VSPART_V_WEBDEPLROOT] = QString::null;
    m_variables[VSPART_V_WINSDKDIR] = QString::null;
    m_variables[VSPART_V_WINSDKDIR_IA64] = QString::null;

    mVViewWidget->slotRefreshValues();

    // Set part ptr in the model for reference
    VSEntity::setPart(this);
  }

  VSPart::~VSPart() {
    selected_sln = 0;
    active_sln = 0;

    // Delete all configurations
    BOOSTVEC_FOR(vcfg_ci, it, m_configs) {
      delete (*it); /*(*it)=0;*/
    }

    // Delete all entities
    BOOSTVEC_FOR(vse_ci, it, m_entities) {
      if((*it)!=0) {
        delete (*it); /*(*it)=0;*/
      } else {
        kddbg << g_err_list_corrupted.arg("vs_entity").arg("VSPart::~VSPart");
      }
    }
    if(m_explorer_widget) {
      mainWindow()->removeView (m_explorer_widget);
    }
    delete m_explorer_widget;

    VSEntity::setPart(0);
  }

  void VSPart::openProject(const QString &dirName, const QString &projectName) {
    m_projectPath = dirName;
    m_projectName = projectName;

    QDomDocument &dom = *projectDom();
    QDomElement general = DomUtil::elementByPath(dom, VSPART_XML_SECTION_GENERAL);

    // Read all project configs along with sln configs
    QDomElement subEl = general.firstChild().toElement();
    while(!subEl.isNull()) {
      if(subEl.tagName() == VSPART_CONFIG) {
        QString name = subEl.attribute("name");
        QString platform = subEl.attribute("platform");

        if(!verifyPlatform(platform)) {
          kddbg << g_err_unsupportedplatform.arg(platform);
          return;
        }

        if(createCfg(name, string2Platform(platform), false, false)) {
          kddbg << "Config added: " << name << "|" << platform << endl;
        }
        else {
          kddbg << VSPART_ERROR"Failed to add config: " << name << "|" << platform
              << " into kdevelop project.\n";
          return;
        }
      }
      subEl = subEl.nextSibling().toElement();
    }

    // Read all solutions and projects parse and setup them
    subEl = general.firstChild().toElement();
    while(!subEl.isNull()) {
      if(subEl.tagName() == VSPART_SOLUTION) {
        QString sln_iname = subEl.attribute("name");
        QString sln_path = subEl.attribute("path");
        QString active_prj = subEl.attribute("active");

        // Check attributes
        if(sln_iname.isEmpty()) {
          kddbg << VSPART_ERROR"Sln name is empty.\n";
          subEl = subEl.nextSibling().toElement();
          continue;
        }
        if(sln_path.isEmpty()) {
          kddbg << VSPART_ERROR"Sln: " << sln_iname << " relative path is empty.\n";
          subEl = subEl.nextSibling().toElement();
          continue;
        }

        // Create model::solution item
        vss_p sln = new VSSolution(sln_iname);
        if(sln == 0) {
          kddbg << g_err_notenoughmem.arg(VSPART_SOLUTION).arg("VSPart::openProject");
          return;
        }

        KURL url(KURL::fromPathOrURL(m_projectPath));
        kddbg << url.url() << endl;
        url.addPath(sln_path);
        sln->setPath(url.pathOrURL());

        //NOTE: We adding "incolmplete" sln to be sure we will save all solutions that were
        //  stored in .kdevelop file
#ifdef USE_BOOST
        m_entities.push_back(sln);
#else
        m_entities.append(sln);
#endif

        // Read and parse sln file
        if(sln->read()) {
          if(!active_prj.isEmpty()) {
            if(!sln->setActivePrj(active_prj)) {
              kddbg << VSPART_ERROR"Can't activate a prj: " << active_prj << " in sln: " << sln->getName() << endl;
            }
          }
          else { kddbg << VSPART_WARNING"Sln: " << sln_iname << " no active project.\n"; }
        } else {
          kddbg << g_err_slnload.arg(sln_iname);
          // subEl = subEl.nextSibling().toElement();
          // continue;
        }

        // Generate dependencies  from meta-dependency information
        if(!sln->updateDependencies()) {
          kddbg << VSPART_ERROR"Can't gen dependencies from meta-dependencies.\n";
        }

        // Create UI representation
        if(!sln->populateUI()) {
          kddbg << VSPART_ERROR"Can't use sln. UI update failed.\n";
        }
      }
      subEl = subEl.nextSibling().toElement();
    }

    // Re-read configurations for project and set parent configs for sln child configs
    subEl = general.firstChild().toElement();
    while(!subEl.isNull()) {
      if(subEl.tagName() == VSPART_CONFIG) {
        QString name = subEl.attribute("name");
        QString platform = subEl.attribute("platform");

        // Find project config <Name|Platform>
        vcfg_p cfg = getCfg(QString(name).append("|%1").arg(platform));

        if(cfg != 0) {
          QDomElement sln_config = subEl.firstChild().toElement();
          while(!sln_config.isNull()) {
            if(sln_config.tagName() == VSPART_SOLUTION) {
              QString sln_name = sln_config.attribute("name");
              QString sln_cfg_name = sln_config.attribute("config");
              QString sln_cfg_platform = sln_config.attribute("platform");
              bool is_ok=false;
              int is_enabled = sln_config.attribute("enabled").toInt(&is_ok, 10);

              if(is_ok) {
                vss_p sln = static_cast<vss_p>(getSlnByName(sln_name));
                if(sln != 0) {
                  vsbb_p slnbb = sln->getBB(QString(sln_cfg_name).append("|%1").arg(sln_cfg_platform));
                  if(slnbb != 0) {
#ifdef DEBUG
                    kddbg << "KPROJ[ " << cfg->toString() << " ] <<< SLN[ " << sln->getName()
                        << " ]:[ " << slnbb->config().toString() << " ].\n";
#endif
                    slnbb->setParentCfg(cfg);
                    slnbb->setEnabled(is_enabled);
                  }
                }
              } else {
                kddbg << VSPART_ERROR"Can't read \"enabled\" param in configuration.\n";
                return;
              }
            } else {
              kddbg << g_wrn_unsupportedtyp.arg(sln_config.tagName()).arg("VSPart::openProject");
              sln_config = sln_config.nextSibling().toElement();
              continue;
            }
            sln_config = sln_config.nextSibling().toElement();
          }
        }
        else {
          kddbg << VSPART_ERROR"Can't find config: " << QString(name).append("|%1").arg(platform)
              << " in kdevelop project.\n";
          return;
        }
      }
      subEl = subEl.nextSibling().toElement();
    }

    if(m_configs.empty()) {
      kddbg << VSPART_ERROR"No configs found for project. quit.\n";
      return;
    }

    // Set active solution
    QString activesln = DomUtil::readEntry(dom, VSPART_XML_SECTION_ACTIVESLN);
    vss_p sln = static_cast<vss_p>(getSlnByName(activesln));
    if(sln == 0) {
      kddbg << g_err_slnactivate.arg(activesln);
      return;
    } else {
      activateSln(sln);
      selectSln(sln);
    }

    // Set active configuration
    QString activecfg = DomUtil::readEntry(dom, VSPART_XML_SECTION_ACTIVECFG);
    vcfg_p acfg = getCfg(activecfg);
    if(acfg != 0) {
#ifdef DEBUG
      kddbg << "Setting config: " << acfg->toString() << endl;
#endif
      if(!selectCfg(acfg)) {
        kddbg << VSPART_ERROR"Failed to set config: " << acfg->toString() << endl;
        return;
      }
    }

    KDevProject::openProject(dirName, projectName);
  }

  void VSPart::closeProject() {
    return;
#ifdef DEBUG
    kddbg << VSPART_WARNING"Closing project file.\n";
#endif
    QDomDocument &dom = *projectDom();
    QDomElement project = DomUtil::elementByPath(dom, VSPART_XML_SECTION);
    QDomElement general = dom.createElement("general");
    QDomElement old_general = DomUtil::elementByPath(dom, VSPART_XML_SECTION_GENERAL);
    old_general.setTagName("general_backup");

    if(project.isNull()) {
      kddbg << VSPART_ERROR"Can't add project DOM node.\n";
      return;
    }

    KURL prj_url;
    prj_url.fromPathOrURL(m_projectPath);

    // Write included solutions
    BOOSTVEC_FOR(vse_ci, it, m_entities) {
      vss_p sln = static_cast<vss_p>(*it);
      if(sln != 0) {
        QDomElement vssln = dom.createElement(VSPART_SOLUTION);
        vssln.setAttribute("path", KURL::relativeURL(prj_url, sln->getURL()));
        vssln.setAttribute("name", sln->getName());
        QString active_prj = (sln->isLoaded()) ? sln->getActivePrj()->getName() : QString("");
        vssln.setAttribute("active", active_prj);

        if(!general.appendChild(vssln).isNull()) { kddbg << "Writing solution: " << sln->getName() << endl; }
        else { kddbg << "Can't write solution: " << sln->getName() << endl; return; }
      }
      else { kddbg << g_err_list_corrupted.arg("vs_entity").arg("VSPart::closeProject"); return; }
    }

    // Write configurations
    BOOSTVEC_FOR(vcfg_ci, it, m_configs) {
      vcfg_p cfg(*it);
      if(cfg != 0) {
        QDomElement vscfg = dom.createElement(VSPART_CONFIG);
        vscfg.setAttribute("platform", platform2String(cfg->platform()));
        vscfg.setAttribute("name", cfg->getName());

        BOOSTVEC_FOR(vse_ci, it, m_entities) {
          vss_p sln = static_cast<vss_p>(*it);
          if(sln != 0) {
            if(sln->isLoaded()) {
              vsbb_p pbb = sln->getBB(cfg);
              if(pbb != 0) {
                QDomElement vssln = dom.createElement(VSPART_SOLUTION);
                vssln.setAttribute("config", pbb->config().getName());
                vssln.setAttribute("platform", platform2String(pbb->config().platform()));
                vssln.setAttribute("name", sln->getName());
                vssln.setAttribute("enabled", pbb->isEnabled());
                if(!vscfg.appendChild(vssln).isNull()) {
                  kddbg << "Buildbox for: " << sln->getName() << " is added to: " << cfg->getName() << endl;
                }
                else {
                  kddbg << VSPART_ERROR"Can't append sln config to config node.\n";
                  return;
                }
              }
              else {
                kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(cfg->toString()).arg("VSPart::closeProject");
                return;
              }
            }
          }
          else { kddbg << g_err_list_corrupted.arg("vs_entity").arg("VSPart::closeProject"); return; }
        }

        if(!general.appendChild(vscfg).isNull()) {
          kddbg << "Config: " << cfg->getName() << " is inserted to project upon save.\n";
        }
        else {
          kddbg << VSPART_ERROR"Can't insert config node: " << cfg->getName() << "into general node upon save.\n";
          return;
        }
      }
      else { kddbg << g_err_list_corrupted.arg(VSPART_CONFIG).arg("VSPart::closeProject"); return; }
    }

    if(!project.appendChild(general).isNull()) {
      kddbg << "Config node \"general\" is saved in DOM tree.\n";
    }
    else {
      kddbg << "Can't append general to project.\n";
      return;
    }

    // Rename general node and remove old general node
    //general.setTagName("general");
    if(!project.removeChild(old_general).isNull()) {
      kddbg << VSPART_ERROR"Can't remove general_backup node.\n";
    }
    else {
      kddbg << "Node: general_backup is removed.\n";
    }

    // Write active config
    DomUtil::writeEntry(dom, VSPART_XML_SECTION_ACTIVECFG, active_cfg->toString());

    // Write active solution
    DomUtil::writeEntry(dom, VSPART_XML_SECTION_ACTIVESLN, active_sln->getName());

#ifdef DEBUG
    kddbg << "Project is saved.\n";
#endif
  }

  QString VSPart::projectName() const {
    return m_projectName;
  }

  QString VSPart::projectDirectory() const {
    return m_projectPath;
  }

  DomUtil::PairList VSPart::runEnvironmentVars() const {
    return DomUtil::readPairListEntry ( *projectDom(), VSPART_XML_SECTION"/run/envvars", "envvar", "name", "value" );
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

  void VSPart::addFile(const QString &fl) {
#ifdef DEBUG
    kddbg << "adding file: " << fl << endl;
#endif
  }

  void VSPart::addFiles(const QStringList &list) {
#ifdef DEBUG
    QStringList::const_iterator it;
    for(it=list.begin(); it!=list.end(); ++it) {
      kddbg << "[L] adding file: " << (*it) << endl;
    }
#endif
  }

  void VSPart::removeFile(const QString &fl) {
#ifdef DEBUG
    kddbg << "removing file: " << fl << endl;
#endif
  }

  void VSPart::removeFiles(const QStringList &list) {
#ifdef DEBUG
    QStringList::const_iterator it;
    for(it=list.begin(); it!=list.end(); ++it) {
      kddbg << "[L] remove file: " << (*it) << endl;
    }
#endif
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

  bool VSPart::loadVsSolution(const QString &iname, const KURL &url) {
    // Create model::solution item
    vss_p sln = new VSSolution(iname);
    if(sln != 0) {
      sln->setPath(url.url());
      /* NOTE: We adding "incolmplete" sln to be sure we will save all solutions that were
          stored in .kdevelop file */
#ifdef USE_BOOST
      m_entities.push_back(sln);
#else
      m_entities.append(sln);
#endif
      // Read and parse sln file
      if(sln->read()) {
        if(!sln->setActivePrj(active_prj)) {
          kddbg << VSPART_ERROR"Can't activate a prj: " << active_prj << " in sln: " << sln->getName() << endl;
        }
      } else { kddbg << g_err_slnload.arg(iname); }

      // Try to attach to current config
      vsbb_p slnbb = sln->getBB(active_cfg->toString());
      if(slnbb != 0) {
#ifdef DEBUG
        kddbg << "KPROJ[ " << active_cfg->toString() << " ] <<< SLN[ " << sln->getName()
            << " ]:[ " << slnbb->config().toString() << " ].\n";
#endif
        slnbb->setParentCfg(active_cfg);
        slnbb->setEnabled(true);
      }

      // Generate dependencies  from meta-dependency information
      if(!sln->updateDependencies()) {
        kddbg << VSPART_ERROR"Can't gen dependencies from meta-dependencies.\n";
      }

      // Create UI representation
      if(!sln->populateUI()) {
        kddbg << VSPART_ERROR"Can't use sln. UI update failed.\n";
      }

      return true;
    }
    else { kddbg << g_err_notenoughmem.arg(VSPART_SOLUTION).arg("VSPart::openProject"); }
    return false;
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

  vse_p VSPart::getByUID(const QUuid &/*uid*/) const {
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
      vse_ci it=m_entities.begin();
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
      if(selected_sln == s) { return true; }
      selected_sln = s;

      // Set variables
      setVar(VSPART_V_SLNDIR, selected_sln->getURL().path());
      setVar(VSPART_V_SLNEXT, selected_sln->getExt());
      setVar(VSPART_V_SLNFLN, selected_sln->getURL().fileName(false));
      setVar(VSPART_V_SLNNAME, selected_sln->getName());
      setVar(VSPART_V_SLNPATH, selected_sln->getURL().path());

      vcfg_cp scfg = selected_sln->currentCfg();
      if(scfg != 0) {
        setVar(VSPART_V_CFGNAME, scfg->getName());
        setVar(VSPART_V_PLATFNAME, scfg->getPlatform());
      }

#ifdef DEBUG
      kddbg << g_msg_slnselect.arg(s->getName());
#endif

      mVViewWidget->slotRefreshValues();
      return true;
    }
    kddbg << g_err_nullptr.arg("VSPart::selectSln");
    return false;
  }

  bool VSPart::selectPrj(vsp_p prj) {
    if(prj != 0 && prj->getType() == vs_project) {
      if(prj == selected_prj) { return true; }
      //TODO: Deselect previous selected maybe !?

      selected_prj = prj;

      if(selected_sln != selected_prj->getParent()) { selectSln(static_cast<vss_p>(selected_prj->getParent())); }

      // Set variables
      setVar(VSPART_V_INPDIR, selected_prj->getURL().path());
      setVar(VSPART_V_INPEXT, selected_prj->getExt());
      setVar(VSPART_V_INPFLN, selected_prj->getURL().fileName(false));
      setVar(VSPART_V_INPNAME, selected_prj->getName());
      setVar(VSPART_V_INPPATH, selected_prj->getURL().path());

      setVar(VSPART_V_PRJDIR, selected_prj->getURL().path());
      setVar(VSPART_V_PRJEXT, selected_prj->getExt());
      setVar(VSPART_V_PRJFLN, selected_prj->getURL().fileName(false));
      setVar(VSPART_V_PRJNAME, selected_prj->getName());
      setVar(VSPART_V_PRJPATH, selected_prj->getURL().path());

      setVar(VSPART_V_INPNAME_S, selected_prj->getName());

      setVar(VSPART_V_PNTNAME, selected_prj->getName());
      setVar(VSPART_V_PNTNAME_S, selected_prj->getName());

      setVar(VSPART_V_ROOTNS, selected_prj->getName());
      setVar(VSPART_V_ROOTNS_S, selected_prj->getName());

      vcfg_cp pcfg = selected_prj->currentCfg();
      if(pcfg != 0) {
        setVar(VSPART_V_CFGNAME, pcfg->getName());
        setVar(VSPART_V_PLATFNAME, pcfg->getPlatform());
      }

      setVar(VSPART_V_OUTDIR, selected_prj->getOutDir().path());
      setVar(VSPART_V_INTDIR, selected_prj->getIntDir().path());

      mVViewWidget->slotRefreshValues();
      return true;
    }
    return false;
  }

  bool VSPart::selectFile(vsfl_p f) {
    if(f != 0 && f->getType() == vs_file) {
      if(f == selected_file) { return true; }
      selected_file = f;
      vsp_p pprj = selected_file->getProject();
      vse_p pnt = selected_file->getParent();

      // Select parent project
      if(selected_prj != pprj) { selectPrj(pprj); }

      // Set variables
      setVar(VSPART_V_INPDIR, selected_file->getURL().path());
      setVar(VSPART_V_INPEXT, selected_file->getExt());
      setVar(VSPART_V_INPFLN, selected_file->getURL().fileName(false));
      setVar(VSPART_V_INPNAME, selected_file->getName());
      setVar(VSPART_V_INPPATH, selected_file->getURL().path());

      setVar(VSPART_V_INPNAME_S, selected_file->getName());

      switch(pnt->getType()) {
        case vs_project: {
          setVar(VSPART_V_PNTNAME, static_cast<vsp_p>(pnt)->getName());
          setVar(VSPART_V_PNTNAME_S, static_cast<vsp_p>(pnt)->getName());
          break; }
        case vs_filter: {
          setVar(VSPART_V_PNTNAME, static_cast<vsf_p>(pnt)->getName());
          setVar(VSPART_V_PNTNAME_S, static_cast<vsf_p>(pnt)->getName());
          break; }
        default: {
          break; }
      }

      mVViewWidget->slotRefreshValues();

      partController()->editDocument(selected_file->getURL());
      return true;
    }
    return false;
  }

  void VSPart::setVar(const QString &var, const QString &val) {
    m_variables[var] = val;
  }

  QString VSPart::expandPath(const QString &path, vse_p ent) {
    return QString("/fake/exp/path"); // TODO: Stopped here
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
      if(sln->write(true)) { return true; }
#ifdef DEBUG
      else { kddbg << VSPART_ERROR"Can't save sln: " << sln->getName() << endl; return false; }
#endif
    }
#ifdef DEBUG
    else { kddbg << g_err_nullptr.arg("VSPart::saveSln"); }
#endif
    return false;
  }

  bool VSPart::saveSlnAs(vss_p s, const QString &path) {
    kddbg << "SLOT: VSPart::slotSaveSlnAs \"" << path << "\".\n";
    return false;
  }

  bool VSPart::createCfg(const QString &n, e_VSPlatform p, bool fix_sln/*=false*/, bool fix_prj/*=false*/) {
    // Check if we already have configuration with the same parameters.
    BOOSTVEC_FOR(vcfg_ci, it, m_configs) {
      vcfg_p cfg = static_cast<vcfg_p>(*it);
      if(cfg != 0) {
        if((cfg->getName() == n) && (cfg->platform() == p)) {
          kddbg << "Duplicate configs are not allowed.\n";
          return false;
        }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_CONFIG).arg("VSPart::createCfg");
        return false;
      }
    }

    // Create new configuration object
    vcfg_p test_cfg = new VSConfig(n, p);

    if(test_cfg != 0) {
      if(fix_sln) { // Create new configs in child sln objects
        vcfgcr cr;
        cr.name = n;
        cr.platform = p;
        cr.sync_subs = fix_prj;
        BOOSTVEC_FOR(vse_ci, it, m_entities) {
          vss_p s = static_cast<vss_p>(*it);
          if(s != 0) {
            if(!s->createCfg(test_cfg, cr)) {
              kddbg << VSPART_ERROR"Failed to add sln config: "<< cr.string() << " for \"" << s->getName()
                  << "\", in {VSPart::setCfg}\n";
              delete test_cfg;
              return false;
            }
          } else {
            kddbg << g_err_list_corrupted.arg(VSPART_SOLUTION).arg("VSPart::addCfg");
            delete test_cfg;
            return false;
          }
        }
      }
      m_configs.push_back(test_cfg);

      // Update UI
      // actConfigName->view()->clear();
      // actConfigPlatform->view()->clear();
      QListViewItem *cfgnm = actConfigName->view()->listView()->findItem(n, 0);
      if(cfgnm == 0) {
        cfgnm = new QListViewItem(actConfigName->view()->listView(), n);
        cfgnm->setPixmap(0, SmallIcon("gear"));
        actConfigName->view()->addItem(cfgnm);
      }

      QListViewItem *cfgpl = actConfigPlatform->view()->listView()->findItem(platform2String(p), 0);
      if(cfgpl == 0) {
        cfgpl = new QListViewItem(actConfigPlatform->view()->listView(), platform2String(p));
        cfgpl->setPixmap(0, SmallIcon("gear"));
        actConfigPlatform->view()->addItem(cfgpl);
      }
      return true;
    }
#ifdef DEBUG
    else { kddbg << g_err_notenoughmem.arg(VSPART_CONFIG).arg("VSPart::addConfiguration"); }
#endif
    return false;
  }

  bool VSPart::selectCfg(const vcfgcr_r pc) {
    const QString cs(pc.string());
    // Check if we have a configuration like that
    vcfg_ci it = m_configs.begin();
    BOOSTVEC_OFOR(it, m_configs) {
      if((*it) != 0) {
        if((*it)->toString() == cs) { break; }
      }
    }

    if(it != m_configs.end()) {
      return selectCfg((*it));
    }
    kddbg << g_err_ent_notfound.arg(VSPART_CONFIG).arg(cs).arg("VSPart::setCfg");
    return false;
  }

  bool VSPart::selectCfg(const vcfg_p cfg) {
    if(cfg != 0) {
      // Check if we have that config in our list
      vcfg_ci it = m_configs.begin();
      BOOSTVEC_OFOR(it, m_configs) {
        if((*it) != 0) {
          if((*it) == cfg) { break; }
        }
        else { kddbg << g_err_list_corrupted.arg(VSPART_CONFIG).arg("VSPart::selectCfg"); return false; }
      }

      if(it != m_configs.end()) {
        active_cfg = cfg; // Set it as our active configuration

        // Let all solutions know new configuration
        BOOSTVEC_FOR(vse_ci, it, m_entities) {
          vss_p s = static_cast<vss_p>(*it);
          if(s != 0) {
            if(s->isLoaded()) { //NOTE: Atempt config change only on loaded solutions
              if(!s->selectCfg(active_cfg)) {
                kddbg << VSPART_ERROR"Sln [" << s->getName() << "] can't set config [" << cfg->toString()
                    << "], in {VSPart::selectCfg}\n";
              }
            }
          } else {
            kddbg << g_err_list_corrupted.arg("vs_entity").arg("VSPart::selectCfg");
            return false;
          }
        }

        // Update UI
        actConfigName->view()->setCurrentText(cfg->getName());
        actConfigPlatform->view()->setCurrentText(platform2String(cfg->platform()));
        emit uisync();
        return true;
      }
    }
    else { kddbg << g_err_nullptr.arg("VSPart::selectCfg"); }
    return false;
  }

  vcfg_p VSPart::getCfg(const QString& c) const {
    BOOSTVEC_FOR(vcfg_ci, it, m_configs) {
      if((*it) != 0) {
        if((*it)->toString() == c) { return (*it); }
      }
      else { kddbg << g_err_list_corrupted.arg(VSPART_CONFIG).arg("VSPart::getCfg"); }
    }
    return 0;
  }

  /*inline*/ pv_vse_cr VSPart::solutions() const {
    return m_entities;
  }

  //BEGIN // Slot methods
  void VSPart::slotAddSolution() {
    // kddbg << "slotAddSolution test" << endl;
    AddExistingSlnDlg dlg(this, m_explorer_widget, "add existing sln", FALSE);
    dlg.setCaption(i18n("Add Existing Solution"));
    dlg.exec();
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
    kddbg << "slotAddFilter test.\n";
  }

  void VSPart::slotBuildFilter() {
    kddbg << "slotBuildFilter test.\n";
  }

  void VSPart::slotCleanFilter() {
    kddbg << "slotCleanFilter test.\n";
  }

  void VSPart::slotCreateConfig() {
    kddbg << "SLOT: Create Config.\n";
    // Ask user. What name and platform should be given to this configuration ?
    // QString test_name("test create config");
    // e_VSPlatform test_pl(vspl_win32);
    // bool create_sln_cfgs = true;
    // bool create_prj_cfgs = false;
  }

  void VSPart::slotSelectCfgName(QListViewItem *i) {
    vcfgcr cr;
    cr.name = i->text(0);
    cr.platform = string2Platform(actConfigPlatform->view()->currentText());
    if(!selectCfg(cr)) {
      kddbg << VSPART_ERROR"Failed to set project config: " << cr.string() << endl;
    }
  }

  void VSPart::slotSelectCfgPlatform(QListViewItem *i) {
    vcfgcr cr;
    cr.name = actConfigName->view()->currentText();
    cr.platform = string2Platform(i->text(0));
    if(!selectCfg(cr)) {
      kddbg << VSPART_ERROR"Failed to set project config: " << cr.string() << endl;
    }
  }
  //END // Slot methods
};
#include "vs_part.moc"
