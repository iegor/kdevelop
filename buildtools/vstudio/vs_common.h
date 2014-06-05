/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_common
*
* Description:
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_COMMON_H__
#define __KDEVPART_VSTUDIOPART_COMMON_H__

#include <config.h>

#include <qtextstream.h>
#include <quuid.h>

#ifdef USE_BOOST
#include <boost/container/vector.hpp>
#include <boost/range/algorithm/find.hpp>
// #include <boost/range/algorithm/find_first_of.hpp>
#else
#endif

// Debug messaging
#define kddbg kdDebug(9000) << "[ VSPART ] "

// VStudio basic type names
#define VSSOLUTION_VERSION 10
#define VSPART_TOOL "vs_tool"
#define VSPART_CONFIG "vs_config"
#define VSPART_SOLUTION "vs_solution"
#define VSPART_PROJECT "vs_project"
#define VSPART_FILTER "vs_filter"
#define VSPART_FILE "vs_file"

// VS build tool names
#define VSTOOL_COMPILER "vstl_compiler"
#define VSTOOL_LINKER "vstl_linker"
#define VSTOOL_MIDL "vstl_midl"

// VS platform (arch) names
#define VSPL_WIN32 "Win32"
#define VSPL_WIN64 "x64"

// Language of project
#define VSPART_PRJLANG_CPP "vs_prjlang_c"
#define VSPART_PRJLANG_CSP "vs_prjlang_cs"

// Project setcion types
#define VSPART_PRJSECTION_DEPENDENCIES "ProjectDependencies"
#define VSPART_PRJSECTION_SLNITEMS "SolutionItems"

// Solution section types
#define VSPART_SLNSECTION_SCFG_PLATFORMS "SolutionConfigurationPlatforms"
#define VSPART_SLNSECTION_PCFG_PLATFORMS "ProjectConfigurationPlatforms"
#define VSPART_SLNSECTION_SPROPS "SolutionProperties"
#define VSPART_SLNSECTION_NESTEDPRJ "NestedProjects"

//Actions
#define VSPART_ACTION_TOOLS_GROUP "vstools"

#define VSPART_ACTION_ADD_SOLUTION "project_addsolution"
#define VSPART_ACTION_ADD_SOLUTION_TIP "Add solution into kdevelop project"
#define VSPART_ACTION_ADD_SOLUTION_WIT "Adds solution that exist in current kdevelop project"

#define VSPART_ACTION_BUILD_SOLUTION "build_buildsolution"
#define VSPART_ACTION_BUILD_SOLUTION_KEY Key_F8
#define VSPART_ACTION_BUILD_SOLUTION_TIP "Build solution"
#define VSPART_ACTION_BUILD_SOLUTION_WIT "Builds selected solution"
#define VSPART_ACTION_BUILD_SOLUTION_WIT_LONG \
"<b>Build solution</b> <p>Runs <b>build process</b>from the project directory.<br>\
Environment variables and make arguments can be specified in the project settings dialog, <b>Make Options</b> tab."

#define VSPART_ACTION_REBUILD_SOLUTION "build_rebuildsolution"
#define VSPART_ACTION_REBUILD_SOLUTION_TIP "Rebuild solution"
#define VSPART_ACTION_REBUILD_SOLUTION_WIT "Rebuilds selected solution"

#define VSPART_ACTION_CLEAN_SOLUTION "build_cleansolution"
#define VSPART_ACTION_CLEAN_SOLUTION_TIP "Clean solution"
#define VSPART_ACTION_CLEAN_SOLUTION_WIT "Cleans out outputs from latest build of selected solution"

#define VSPART_ACTION_ADD_PROJECT "project_addproject"
#define VSPART_ACTION_ADD_PROJECT_TIP "Add project into selected solution"
#define VSPART_ACTION_ADD_PROJECT_WIT "Adds existing project into selected solution"

#define VSPART_ACTION_BUILD_PROJECT "build_buildproject"
#define VSPART_ACTION_BUILD_PROJECT_KEY Key_F7
#define VSPART_ACTION_BUILD_PROJECT_TIP "Build project"
#define VSPART_ACTION_BUILD_PROJECT_WIT "Builds selected project"
#define VSPART_ACTION_BUILD_PROJECT_WIT_LONG \
"<b>Build project</b><p>Runs <b>build process</b> from the project directory.<br> \
Environment variables and make arguments can be specified in the project settings dialog, <b>Make Options</b> tab."

#define VSPART_ACTION_REBUILD_PROJECT "build_rebuildproject"
#define VSPART_ACTION_REBUILD_PROJECT_TIP "Rebuild project"
#define VSPART_ACTION_REBUILD_PROJECT_WIT "Rebuilds selected project"

#define VSPART_ACTION_CLEAN_PROJECT "build_cleanproject"
#define VSPART_ACTION_CLEAN_PROJECT_TIP "Clean project"
#define VSPART_ACTION_CLEAN_PROJECT_WIT "Cleans out outputs from latest build of selected project"

