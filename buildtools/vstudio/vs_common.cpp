/*  C/C++ Implementation: vs_common
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
#include <qstringlist.h>

#include <kdebug.h>

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

  bool readGUID(const QString &string, QUuid &uid) {
    QTextIStream s(&string);
#ifdef QT_NO_QUUID_STRING
    char d4[8];
    s >> d4[0]; //NOTE: This will remove '{'
    s >> iuid.data1 >> iuid.data2 >> iuid.data3;
    s >> d4[0] >> d4[1] >> d4[2] >> d4[3] >> d4[4] >> d4[5] >> d4[6] >> d4[7];
    memcpy(iuid.data4, d4, sizeof(char)*8);
#else
    ushort raw[37]={0};
    s.readRawBytes((char*)raw, 72);
    QString guid((const QChar*)raw, 36);
    uid = QUuid(guid);
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
      case vs_buildbox: return VSPART_BUILDBOX;
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
    else if(typ == VSPART_BUILDBOX) return vs_buildbox;
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
      case vspl_win32: { return VSPL_WIN32; }
      case vspl_win64: { return VSPL_WIN64; }
      case vspl_mixed: { return VSPL_MIXED; }
      case vspl_anycpu: { return VSPL_ANYCPU; }
      default: { return "unknown"; }
    }
  }

  e_VSPlatform string2Platform(const QString &s) {
    if(s.compare(VSPL_WIN32) == 0) { return vspl_win32; }
    else if(s.compare(VSPL_WIN64) == 0) { return vspl_win64; }
    else if(s.compare(VSPL_MIXED) == 0) { return vspl_mixed; }
    else if(s.compare(VSPL_ANYCPU) == 0) { return vspl_anycpu; }
    else { return vspl_unknown; }
  }

  bool verifyPlatform(const QString &p) {
    return (string2Platform(p) != vspl_unknown);
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

  QString slnVer2String(e_VSSlnVersion v) {
    switch(v) {
      case vssln_ver9: return VSSLN_VER9;
      case vssln_ver8: return VSSLN_VER8;
      case vssln_ver7: return VSSLN_VER7;
      default: return "unknown";
    }
  }

  e_VSSlnVersion string2SlnVer(const QString &s) {
    if(s == VSSLN_VER9) { return vssln_ver9; }
    if(s == VSSLN_VER8) { return vssln_ver8; }
    if(s == VSSLN_VER7) { return vssln_ver7; }
    else { return vssln_ver_unknown; }
  }

  QString prjVer2String(e_VSPrjVersion v) {
    switch(v) {
      case vsprj_ver9: { return VSPRJ_VER9; }
      case vsprj_ver8: { return VSPRJ_VER8; }
      case vsprj_ver7: { return VSPRJ_VER7; }
      case vsprj_ver6: { return VSPRJ_VER6; }
      default: { return "unknown"; }
    }
  }

  e_VSPrjVersion string2PrjVer(const QString &s) {
    if(s == VSPRJ_VER9) { return vsprj_ver9; }
    else if(s == VSPRJ_VER8) { return vsprj_ver8; }
    else if(s == VSPRJ_VER7) { return vsprj_ver7; }
    else if(s == VSPRJ_VER6) { return vsprj_ver6; }
    else { return vsprj_ver_unknown; }
  }

  void NormalizeSlashes(QString& path) {
    uint i;
    for(i=path.length(); i>0; --i) {
      switch(path[i].latin1()) {
        case '\\': {
          path[i] = g_slash;
          break; }
        default: {
          break; }
      }
    }
  }

  QString RebasePath_Win(QString path_base, QString path_relative) {
    // Normalize pathes
    NormalizeSlashes(path_base);
    NormalizeSlashes(path_relative);
#ifdef DEBUG
    // kddbg << "PATH_B: " << path_base << endl;
    // kddbg << "PATH_R: " << path_relative << endl;
#endif
    QString path;
    QStringList base = QStringList::split(g_slash, path_base);
    QStringList::ConstIterator base_it = base.end();
    --base_it; //NOTE: Ignoring filename
    QStringList relative = QStringList::split(g_slash, path_relative);
    QStringList::ConstIterator relative_it;

    // Shift up as much as we need
    for(relative_it = relative.begin(); relative_it != relative.end(); ) {
      if((*relative_it).compare("..") == 0) { // Shift base one level up
        --base_it;
        ++relative_it;
      }
      else if((*relative_it).compare(".") == 0) {
        ++relative_it;
      }
      else {
        break;
      }
    }
#ifdef DEBUG
    // kddbg << "BASEIT: " << *base_it << endl;
    // kddbg << "TEST_END: " << *(base.end()) << endl;
#endif
    // Construct new corrected absolute path
    QStringList::ConstIterator bsi;
    for(bsi = base.begin(); bsi != base_it; ++bsi) {
#ifdef DEBUG
      // kddbg << "append: " << *bsi << endl;
#endif
      path.append(g_slash).append(*bsi);
    }
    for(bsi = relative_it; bsi != relative.end(); ++bsi) {
#ifdef DEBUG
      // kddbg << "append: " << *bsi << endl;
#endif
      path.append(g_slash).append(*bsi);
    }

    return path;
  }

  bool MakeRelative(QString origin_path, QString path, QString& relative) {
    // Normalize paths
    NormalizeSlashes(origin_path);
    NormalizeSlashes(path);

    relative = path.mid(origin_path.length() + 1);
    return true;
  }

  //===========================================================================
  // VS Part error message strings:
  //===========================================================================
#ifdef DEBUG
  const QString g_msg_configapply("Applying config: [%1] for [%2], in {%3}.\n");
  const QString g_msg_slnselect("Selecting \"%1\" solution.\n");
  const QString g_msg_entselected("[%1] \"%2\" is selected in vsexplorer.\n");
#endif /* DEBUG */
  const QString g_err_guidparse(VSPART_ERROR"Can't parse GUID %1, in {%2}.\n");
  const QString g_err_emptypath(VSPART_ERROR"%1 path is empty, in {%2} .\n");
  const QString g_err_unsupportedplatform(VSPART_ERROR"Platform: %1 is not supported.\n");
  const QString g_err_notenoughmem(VSPART_ERROR"Not enough mem to alloc %1, in {%2}.\n");
  // const QString g_err_refcount_nonzeroremoval(VSPART_ERROR"Destructing referenced object.\n");
  const QString g_err_list_corrupted(VSPART_ERROR"a [%1] list is corrupted, in {%2}.\n");
  const QString g_err_nullptr(VSPART_ERROR"Null pointer at {%1}.\n");
  const QString g_err_slnactivate(VSPART_ERROR"sln \"%1\" is failed to be activated.\n");
  const QString g_err_slnload(VSPART_ERROR"\"%1\" solution failed to load.\n");
  const QString g_err_prjload(VSPART_ERROR"\"%1\" project is not loaded, in {%2}.\n");
  const QString g_err_ent_notfound(VSPART_ERROR"[ %1 ]:[ %2 ] entity is not found, in {%3}.\n");
  const QString g_err_unsupportedtyp(VSPART_ERROR"Type [%1] is unsupported, in {%2}.\n");
  const QString g_wrn_unsupportedtyp(VSPART_WARNING"Type [%1] is unsupported, in {%2}.\n");
  const QString g_err_fileread(VSPART_ERROR"Can't read file \"%1\", in {%2}.\n");
  const QString g_err_filewrite(VSPART_ERROR"Can't write file \"%1\", in {%2}.\n");
  const QString g_err_domelemnotpresent(VSPART_ERROR"Can't find dom element [ %1 ], in {%2}.\n");
  // Project params read/write
  const QString g_err_pcfgattr_read(VSPART_ERROR"PRJ [%1] param \"%2\" read error.\n");
};