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
//#include <string.h>

#include "vs_common.h"

#ifndef USE_BOOST
#error "VStudio: Boost support is no enabled"
#endif

namespace VStudio {
  // const QUuid uid_null();
  /*
  UUID::UUID() {
    memset(&m1, 0, 32);
  }
  */

  bool readGUID(QTextStream &s, QUuid &uid) {
    // if(!s) {
    //   kddbg << "Error! unusable stream supplied" << endl;
    //   return false; }
#ifndef QT_NO_QUUID_STRING
    ushort raw[37]={0};
    s.readRawBytes((char*)raw, 72);
    QString guid((const QChar*)raw, 36);
    uid = QUuid(guid);
    /*
    QRegExp rx("[A-F0-9]{8}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{12}");
    int irx = rx.search(token);
    QString prjguid = token.mid(irx, 36);
    QUuid prj_uid(prjguid);
    */
#else
    char d4[8];
    s >> iuid.data1 >> iuid.data2 >> iuid.data3;
    s >> d4[0] >> d4[1] >> d4[2] >> d4[3] >> d4[4] >> d4[5] >> d4[6] >> d4[7];
    memcpy(iuid.data4, d4, sizeof(char)*8);
#endif
    return true;
  }

  QString guid2String(const QUuid &uid) {
#ifndef QT_NO_QUUID_STRING
    return uid.toString().upper();
#else
    //TODO: Implement this
#endif
  }

  e_VSEntityType uid2VSType(const QUuid &uid) {
    if(uid == uid_vs9project_c) return vs_project;
    else if(uid == uid_vs9project_cs) return vs_project;
    else if(uid == uid_vs9filter) return vs_filter;
    else return vs_unknown;
  }

  e_VSPrjLangType uid2PrjLangType(const QUuid &uid) {
    if(uid == uid_vs9project_c) return vs_prjlang_c;
    else if(uid == uid_vs9project_cs) return vs_prjlang_cs;
    else return vs_prjlang_unknown;
  }

  QString type2String(e_VSEntityType typ) {
    switch(typ) {
      case vs_tool: return VSPART_TOOL;
      case vs_config: return VSPART_CONFIG;
      case vs_solution: return VSPART_SOLUTION;
      case vs_project: return VSPART_PROJECT;
      case vs_filter: return VSPART_FILTER;
      case vs_file: return VSPART_FILE;
      case vs_platform: return VSPART_PLATFORM;
      default: return "unknown";
    }
  }

  e_VSEntityType string2Type(const QString &typ) {
    if(typ == VSPART_TOOL) return vs_tool;
    else if(typ == VSPART_CONFIG) return vs_config;
    else if(typ == VSPART_SOLUTION) return vs_solution;
    else if(typ == VSPART_PROJECT) return vs_project;
    else if(typ == VSPART_FILTER) return vs_filter;
    else if(typ == VSPART_FILE) return vs_file;
    else if(typ == VSPART_PLATFORM) return vs_platform;
    else return vs_unknown;
  }

  QString prjLangType2String(e_VSPrjLangType typ) {
    switch(typ) {
      case vs_prjlang_c: return VSPART_PRJLANG_CPP;
      case vs_prjlang_cs: return VSPART_PRJLANG_CSP;
      default: return "unknown";
    }
  }

  e_VSPrjLangType string2PrjLangType(const QString &typ) {
    if(typ == VSPART_PRJLANG_CPP) return vs_prjlang_c;
    else if(typ == VSPART_PRJLANG_CSP) return vs_prjlang_cs;
    else return vs_prjlang_unknown;
  }

  QString slnSectionType2String(e_VSSlnSection typ) {
    switch(typ) {
      case slns_sln_cfgplatforms: return VSPART_SLNSECTION_SCFG_PLATFORMS;
      case slns_prj_cfgplatforms: return VSPART_SLNSECTION_PCFG_PLATFORMS;
      case slns_sln_properties: return VSPART_SLNSECTION_SPROPS;
      case slns_nested_prjs: return VSPART_SLNSECTION_NESTEDPRJ;
      default: return "unknown";
    }
  }

