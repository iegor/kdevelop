/*  C/C++ Implementation: vs_part
*
* Description:
*  ! Parts were inherited from autoprojectpart src
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_H__
#define __KDEVPART_VSTUDIOPART_H__

/* Qt */
#include <qdict.h>
#include <qguardedptr.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qdir.h>

/* KDE */
#include <kurl.h>

/* KDevelop */
#include <kdevgenericfactory.h>
#include <klistviewaction.h>
#include <kdevbuildtool.h>

/* VStudio */
#include "vs_common.h"
#include "vs_explorer.h"  // VS Explorer widget
#include "vs_dlg_addexisting_sln.h"
#include "vs_view_variables.h"
#include "vs_model.h"
// using namespace Extensions;

namespace KParts { class Part; }
namespace KTextEditor {
  class Document;
  class View;
  class EditInterface;
  class SelectionInterface;
  class ViewCursorInterface;
}

class QDomElement;
class QStringList;
class KDialogBase;
class KSelectAction;
class TargetItem;
class ConfigWidgetProxy;
class VSManagerWidget;

//BEGIN //VStudio namespace
namespace VStudio {
  class VSPart : public KDevBuildTool {
    Q_OBJECT

    public:
      typedef map<QString, QString> variables_map_t;

    public:
      VSPart(QObject *parent, const char *name, const QStringList &args);
      virtual ~VSPart();

      //===========================================================================
      // KDevProject methods
      //===========================================================================
      virtual void openProject(const QString &dirName, const QString &projectName);
      virtual void closeProject();
      virtual QString projectDirectory() const;
      virtual QString projectName() const;
      virtual DomUtil::PairList runEnvironmentVars() const;
      virtual QString mainProgram() const;
      virtual QString runDirectory() const;
      virtual QString runArguments() const;
      virtual QString debugArguments() const;
      virtual QString activeDirectory() const;
      virtual QString buildDirectory() const;
      virtual QStringList allFiles() const;
      virtual QStringList distFiles() const;
      virtual void addFile(const QString &fileName);
      virtual void addFiles(const QStringList& fileList);
      virtual void removeFile(const QString &fileName);
      virtual void removeFiles(const QStringList& fileList);

      virtual Options options() const;

      //===========================================================================
      // Implementation of the KDevPlugin interface.
      //===========================================================================
      virtual void restorePartialProjectSession(const QDomElement* el);
      virtual void savePartialProjectSession(QDomElement* el);

      //===========================================================================
      // Own methods (e.g. vs projects and solutions loading)
      //===========================================================================
      bool loadVsSolution(const QString &sln_name, const KURL &sln_path);
      bool unloadVsSolution(const QString &sln_path);
      bool loadVsProject(const QString &prj_path);
      bool unloadVsProject(const QString &prj_path);
      vse_p getByUID(const QUuid &uid) const; //TODO: decide if I need this ?
      vss_p getSlnByName(const QString &name);
      VSExplorer* explorerWidget() const { return m_explorer_widget; }
      bool selectSln(vss_p sln);
      bool selectPrj(vsp_p prj);
      bool selectFile(vsfl_p file);
      vss_p getSelectedSln() const;
      bool activateSln(vss_p sln);
      vss_p getActiveSln() const;
      bool activatePrj(vsp_p prj);
      vsp_p getActivePrj() const;
      bool saveSln(vss_p sln);
      bool saveSlnAs(vss_p sln, const QString &new_path);

      // Configurations works
      bool createCfg(const QString &name, e_VSPlatform platform, bool fix_sln=false, bool fix_prj=false);
      bool selectCfg(const vcfgcr_r pc);
      bool selectCfg(const vcfg_p config);
      bool createSlnCfg(vss_p sln);
      vcfg_p getCfg(const QString& c) const;

      pv_vse_cr solutions() const;
      vsinline const variables_map_t& vars() const vsinline_attrib { return m_variables; }
      void setVar(const QString &variable, const QString &value);

      QString expandPath(const QString &path, vse_p entity);

    private slots:
      void slotAddSolution();
      void slotBuildSolution();
      void slotRebuildSolution();
      void slotCleanSolution();
      void slotAddProject();
      void slotBuildProject();
      void slotRebuildProject();
      void slotCleanProject();
      void slotAddFile();
      void slotBuildFile();
      void slotCleanFile();
      void slotAddFilter();
      void slotBuildFilter();
      void slotCleanFilter();

      void slotCreateConfig();
      void slotSelectCfgName(QListViewItem *item);
      void slotSelectCfgPlatform(QListViewItem *item);

    public:
      KAction *actCreateConfig;

    private:
      KAction *actAddSolution;
      KAction *actBuildSolution;
      KAction *actRebuildSolution;
      KAction *actCleanSolution;
      KAction *actAddProject;
      KAction *actBuildProject;
      KAction *actRebuildProject;
      KAction *actCleanProject;
      KAction *actAddFile;
      KAction *actBuildFile;
      KAction *actCleanFile;
      KAction *actAddFilter;
      KAction *actBuildFilter;
      KAction *actCleanFilter;

      KListViewAction *actConfigName;
      KListViewAction *actConfigPlatform;

      QGuardedPtr<VSExplorer> m_explorer_widget;
      QGuardedPtr<VSViewVars> mVViewWidget; // Variables view

      QString m_projectName;
      QString m_projectPath;
      QMap<QString, QDateTime> m_timestamp;

      vss_p selected_sln;
      vsp_p selected_prj;
      vsfl_p selected_file;
      vss_p active_sln;
      vsp_p active_prj;
      vcfg_p active_cfg; // Selected configuration
#ifdef USE_BOOST
      pv_vse m_entities;     // Solutions
      pv_vcfg m_configs;
      variables_map_t m_variables; // Variable stack ( i.e. $(SolutionDir), etc )
#else
      QPtrList<vse_p> m_entities;
#endif
      // QString m_prjpath;
      // QString m_prjname;

    signals:
      void uisync();
  };
};
//END // VStudio namespace
#endif