#define VSPART_ACTION_ADD_FILE "project_addfile"
#define VSPART_ACTION_ADD_FILE_TIP "Add file"
#define VSPART_ACTION_ADD_FILE_WIT "Adds or creates file into selected project"

#define VSPART_ACTION_BUILD_FILE "build_buildfile"
#define VSPART_ACTION_BUILD_FILE_TIP "Compile file"
#define VSPART_ACTION_BUILD_FILE_WIT "Compiles selected file and creates object code file .obj for it"

#define VSPART_ACTION_CLEAN_FILE "build_cleanfile"
#define VSPART_ACTION_CLEAN_FILE_TIP "Clean file"
#define VSPART_ACTION_CLEAN_FILE_WIT "Cleans out compiled .obj for selected file"

#define VSPART_ACTION_ADD_FILTER "project_addfilter"
#define VSPART_ACTION_ADD_FILTER_TIP "Add filter"
#define VSPART_ACTION_ADD_FILTER_WIT "Adds filter into selected solution or project"

#define VSPART_ACTION_BUILD_FILTER "build_buildfilter"
#define VSPART_ACTION_BUILD_FILTER_TIP "Build filter"
#define VSPART_ACTION_BUILD_FILTER_WIT "Builds solutions and/or projects under selected filter"

#define VSPART_ACTION_CLEAN_FILTER "build_cleanfilter"
#define VSPART_ACTION_CLEAN_FILTER_TIP "Clean filter"
#define VSPART_ACTION_CLEAN_FILTER_WIT "Cleans out build outputs of solutions/or projects under selected filter"

#define VSPART_ACTION_CONFIGURE_ENTITY "confentity"
#define VSPART_ACTION_CONFIGURE_ENTITY_KEY "Alt+F7"
#define VSPART_ACTION_CONFIGURE_ENTITY_TIP "Configure vs entity"
#define VSPART_ACTION_CONFIGURE_ENTITY_WIT "Opens up configuration dialog for vs entity"
#define VSPART_ACTION_CONFIGURE_ENTITY_WIT_LONG \
"<qt><b>Configure entity</b><p>Change configuration values for selected entity.</p></qt>"

#define VSPART_ACTION_RENAME_ENTITY "renfentity"
#define VSPART_ACTION_RENAME_ENTITY_KEY Key_F2
#define VSPART_ACTION_RENAME_ENTITY_TIP "Rename vs entity"
#define VSPART_ACTION_RENAME_ENTITY_WIT "Allows to change selected entity's name"
#define VSPART_ACTION_RENAME_ENTITY_WIT_LONG \
"<qt><b>Rename entity</b><p>Change internal name of selected entity.</p></qt>"

#define VSPART_ACTION_CONFIGURATION_CFGNAME "config_cfgname"
#define VSPART_ACTION_CONFIGURATION_CFGNAME_TIP "Configuration name"
#define VSPART_ACTION_CONFIGURATION_CFGNAME_WIT "Select the name for configuration to build"

#define VSPART_ACTION_CONFIGURATION_CFGPLATFORM "config_cfgplatform"
#define VSPART_ACTION_CONFIGURATION_CFGPLATFORM_TIP "Configuration platform"
#define VSPART_ACTION_CONFIGURATION_CFGPLATFORM_WIT "Select the platform for current configuration"

//Widget data
#define VSPART_SETPATH_WIDGET_NAME "setPathWidget"

#ifdef USE_BOOST
#define predeclare_vs_tl_iters(pname, citer, iter) \
  typedef boost::container::vector<pname>::const_iterator citer; \
  typedef boost::container::vector<pname>::iterator iter;
#define predeclare_vs_tl_vector(name) \
  typedef boost::container::vector<name> v_##name; \
  typedef boost::container::vector<name*> pv_##name;
#else
#error "VStudio: Boost support is no enabled"
#endif

#define predeclare_vs_typ(cname, pname, citer, iter) \
  class cname; \
  typedef cname* pname; \
  predeclare_vs_tl_iters(pname, citer, iter); \
  predeclare_vs_tl_vector(cname);

//BEGIN //VStudio namespace
namespace VStudio {
  class VSPart;
  //===========================================================================
  // General types pre-declaration and simplification
  //===========================================================================
  //===========================================================================
  // VS model representation classes pre-declaration
  //===========================================================================
  predeclare_vs_typ(VSEntity, vse_p, ve_ci, ve_i); // Pointer typedef for VS entity model representation
  predeclare_vs_typ(VSSolution, vss_p, vs_ci, vs_i); // Pointer typedef for VS solution model representation
  predeclare_vs_typ(VSProject, vsp_p, vp_ci, vp_i); // Pointer typedef for VS project model representation
  predeclare_vs_typ(VSProject_c, vspc_p, vpc_ci, vpc_i); // Pointer typedef for VS project (C lang) model representation
  predeclare_vs_typ(VSFilter, vsf_p, vf_ci, vf_i); // Pointer typedef for VS filter model representation
  predeclare_vs_typ(VSFile, vsfl_p, vfl_ci, vfl_i); // Pointer typedef for VS file model representation
  predeclare_vs_typ(VSConfig, vcfg_p, vcfg_ci, vcfg_i); //Pointer typedef for VS configuration model representation