  e_VSSlnSection string2SlnSectionType(const QString &typ) {
    if(typ == VSPART_SLNSECTION_SCFG_PLATFORMS) return slns_sln_cfgplatforms;
    else if(typ == VSPART_SLNSECTION_PCFG_PLATFORMS) return slns_prj_cfgplatforms;
    else if(typ == VSPART_SLNSECTION_SPROPS) return slns_sln_properties;
    else if(typ == VSPART_SLNSECTION_NESTEDPRJ) return slns_nested_prjs;
    else return slns_unknown;
  }

  QString prjSectionType2String(e_VSPrjSection typ) {
    switch(typ) {
      case prjs_dependencies: return VSPART_PRJSECTION_DEPENDENCIES;
      case prjs_slnitems: return VSPART_PRJSECTION_SLNITEMS;
      default: return "unknown";
    }
  }

  e_VSPrjSection string2PrjSectionType(const QString &typ) {
    if(typ == VSPART_PRJSECTION_DEPENDENCIES) return prjs_dependencies;
    else if(typ == VSPART_PRJSECTION_SLNITEMS) return prjs_slnitems;
    else return prjs_unknown;
  }

  QString platform2String(e_VSPlatform p) {
    switch(p) {
      case vspl_win32: return VSPL_WIN32;
      case vspl_win64: return VSPL_WIN64;
      default: return "unknown";
    }
  }

  e_VSPlatform string2Platform(const QString &s) {
    if(s == VSPL_WIN32) return vspl_win32;
    else if(s == VSPL_WIN64) return vspl_win64;
    else return vspl_unknown;
  }

  QString tool2String(e_VSBuildTool t) {
    switch(t) {
      case vstl_compiler: return VSTOOL_COMPILER;
      case vstl_linker: return VSTOOL_LINKER;
      case vstl_midl: return VSTOOL_MIDL;
      default: return "unknown";
    }
  }

  e_VSBuildTool string2Tool(const QString &s) {
    if(s == VSTOOL_COMPILER) return vstl_compiler;
    else if(s == VSTOOL_LINKER) return vstl_linker;
    else if(s == VSTOOL_MIDL) return vstl_midl;
    else return vstl_unknown;
  }

  //===========================================================================
  // VS Part error message strings:
  //===========================================================================
#ifdef DEBUG
  const QString g_msg_configapply("Applying config: [%1] in {%2}.\n");
  const QString g_msg_slnselect("Selecting \"%1\" solution.\n");
  const QString g_msg_entselected("[%1] \"%2\" is selected in vsexplorer.\n");
#endif /* DEBUG */
  const QString g_err_notenoughmem(VSPART_ERROR"Not enough mem to alloc %1, in {%2}.\n");
  // const QString g_err_refcount_nonzeroremoval(VSPART_ERROR"Destructing referenced object.\n");
  const QString g_err_list_corrupted(VSPART_ERROR"a [%1] list is corrupted, in {%2}.\n");
  const QString g_err_nullptr(VSPART_ERROR"Null pointer at {%1}.\n");
  const QString g_err_slnactivate(VSPART_ERROR"sln \"%1\" is failed to be activated.\n");
  const QString g_err_slnload(VSPART_ERROR"\"%1\" solution failed to load.\n");
  const QString g_err_prjload(VSPART_ERROR"\"%1\" project failed to load.\n");
  const QString g_err_ent_notfound(VSPART_ERROR"[%1]:[%2] entity is not found, in {%3}.\n");
  const QString g_err_unsupportedtyp(VSPART_ERROR"Type [%1] is unsupported, in {%2}.\n");
  const QString g_wrn_unsupportedtyp(VSPART_WARNING"Type [%1] is unsupported, in {%2}.\n");
};