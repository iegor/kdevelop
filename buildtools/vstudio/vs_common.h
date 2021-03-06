/* C/C++ Implementation: vs_common
*
* Description:
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_COMMON_H__
#define __KDEVPART_VSTUDIOPART_COMMON_H__

#include <config.h>

#include <qtextstream.h>
#include <quuid.h>

#include "vs_errors.h"

#ifdef USE_BOOST
#include <boost/container/vector.hpp>
#include <boost/container/map.hpp>
#include <boost/container/detail/pair.hpp>
#include <boost/range/algorithm/find.hpp>
// #include <boost/range/algorithm/find_first_of.hpp>
#else
#endif

// Debug messaging
#define kddbg kdDebug(9000) << "[ VSPART ] "

// VStudio XML data layout
#define VSPART_XML_SECTION "kdevvstudioproject"
#define VSPART_XML_SECTION_GENERAL VSPART_XML_SECTION"/general"
#define VSPART_XML_SECTION_ACTIVESLN VSPART_XML_SECTION_GENERAL"/active_sln"
#define VSPART_XML_SECTION_ACTIVECFG VSPART_XML_SECTION_GENERAL"/active_cfg"

#define VSPRJ_DOM_PROJECT "VisualStudioProject"
#define VSPRJ_DOM_CONFIGS "Configurations"
#define VSPRJ_DOM_FILES "Files"

// VStudio basic type names
#define VSSOLUTION_VERSION 10
#define VSPART_TOOL "vs_tool"
#define VSPART_CONFIG "vs_config"
#define VSPART_SOLUTION "vs_solution"
#define VSPART_PROJECT "vs_project"
#define VSPART_FILTER "vs_filter"
#define VSPART_FILE "vs_file"
#define VSPART_PLATFORM "vs_platform"
#define VSPART_BUILDBOX "vs_buildbox"

#define VSSLN_VER9 "vssln_ver9"
#define VSSLN_VER8 "vssln_ver8"
#define VSSLN_VER7 "vssln_ver7"

#define VSPRJ_VER9 "vsprj_ver9"
#define VSPRJ_VER8 "vsprj_ver8"
#define VSPRJ_VER7 "vsprj_ver7"
#define VSPRJ_VER6 "vsprj_ver6"

// VS build tool names
#define VSTOOL_COMPILER "vstl_compiler"
#define VSTOOL_LINKER "vstl_linker"
#define VSTOOL_MIDL "vstl_midl"

// VS platform (arch) names
#define VSPL_WIN32 "Win32"
#define VSPL_WIN64 "x64"
#define VSPL_MIXED "Mixed Platforms"
#define VSPL_ANYCPU "Any CPU"

// Language of project
#define VSPART_PRJLANG_CPP "vs_prjlang_c"
#define VSPART_PRJLANG_CSP "vs_prjlang_cs"

// Project configuration attribute names
#define VSPART_PCFGATTR_NAME "Name"
#define VSPART_PCFGATTR_OUTDIR "OutputDirectory"
#define VSPART_PCFGATTR_INTDIR "IntermediateDirectory"
#define VSPART_PCFGATTR_CTYPE "ConfigurationType"
#define VSPART_PCFGATTR_USEMFC "UseOfMFC"
#define VSPART_PCFGATTR_CHRSET "CharacterSet"
#define VSPART_PCFGATTR_ATLMIN "ATLMinimizesCRunTimeLibraryUsage"
#define VSPART_PCFGATTR_INHPSH "InheritedPropertySheets"

// Variables names
#define VSPART_V_CFGNAME "ConfigurationName"
#define VSPART_V_DNVDIR "DevEnvDir"
#define VSPART_V_FRWDIR "FrameworkDir"
#define VSPART_V_FRWSDKDIR "FrameworkSDKDir"
#define VSPART_V_FRWVER "FrameworkVersion"
#define VSPART_V_FXCOPDIR "FxCopDir"
#define VSPART_V_INPDIR "InputDir"
#define VSPART_V_INPEXT "InputExt"
#define VSPART_V_INPFLN "InputFileName"
#define VSPART_V_INPNAME "InputName"
#define VSPART_V_INPPATH "InputPath"
#define VSPART_V_INTDIR "IntDir"
#define VSPART_V_OUTDIR "OutDir"
#define VSPART_V_PNTNAME "ParentName"
#define VSPART_V_PLATFNAME "PlatformName"
#define VSPART_V_PRJDIR "ProjectDir"
#define VSPART_V_PRJEXT "ProjectExt"
#define VSPART_V_PRJFLN "ProjectFileName"
#define VSPART_V_PRJNAME "ProjectName"
#define VSPART_V_PRJPATH "ProjectPath"
#define VSPART_V_REFERENCES "References"
#define VSPART_V_REMOTEM "RemoteMachine"
#define VSPART_V_ROOTNS "RootNamespace"
#define VSPART_V_INPNAME_S "SafeInputName"
#define VSPART_V_PNTNAME_S "SafeParentName"
#define VSPART_V_ROOTNS_S "SafeRootNamespace"
#define VSPART_V_SLNDIR "SolutionDir"
#define VSPART_V_SLNEXT "SolutionExt"
#define VSPART_V_SLNFLN "SolutionFileName"
#define VSPART_V_SLNNAME "SolutionName"
#define VSPART_V_SLNPATH "SolutionPath"
#define VSPART_V_TGTDIR "TargetDir"
#define VSPART_V_TGTEXT "TargetExt"
#define VSPART_V_TGTFLN "TargetFileName"
#define VSPART_V_TGTFRW "TargetFramework"
#define VSPART_V_TGTNAME "TargetName"
#define VSPART_V_TGTPATH "TargetPath"
#define VSPART_V_VCINSTDIR "VCInstallDir"
#define VSPART_V_VSINSTDIR "VSInstallDir"
#define VSPART_V_WEBDEPLPATH "WebDeployPath"
#define VSPART_V_WEBDEPLROOT "WebDeployRoot"
#define VSPART_V_WINSDKDIR "WindowsSdkDir"
#define VSPART_V_WINSDKDIR_IA64 "WindowsSdkDirIA64"

// Variables classes
// #define VSPART_VCLS_SYSTEM "sys_var"
// #define VSPART_VCLS_USERDEF "usr_var"
// #define VSPART_VCLS_INPUT "inp_var"
// #define VSPART_VCLS_TARGET "tgt_var"
// #define VSPART_VCLS_PROJECT "prj_var"
// #define VSPART_VCLS_SOLUTION "sln_var"

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

#define VSPART_ACTION_ADD_CONFIG "config_gcreate"
#define VSPART_ACTION_ADD_CONFIG_TIP "Create kdevelop project configuration"
#define VSPART_ACTION_ADD_CONFIG_WIT "Creates a configuration for kdevelop project"
#define VSPART_ACTION_ADD_CONFIG_WIT_LONG \
"<qt>Will create a configuration for top-level kdevelop project. Then if <b>fix_sln</b> is set to \b true it " \
"create configs in all solutions that are inserted in this project, with the same name as parent config.\n" \
"If \b fix_prj is set to \b true too, then it will create corresponding configs in all vs projects that are in all \n" \
"vs solutions in this project.</qt>"

#define VSPART_ACTION_CONFIGURATION_CFGNAME "config_cfgname"
#define VSPART_ACTION_CONFIGURATION_CFGNAME_TIP "Configuration name"
#define VSPART_ACTION_CONFIGURATION_CFGNAME_WIT "Select the name for configuration to build"

#define VSPART_ACTION_CONFIGURATION_CFGPLATFORM "config_cfgplatform"
#define VSPART_ACTION_CONFIGURATION_CFGPLATFORM_TIP "Configuration platform"
#define VSPART_ACTION_CONFIGURATION_CFGPLATFORM_WIT "Select the platform for current configuration"

// VS Tools settings
#define VSPARTTS_ADDOPTS "AdditionalOptions"
#define VSPARTTS_PREPROCDEFS "PreprocessorDefinitions"
#define VSPARTTS_ADDIDIRS "AdditionalIncludeDirectories"

//Widget data
#define VSPART_SETPATH_WIDGET_NAME "setPathWidget"

// UI size data
#define VSPART_UIBTN_MINH 18
#define VSPART_UIBTN_MINW 18
#define VSPART_UIBTN_MAXH 20
#define VSPART_UIBTN_MAXW 20

#define glue2(a,b) a ## b
#define glue3(a,b,c) a ## b ## c

#define check_bit(flags, mask) (flags&mask) == mask
#define set_bit(flags, mask) ((flags)|=(mask))
#define clear_bit(flags, mask) ((flags)&=~(mask))

#ifdef USE_BOOST
#define predeclare_vs_tl_iters(pname, citer, iter) \
  typedef vector<pname>::const_iterator citer; \
  typedef vector<pname>::iterator iter;
#define predeclare_vs_tl_vector(name) \
  typedef vector<name> glue2(v_,name); \
  typedef vector<name> * glue3(v_,name,_p); \
  typedef vector<name> & glue3(v_,name,_r); \
  typedef const vector<name> & glue3(v_,name,_cr); \
  typedef vector<name*> glue2(pv_,name);\
  typedef vector<name*> * glue3(pv_,name,_p); \
  typedef vector<name*> & glue3(pv_,name,_r); \
  typedef const vector<name*> & glue3(pv_,name,_cr);
#else
#error "VStudio: Boost support is no enabled"
#endif

#define predeclare_vs_typ(cname, shortname) \
  class cname; \
  typedef cname shortname; \
  typedef cname* glue2(shortname,_p); \
  typedef const cname* glue2(shortname,_cp); \
  typedef cname& glue2(shortname,_r); \
  typedef const cname& glue2(shortname,_cr); \
  predeclare_vs_tl_iters(glue2(shortname,_p), glue2(shortname,_ci), glue2(shortname,_i)); \
  predeclare_vs_tl_vector(shortname); \

#ifdef USE_BOOST
#define BOOSTVEC_FOR(ityp, i, v) for(ityp i=v.begin(); i!=v.end(); ++i)
#define BOOSTVEC_OFOR(iter, v) for(iter=v.begin(); iter!=v.end(); ++iter)
#define BOOSTVEC_PUSHBACK(vector, value) (vector).push_back((value))
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif

#ifndef DEBUG
#define vsinline inline
#define vsinline_attrib __attribute__((always_inline))
#else
#define vsinline inline
#define vsinline_attrib
#endif

//BEGIN //VStudio namespace
namespace VStudio {
  class VSPart;

#ifdef USE_BOOST
  using boost::container::vector;
  using boost::container::map;
  using boost::container::container_detail::pair;
#else
#endif
  //===========================================================================
  // General types pre-declaration and simplification
  //===========================================================================
  //===========================================================================
  // VS model representation classes pre-declaration
  //===========================================================================
  predeclare_vs_typ(VSRefcountable, vsr); // Predeclaration for VS Refcountable model representation
  predeclare_vs_typ(VSFSStored, vsfs); // Predeclaration for VS FSStored model representation
  predeclare_vs_typ(VSEntity, vse); // Predeclaration for VS entity model representation
  predeclare_vs_typ(VSSolution, vss); // Predeclaration for VS solution model representation
  predeclare_vs_typ(VSProject, vsp); // Predeclaration for VS project model representation
  predeclare_vs_typ(VSProject_c, vspc); // Predeclaration for VS project (C lang) model representation
  predeclare_vs_typ(VSProject_cs, vspcs); // Predeclaration for VS project (C# lang) model representation
  predeclare_vs_typ(VSFilter, vsf); // Predeclaration for VS filter model representation
  predeclare_vs_typ(VSFile, vsfl); // Predeclaration for VS file model representation
  predeclare_vs_typ(VSConfig, vcfg); //Predeclaration for VS configuration model representation
  predeclare_vs_typ(VSConfigCreate, vcfgcr); // Predeclaration for VS Config creating utility class

  //===========================================================================
  // Visual studio UI representation classes
  //===========================================================================
  class VSExplorer;
  predeclare_vs_typ(VSExplorerListWidget, vslw); // Predeclaration of VS List widget
  predeclare_vs_typ(VSExplorerEntity, uivse); // Predeclaration for VS entity UI representation
  predeclare_vs_typ(VSSlnNode, uivss); // Predeclaration for VS solution UI representation
  predeclare_vs_typ(VSPrjNode, uivsp); // Predeclaration for VS project UI representation
  predeclare_vs_typ(VSFltNode, uivsf); // Predeclaration for VS filter UI representation
  predeclare_vs_typ(VSFilNode, uivsfl); // Predeclaration for VS file UI representation

  //===========================================================================
  // VS build system classes
  //===========================================================================
  predeclare_vs_typ(VSConfig, vcfg); // Predeclaration of VS Configuration
  predeclare_vs_typ(VSPlatform, vspl); // Predeclaration of VS Platform
  predeclare_vs_typ(VSTool, vstl); // Predeclaration of VS Build tool
  predeclare_vs_typ(VSBuildBox, vsbb); // Predeclaration of VS BuildBox

  //some necessary GUIDs
#ifndef QT_NO_QUUID_STRING
  static const QUuid uid_null("00000000-0000-0000-0000-000000000000");
  static const QUuid uid_vs9project_c("8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942");
  static const QUuid uid_vs9project_cs("FAE04EC0-301F-11D3-BF4B-00C04F79EFBC");
  static const QUuid uid_vs9filter("2150E333-8FDC-42A3-9474-1A3956D46DE8");
#else
  static const QUuid uid_null(0,0,0,0,0,0,0,0,0,0,0);
  static const QUuid uid_vs9project_c(0x8BC9CEB8, 0x8B4A, 0x11D0, 0x8D, 0x11, 0x00, 0xA0, 0xC9, 0x1B, 0xC9, 0x42);
  static const QUuid uid_vs9project_cs(0xFAE04EC0, 0x301F, 0x11D3, 0xBF, 0x4B, 0x00, 0xC0, 0x4F, 0x79, 0xEF, 0xBC);
  static const QUuid uid_vs9filter(0x2150E333, 0x8FDC, 0x42A3, 0x94, 0x74, 0x1A, 0x39, 0x56, 0xD4, 0x6D, 0xE8);
#endif

  // Path slash used in our system
  static const QChar g_slash('/');

  enum e_VSEntityType {
    vs_unknown = 0,
    vs_file,
    vs_filter,
    vs_project,
    vs_solution,
    vs_config,
    vs_tool,
    vs_platform,
    vs_buildbox,
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
    // Special cases
    vspl_mixed,
    vspl_anycpu, // Mostly used in C# projects
  };

  enum e_VSBuildTool {
    vstl_unknown = 0,
    vstl_compiler,
    vstl_linker,
    vstl_midl,
  };

  enum e_VSSlnVersion {
    vssln_ver_unknown = 0,
    vssln_ver9, // VS 2008 .sln file
    vssln_ver8, // VS 2005 .sln file
    vssln_ver7, // VS .Net 2003 .sln file
  };

  enum e_VSPrjVersion {
    vsprj_ver_unknown = 0,
    vsprj_ver9, // VS 2008 .vcproj file
    vsprj_ver8, // VS 2005 .vcproj file
    vsprj_ver7, // VS .Net 2003 .vsproj file
    vsprj_ver6, // VisualC++ 6.0 project file
  };

  /*
  enum e_VSSlnCfgSelectMethod {
    vscfgsel_by_parent = 0,
    vscfgsel_by_
  };
  */

  bool readGUID(QTextStream &tstream, QUuid &uid);
  bool readGUID(const QString &string, QUuid &uid);
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
  bool verifyPlatform(const QString &platform);

  QString tool2String(e_VSBuildTool vstl);
  e_VSBuildTool string2Tool(const QString &vstl);

  QString slnVer2String(e_VSSlnVersion sln_version);
  e_VSSlnVersion string2SlnVer(const QString &string);

  QString prjVer2String(e_VSPrjVersion prj_versio);
  e_VSPrjVersion string2PrjVer(const QString &string);

  typedef bool (*entityFunctor)(vse_p entity);

  void NormalizeSlashes(QString& path);
  QString RebasePath_Win(QString path_base, QString path_relative);
  bool MakeRelative(QString origin_path, QString path, QString& relative);

#ifdef USE_BOOST
  typedef vector<QString> pv_QString;
  typedef vector<QString>::const_iterator qstr_ci;
  typedef vector<QString>::iterator qstr_i;
  typedef vector<QUuid> pv_QUuid;
  typedef vector<QUuid>::const_iterator quid_ci;
  typedef vector<QUuid>::iterator quid_i;
#else
#endif
};
//END // VStudio namespace
#endif