  //===========================================================================
  // Visual studio UI representation classes
  //===========================================================================
  class VSExplorer;
  predeclare_vs_typ(VSExplorerEntity, uivse_p, uive_ci, uive_i); // Pointer typedef for VS entity UI representation
  predeclare_vs_typ(VSSlnNode, uivss_p, uivs_ci, uivs_i); // Pointer typedef for VS solution UI representation
  predeclare_vs_typ(VSPrjNode, uivsp_p, uivp_ci, uivp_i); // Pointer typedef for VS project UI representation
  predeclare_vs_typ(VSFltNode, uivsf_p, uivf_ci, uivf_i); // Pointer typedef for VS filter UI representation
  predeclare_vs_typ(VSFilNode, uivsfl_p, uivfl_ci, uivfl_i); // Pointer typedef for VS file UI representation

  //===========================================================================
  // VS build system classes
  //===========================================================================
  predeclare_vs_typ(VSConfig, vscf_p, vscf_ci, vscf_i); // VS Configuration
  predeclare_vs_typ(VSPlatform, vspl_p, vspl_ci, vspl_i); // VS Platform
  predeclare_vs_typ(VSTool, vstl_p, vstl_ci, vstl_i); // VS Build tool

  //some necessary GUIDs
#ifndef QT_NO_QUUID_STRING
  static const QUuid uid_vs9project_c("8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942");
  static const QUuid uid_vs9project_cs("FAE04EC0-301F-11D3-BF4B-00C04F79EFBC");
  static const QUuid uid_vs9filter("2150E333-8FDC-42A3-9474-1A3956D46DE8");
#else
  static const QUuid uid_vs9project_c(0x8BC9CEB8, 0x8B4A, 0x11D0, 0x8D, 0x11, 0x00, 0xA0, 0xC9, 0x1B, 0xC9, 0x42);
  static const QUuid uid_vs9project_cs(0xFAE04EC0, 0x301F, 0x11D3, 0xBF, 0x4B, 0x00, 0xC0, 0x4F, 0x79, 0xEF, 0xBC);
  static const QUuid uid_vs9filter(0x2150E333, 0x8FDC, 0x42A3, 0x94, 0x74, 0x1A, 0x39, 0x56, 0xD4, 0x6D, 0xE8);
#endif

  enum e_VSEntityType {
    vs_unknown = 0,
    vs_file,
    vs_filter,
    vs_project,
    vs_solution,
    vs_config,
    vs_tool,
  };

  enum e_VSPrjLangType {
    vs_prjlang_unknown=0,
    vs_prjlang_c,
    vs_prjlang_cs,
  };

  enum e_VSSlnSection {
    slns_unknown = 0,
    slns_sln_cfgplatforms,
    slns_prj_cfgplatforms,
    slns_sln_properties,
    slns_nested_prjs,
  };

  enum e_VSPrjSection {
    prjs_unknown = 0,
    prjs_dependencies,
    prjs_slnitems,
  };

  enum e_VSPlatform {
    vspl_unknown = 0,
    vspl_win32,
    vspl_win64,
  };

  enum e_VSBuildTool {
    vstl_unknown = 0,
    vstl_compiler,
    vstl_linker,
    vstl_midl,
  };

  bool readGUID(QTextStream &tstream, QUuid &uid);
  QString guid2String(const QUuid &uid);

  e_VSEntityType uid2VSType(const QUuid &uid);
  e_VSPrjLangType uid2PrjLangType(const QUuid &uid);

  QString prjLangType2String(e_VSPrjLangType type);
  e_VSPrjLangType string2PrjLangType(const QString &type);

  QString type2String(e_VSEntityType type);
  e_VSEntityType string2Type(const QString &type);

  QString slnSectionType2String(e_VSSlnSection type);
  e_VSSlnSection string2SlnSectionType(const QString &type);

  QString prjSectionType2String(e_VSPrjSection type);
  e_VSPrjSection string2PrjSectionType(const QString &type);

  QString platform2String(e_VSPlatform platform);
  e_VSPlatform string2Platform(const QString &platform);

  QString tool2String(e_VSBuildTool vstl);
  e_VSBuildTool string2Tool(const QString &vstl);

  typedef bool (*entityFunctor)(vse_p entity);

#ifdef USE_BOOST
  typedef boost::container::vector<QString> pv_QString;
  typedef boost::container::vector<QString>::const_iterator qstr_ci;
  typedef boost::container::vector<QString>::iterator qstr_i;
  typedef boost::container::vector<QUuid> pv_QUuid;
  typedef boost::container::vector<QUuid>::const_iterator quid_ci;
  typedef boost::container::vector<QUuid>::iterator quid_i;
#else
#endif
};
//END // VStudio namespace
#endif