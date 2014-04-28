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

namespace VStudio {
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
    return uid.toString();
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
      case vs_solution: return VSPART_SOLUTION;
      case vs_project: return VSPART_PROJECT;
      case vs_filter: return VSPART_FILTER;
      case vs_file: return VSPART_FILE;
      default: return "unknown";
    }
  }

  e_VSEntityType string2Type(const QString &typ) {
    if(typ == VSPART_SOLUTION) return vs_solution;
    else if(typ == VSPART_PROJECT) return vs_project;
    else if(typ == VSPART_FILTER) return vs_filter;
    else if(typ == VSPART_FILE) return vs_file;
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
};